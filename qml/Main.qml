import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: root
    width: 520
    height: 720
    visible: true
    title: "轻图 · Qt Quick"
    color: "#1A2629"

    DropArea {
        anchors.fill: parent
        onDropped: drop => {
            if (drop.hasUrls) {
                const paths = []
                for (let i = 0; i < drop.urls.length; ++i)
                    paths.push(drop.urls[i].toLocalFile())
                Qingtu.addPaths(paths)
            }
        }
    }

    ScrollView {
        anchors.fill: parent
        anchors.margins: 16
        contentWidth: availableWidth

        ColumnLayout {
            width: parent.width
            spacing: 12

            Label {
                text: "轻图"
                font.pixelSize: 28
                font.bold: true
                color: "#48C4B6"
                Layout.alignment: Qt.AlignHCenter
            }
            Label {
                text: "把体积压下去，细节留住 · 本地处理"
                color: "#828F96"
                font.pixelSize: 11
                Layout.alignment: Qt.AlignHCenter
            }

            RowLayout {
                Button {
                    text: "压缩"
                    highlighted: Qingtu.compressMode
                    onClicked: Qingtu.compressMode = true
                }
                Button {
                    text: "拼接"
                    highlighted: !Qingtu.compressMode
                    onClicked: Qingtu.compressMode = false
                }
            }

            Rectangle {
                Layout.fillWidth: true
                height: 120
                radius: 6
                color: "#1E2C30"
                border.color: "#48C4B6"

                ColumnLayout {
                    anchors.centerIn: parent
                    Label {
                        text: Qingtu.fileNames.length > 0
                              ? ("已选 " + Qingtu.fileNames.length + " 个文件")
                              : "拖拽图片到这里"
                        color: Qingtu.fileNames.length > 0 ? "#48C4B6" : "#828F96"
                    }
                    Button {
                        text: "选择图片"
                        highlighted: true
                        onClicked: Qingtu.pickFiles()
                    }
                }
            }

            ListView {
                Layout.fillWidth: true
                height: 100
                model: Qingtu.fileNames
                clip: true
                delegate: Label { text: modelData; color: "#D2DADE" }
            }

            ColumnLayout {
                visible: Qingtu.compressMode
                Layout.fillWidth: true
                Label { text: "质量 " + Qingtu.quality; color: "#D2DADE" }
                Slider {
                    from: 1; to: 100; value: Qingtu.quality
                    onMoved: Qingtu.quality = value
                }
                RowLayout {
                    Label { text: "最长边"; color: "#D2DADE" }
                    SpinBox {
                        from: 0; to: 10000; value: Qingtu.maxSide
                        onValueModified: Qingtu.maxSide = value
                    }
                    Label { text: "px（0 = 不缩放）"; color: "#828F96" }
                }
                ComboBox {
                    model: [
                        { k: "keep", t: "保持原格式" },
                        { k: "jpeg", t: "JPEG" },
                        { k: "png", t: "PNG" },
                        { k: "webp", t: "WebP" }
                    ]
                    textRole: "t"
                    onActivated: Qingtu.format = model[currentIndex].k
                }
            }

            Label {
                visible: !Qingtu.compressMode
                text: "按列表顺序横向拼接，高度对齐最短边"
                color: "#828F96"
                wrapMode: Text.Wrap
                Layout.fillWidth: true
            }

            CheckBox {
                text: "自定义输出目录"
                checked: Qingtu.customOut
                onToggled: Qingtu.customOut = checked
            }
            RowLayout {
                TextField {
                    Layout.fillWidth: true
                    text: Qingtu.outDir
                    onTextEdited: Qingtu.outDir = text
                }
                Button { text: "浏览…"; onClicked: Qingtu.pickOutDir() }
            }

            RowLayout {
                Button {
                    text: Qingtu.compressMode
                          ? ("开始压缩 (" + Qingtu.fileNames.length + ")")
                          : ("开始拼接 (" + Qingtu.fileNames.length + ")")
                    highlighted: true
                    enabled: !Qingtu.busy
                    onClicked: Qingtu.run()
                }
                Button {
                    text: "清空列表"
                    onClicked: Qingtu.clearFiles()
                }
            }

            TextArea {
                Layout.fillWidth: true
                Layout.minimumHeight: 80
                readOnly: true
                text: Qingtu.logText
                color: "#D2DADE"
                font.family: "Consolas"
                wrapMode: TextArea.Wrap
            }
        }
    }
}
