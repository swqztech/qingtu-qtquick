#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
STAGE="$ROOT/packaging/stage"
DIST="$ROOT/dist"
VERSION="$(grep -E 'project\(.*VERSION' "$ROOT/CMakeLists.txt" | sed -E 's/.*VERSION\s+([0-9.]+).*/\1/')"
NAME="轻图-qtquick-${VERSION}-win64-绿色版"

find_qt() {
  [[ -n "${CMAKE_PREFIX_PATH:-}" && -d "$CMAKE_PREFIX_PATH" ]] && { echo "$CMAKE_PREFIX_PATH"; return; }
  local repo_qt="$ROOT/../.qt"
  if [[ -d "$repo_qt" ]]; then
    find "$repo_qt" -maxdepth 2 -type d -name 'msvc*_64' 2>/dev/null | sort -r | head -1
  fi
}

CMAKE="$(command -v cmake 2>/dev/null || true)"
QT="$(find_qt || true)"
[[ -n "$CMAKE" && -n "$QT" ]] || { echo "需要 cmake 与 Qt6 (CMAKE_PREFIX_PATH)"; exit 1; }

(cd "$ROOT" && "$CMAKE" -B build -DCMAKE_PREFIX_PATH="$QT" && "$CMAKE" --build build --config Release)
EXE_SRC="$ROOT/build/Release/qingtu-qtquick.exe"
[[ -f "$EXE_SRC" ]] || { echo "未找到 exe"; exit 1; }

PORTABLE_DIR="$DIST/$NAME"
rm -rf "$DIST"
mkdir -p "$PORTABLE_DIR"
cp -f "$EXE_SRC" "$PORTABLE_DIR/qingtu-qtquick.exe"
cp -f "$STAGE/"* "$PORTABLE_DIR/"
"$QT/bin/windeployqt.exe" --qmldir "$ROOT/qml" --no-compiler-runtime --no-translations \
  "$PORTABLE_DIR/qingtu-qtquick.exe"

(cd "$DIST" && tar -a -cf "${NAME}.zip" "$NAME")
echo "绿色版: $PORTABLE_DIR"
echo "压缩包: $DIST/${NAME}.zip"
