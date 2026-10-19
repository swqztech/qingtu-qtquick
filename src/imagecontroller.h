#pragma once

#include "image_job.h"

#include <QObject>
#include <QStringList>
#include <filesystem>
#include <vector>

class ImageController : public QObject {
  Q_OBJECT
  Q_PROPERTY(QStringList fileNames READ fileNames NOTIFY filesChanged)
  Q_PROPERTY(QString logText READ logText NOTIFY logChanged)
  Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
  Q_PROPERTY(bool compressMode READ compressMode WRITE setCompressMode NOTIFY modeChanged)
  Q_PROPERTY(int quality READ quality WRITE setQuality NOTIFY qualityChanged)
  Q_PROPERTY(int maxSide READ maxSide WRITE setMaxSide NOTIFY maxSideChanged)
  Q_PROPERTY(QString format READ format WRITE setFormat NOTIFY formatChanged)
  Q_PROPERTY(bool customOut READ customOut WRITE setCustomOut NOTIFY customOutChanged)
  Q_PROPERTY(QString outDir READ outDir WRITE setOutDir NOTIFY outDirChanged)

 public:
  explicit ImageController(QObject* parent = nullptr);

  QStringList fileNames() const;
  QString logText() const { return log_; }
  bool busy() const { return busy_; }
  bool compressMode() const { return compress_mode_; }
  int quality() const { return quality_; }
  int maxSide() const { return max_side_; }
  QString format() const { return format_; }
  bool customOut() const { return custom_out_; }
  QString outDir() const { return out_dir_; }

  void setCompressMode(bool v);
  void setQuality(int v);
  void setMaxSide(int v);
  void setFormat(const QString& v);
  void setCustomOut(bool v);
  void setOutDir(const QString& v);

  Q_INVOKABLE void addPaths(const QStringList& paths);
  Q_INVOKABLE void clearFiles();
  Q_INVOKABLE void run();

 signals:
  void filesChanged();
  void logChanged();
  void busyChanged();
  void modeChanged();
  void qualityChanged();
  void maxSideChanged();
  void formatChanged();
  void customOutChanged();
  void outDirChanged();

 private:
  JobOptions buildOptions() const;
  std::vector<std::filesystem::path> files_;
  QString log_;
  bool busy_ = false;
  bool compress_mode_ = true;
  int quality_ = 80;
  int max_side_ = 0;
  QString format_ = QStringLiteral("keep");
  bool custom_out_ = false;
  QString out_dir_;
};
