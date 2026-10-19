#include "imagecontroller.h"

#include <QtConcurrent/QtConcurrent>

#include <QFileDialog>
#include <QFutureWatcher>

#include <algorithm>

QStringList ImageController::fileNames() const {
  QStringList names;
  for (const auto& p : files_) names << QString::fromStdWString(p.filename().wstring());
  return names;
}

ImageController::ImageController(QObject* parent) : QObject(parent) {}

void ImageController::setCompressMode(bool v) {
  if (compress_mode_ == v) return;
  compress_mode_ = v;
  emit modeChanged();
}

void ImageController::setQuality(int v) {
  if (quality_ == v) return;
  quality_ = v;
  emit qualityChanged();
}

void ImageController::setMaxSide(int v) {
  if (max_side_ == v) return;
  max_side_ = v;
  emit maxSideChanged();
}

void ImageController::setFormat(const QString& v) {
  if (format_ == v) return;
  format_ = v;
  emit formatChanged();
}

void ImageController::setCustomOut(bool v) {
  if (custom_out_ == v) return;
  custom_out_ = v;
  emit customOutChanged();
}

void ImageController::setOutDir(const QString& v) {
  if (out_dir_ == v) return;
  out_dir_ = v;
  emit outDirChanged();
}

static bool is_image_path(const std::filesystem::path& path) {
  const auto ext = path.extension().wstring();
  static const wchar_t* ok[] = {L".png", L".jpg", L".jpeg", L".webp", L".gif", L".bmp"};
  for (auto* e : ok) {
    if (_wcsicmp(ext.c_str(), e) == 0) return true;
  }
  return false;
}

void ImageController::addPaths(const QStringList& paths) {
  for (const auto& p : paths) {
    const auto path = std::filesystem::path(p.toStdWString());
    if (!is_image_path(path)) continue;
    if (std::find(files_.begin(), files_.end(), path) != files_.end()) continue;
    files_.push_back(path);
  }
  emit filesChanged();
}

void ImageController::pickFiles() {
  const auto paths = QFileDialog::getOpenFileNames(
      nullptr, QStringLiteral("选择图片"), {},
      QStringLiteral("图片 (*.png *.jpg *.jpeg *.webp *.gif *.bmp);;所有文件 (*.*)"));
  addPaths(paths);
}

void ImageController::pickOutDir() {
  const QString dir = QFileDialog::getExistingDirectory(nullptr, QStringLiteral("输出目录"));
  if (dir.isEmpty()) return;
  setOutDir(dir);
  setCustomOut(true);
}

void ImageController::clearFiles() {
  files_.clear();
  log_.clear();
  emit filesChanged();
  emit logChanged();
}

JobOptions ImageController::buildOptions() const {
  JobOptions o;
  o.quality = quality_;
  o.max_side = max_side_;
  if (format_ == QLatin1String("jpeg")) o.format = OutputFormat::Jpeg;
  else if (format_ == QLatin1String("png")) o.format = OutputFormat::Png;
  else if (format_ == QLatin1String("webp")) o.format = OutputFormat::WebP;
  else o.format = OutputFormat::KeepOriginal;
  o.use_custom_out = custom_out_;
  if (o.use_custom_out) o.output_dir = std::filesystem::path(out_dir_.toStdWString());
  return o;
}

static QString formatJobLog(const std::vector<JobResult>& results) {
  QStringList lines;
  for (const auto& r : results) {
    const auto in = QString::fromStdWString(r.input.wstring());
    if (r.message != "完成") {
      lines << QString("%1 → %2").arg(in, QString::fromStdString(r.message));
      continue;
    }
    const qint64 saved = static_cast<qint64>(r.bytes_before - r.bytes_after);
    lines << QString("%1 → %2 (%3 KB 节省)")
                 .arg(in, QString::fromStdWString(r.output.wstring()))
                 .arg(saved / 1024);
  }
  return lines.join('\n');
}

void ImageController::run() {
  if (busy_) return;
  if (files_.empty()) {
    log_ = QStringLiteral("请先添加图片");
    emit logChanged();
    return;
  }
  busy_ = true;
  emit busyChanged();

  const auto opts = buildOptions();
  const bool compress = compress_mode_;
  const auto inputs = files_;

  auto* watcher = new QFutureWatcher<QString>(this);
  connect(watcher, &QFutureWatcher<QString>::finished, this, [this, watcher]() {
    log_ = watcher->result();
    busy_ = false;
    emit logChanged();
    emit busyChanged();
    watcher->deleteLater();
  });
  watcher->setFuture(QtConcurrent::run([compress, inputs, opts]() -> QString {
    if (compress) return formatJobLog(run_compress_jobs(inputs, opts));
    if (inputs.size() < 2) return QStringLiteral("拼接需要至少 2 张图");
    std::filesystem::path out;
    if (opts.use_custom_out && !opts.output_dir.empty())
      out = opts.output_dir / "stitched.png";
    else if (!inputs.empty()) {
      auto parent = inputs.front().parent_path();
      out = (parent.empty() ? std::filesystem::path("compressed") : parent / "compressed") /
            "stitched.png";
    }
    std::string err;
    if (!stitch_horizontal(inputs, out, err)) return QString::fromStdString(err);
    return QStringLiteral("拼接完成 → %1").arg(QString::fromStdWString(out.wstring()));
  }));
}
