# 轻图 · Qt Quick

C++ 图片压缩/拼接（stb）+ **Qt Quick (QML)** 界面，功能对齐 `qingtu-qt`。

## 构建

```powershell
$env:CMAKE_PREFIX_PATH="d:\codes\soft\.qt\6.8.2\msvc2022_64"
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

**不要直接双击 `build\Release` 里的 exe**（会提示缺少 `Qt6Qml.dll` 等）。请打包：

```powershell
bash packaging/build.sh
```

绿色版：`dist\轻图-qtquick-0.1.0-win64-绿色版\qingtu-qtquick.exe`
