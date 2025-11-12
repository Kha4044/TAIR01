import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15
import QtCharts 2.15

Rectangle {
    id: root
    width: 435
    height: 640
    visible: true
    color: "#1e1e1e"

    // property var graphColors: [
    //     "#ff3b30", "#ff9500", "#ffcc00", "#34c759", "#5ac8fa", "#007aff", "#5856d6", "#af52de",
    //     "#ff2d55", "#a2845e", "#ff9f0a", "#32ade6", "#bf5af2", "#64d2ff", "#30d158", "#ff375f"
    // ]
    property var usedColors: []

    ListModel { id: graphModel }

    // function getNextColor() {
    //     for (let c of graphColors)
    //         if (usedColors.indexOf(c) === -1) {
    //             usedColors.push(c)
    //             return c
    //         }
    //     return graphColors[Math.floor(Math.random() * graphColors.length)]
    // }

    function releaseColor(color) {
        let i = usedColors.indexOf(color)
        if (i !== -1)
            usedColors.splice(i, 1)
    }

    Rectangle {
        id: comboBoxField
        width: 420
        height: 40
        radius: 6
        color: "#2e2e2e"
        border.color: "#555"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 34
        anchors.horizontalCenterOffset: 0

        Text {
            anchors.centerIn: parent
            text: "Графики (" + graphModel.count + ")"
            color: "#e0e0e0"
            font.family: "Consolas"
            font.pixelSize: 16
        }

        MouseArea {
            anchors.fill: parent
            anchors.leftMargin: -2
            anchors.rightMargin: 2
            anchors.topMargin: 1
            anchors.bottomMargin: -1
            onClicked: {
                if (popup.visible) {
                    popup.forceClose = true
                    popup.close()
                    popup.forceClose = false
                } else {
                    popup.open()
                }
            }
        }
    }
    Popup {
        id: popup
        width: comboBoxField.width
        x: comboBoxField.x
        y: comboBoxField.y + comboBoxField.height
        focus: true
        closePolicy: popup.NoAutoClose
        property bool forceClose: false
        onClosed: {
            if (!forceClose) popup.open()
        }

        background: Rectangle {
            radius: 6
            color: "#202020"
            border.color: "#555"
        }

        ColumnLayout {
            anchors.fill: parent
            spacing: 2
            anchors.margins: 6

            ListView {
                id: listView
                model: graphModel
                Layout.fillWidth: true
                Layout.preferredHeight: 300
                clip: true
                spacing: 4
                boundsBehavior: Flickable.StopAtBounds

                delegate: Rectangle {
                    width: parent.width
                    height: 34
                    color: hovered ? "#333333" : "#282828"
                    property bool hovered: false

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: parent.hovered = true
                        onExited: parent.hovered = false
                    }

                    Row {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: 15
                        anchors.rightMargin: 5
                        spacing: 15

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            text: "гр" + model.num
                            color: "#e0e0e0"
                            font.family: "Consolas"
                            font.pixelSize: 16
                            width: 40
                        }

                        ComboBox {
                            id: typeCombo
                            width: 70
                            height: 26
                            model: ["S11", "S12", "S21", "S22"]
                            currentIndex: model.typeIndex
                            font.pixelSize: 16
                            indicator: null

                            background: Rectangle {
                                color: "#282828"
                                border.width: 0
                                radius: 4
                            }

                            contentItem: Text {
                                text: typeCombo.currentText
                                color: "#e0e0e0"
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                            }

                            popup: Popup {
                                id: popupType
                                y: typeCombo.height
                                width: typeCombo.width
                                height: 200
                                background: Rectangle {
                                    color: "#b3282828"
                                    radius: 4
                                }
                                contentItem: ListView {
                                    model: typeCombo.model
                                    width: parent.width
                                    clip: true
                                    delegate: ItemDelegate {
                                        width: parent.width
                                        background: Rectangle {
                                            color: hovered ? "#505050" : Qt.rgba(40/255,40/255,40/255,0.7)
                                        }
                                        contentItem: Text {
                                            text: modelData
                                            color: "#e0e0e0"
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                typeCombo.currentIndex = index
                                                popupType.close()
                                            }
                                        }
                                    }
                                }
                            }

                            onCurrentIndexChanged: {
                                model.typeIndex = currentIndex
                                graphModel.setProperty(index, "typeIndex", currentIndex)
                                notifyC()
                            }
                        }
                        // Единицы измерений
                        ComboBox {
                            id: unitCombo
                            width: 150
                            height: 26
                            model: [
                                "Амп.лог", "КСВН", "Фаза", "Фаза>180", "ГВЗ",
                                "Амп лин", "Реал", "Мним"
                            ]
                            currentIndex: model.unitIndex
                            font.pixelSize: 16
                            indicator: null

                            background: Rectangle {
                                color: "#282828"
                                border.width: 0
                                radius: 4
                            }

                            contentItem: Text {
                                text: unitCombo.currentText
                                color: "#e0e0e0"
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                            }

                            popup: Popup {
                                id: popupUnit
                                y: unitCombo.height
                                width: unitCombo.width
                                height: 200
                                background: Rectangle {
                                    color: "#b3282828"
                                    radius: 4
                                }
                                contentItem: ListView {
                                    model: unitCombo.model
                                    width: parent.width
                                    clip: true
                                    delegate: ItemDelegate {
                                        width: parent.width
                                        background: Rectangle {
                                            color: hovered ? "#505050" : Qt.rgba(40/255,40/255,40/255,0.7)
                                        }
                                        contentItem: Text {
                                            text: modelData
                                            color: "#e0e0e0"
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                unitCombo.currentIndex = index
                                                popupUnit.close()
                                            }
                                        }
                                    }
                                }
                            }

                            onCurrentIndexChanged: {
                                model.unitIndex = currentIndex
                                graphModel.setProperty(index, "unitIndex", currentIndex)
                                notifyC()
                            }
                        }

                        // Кнопка удаления графика
                        Rectangle {
                            width: 22
                            height: 22
                            radius: 4
                            color: "transparent"
                            border.color: "#555"
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    releaseColor(model.color)
                                    graphModel.remove(index)
                                    for (let i = 0; i < graphModel.count; ++i)
                                        graphModel.setProperty(i, "num", i + 1)
                                    notifyC()
                                }
                                Rectangle {
                                    anchors.centerIn: parent
                                    width: 10
                                    height: 2
                                    color: "#1e1e1e"
                                    rotation: 45
                                }
                                Rectangle {
                                    anchors.centerIn: parent
                                    width: 10
                                    height: 2
                                    color: "#1e1e1e"
                                    rotation: -45
                                }
                            }
                        }
                    }
                }
            }

            // Кнопка добавления графика
            Button {
                text: "+ Добавить график"
                Layout.fillWidth: true
                height: 34
                font.pixelSize: 14
                background: Rectangle {
                    color: "#6a9794"
                    radius: 4
                    border.color: "#666"
                }
                onClicked: {
                    if (graphModel.count < 16) {
                        graphModel.append({ num: graphModel.count + 1, typeIndex: 0, unitIndex: 0 })
                        notifyC()
                    }
                }
            }
        }
    }
    // 🟢 Глобальная зона кликов
    MouseArea {
        id: unfocusArea
        anchors.fill: parent
        anchors.rightMargin: 372
        propagateComposedEvents: true
        onPressed: {
            if (!startFreqInput.containsMouse) startFreqInput.focus = false
            if (!stopFreqInput.containsMouse) stopFreqInput.focus = false
            if (!numberOfPointsInput.containsMouse) numberOfPointsInput.focus = false
            if (!freqBandInput.containsMouse) freqBandInput.focus = false
            mouse.accepted = false
        }
    }

    // Начальная частота
    Text { text: "Начальная частота (кГц)"; color: "#666"; font.pixelSize: 12; anchors.left: parent.left; anchors.top: parent.top; anchors.leftMargin: 10; anchors.topMargin: 435 }
    Rectangle {
        id: startFreq; x: 8; y: 463; width: 130; height: 36; radius: 6; color: "#2a2a2a"; border.color: "#444"
        onActiveFocusChanged: { if (activeFocus) Qt.inputMethod.hide() }
        TextInput {
            id: startFreqInput
            anchors.fill: parent; anchors.margins: 8; color: "#e0e0e0"; font.pixelSize: 16
            onTextChanged: {
                let clean = text.replace(/[^0-9]/g, "")
                if (clean.length > 7) clean = clean.slice(0,7)
                if (clean !== text) text = clean
            }
            Keys.onReturnPressed: {
                let value = parseInt(text)
                if (isNaN(value) || text === "") value = 20
                else if (value < 20) value = 20
                else if (value > 4800000) value = 4800000
                text = value.toString()
                focus = false
                console.log("✅ Начальная частота:", text)
            }
            Text { text: "20"; color: "#666"; anchors.centerIn: parent; visible: startFreqInput.text.length === 0 && !startFreqInput.activeFocus; font.pixelSize: 16 }
        }
    }

    // Конечная частота
    Text { color: "#666666"; text: "Конечная частота (кГц)"; font.pixelSize: 12; anchors.left: parent.left; anchors.top: parent.top; anchors.leftMargin: 153; anchors.topMargin: 435 }
    Rectangle {
        id: stopFreq; x: 153; y: 463; width: 130; height: 36; radius: 6; color: "#2a2a2a"; border.color: "#444"
        onActiveFocusChanged: { if (activeFocus) Qt.inputMethod.hide() }
        TextInput {
            id: stopFreqInput
            anchors.fill: parent; anchors.margins: 8; color: "#e0e0e0"; font.pixelSize: 16
            onTextChanged: {
                let clean = text.replace(/[^0-9]/g, "")
                if (clean.length > 7) clean = clean.slice(0,7)
                if (clean !== text) text = clean
            }
            Keys.onReturnPressed: {
                let value = parseInt(text)
                if (isNaN(value) || text === "") value = 20
                else if (value < 20) value = 20
                else if (value > 4800000) value = 4800000
                text = value.toString()
                focus = false
                console.log("✅ Конечная частота:", text)
            }
            Text { text: "4800000"; color: "#666"; anchors.centerIn: parent; visible: stopFreqInput.text.length === 0 && !stopFreqInput.activeFocus; font.pixelSize: 16 }
        }
    }

    // Количество точек
    Text { color: "#666666"; text: "Количество точек"; font.pixelSize: 12; anchors.left: parent.left; anchors.top: parent.top; anchors.leftMargin: 298; anchors.topMargin: 435 }
    Rectangle {
        id: numberOfPoints; x: 298; y: 463; width: 130; height: 36; radius: 6; color: "#2a2a2a"; border.color: "#444"
        onActiveFocusChanged: { if (activeFocus) Qt.inputMethod.hide() }
        TextInput {
            id: numberOfPointsInput
            anchors.fill: parent; anchors.margins: 8; color: "#e0e0e0"; font.pixelSize: 16
            onTextChanged: {
                let clean = text.replace(/[^0-9]/g, "")
                if (clean.length > 4) clean = clean.slice(0,4)
                if (clean !== text) text = clean
            }
            Keys.onReturnPressed: {
                let value = parseInt(text)
                if (isNaN(value) || text === "") value = 201
                else if (value < 201) value = 201
                else if (value > 1000) value = 1000
                text = value.toString()
                focus = false
                console.log("✅ Количество точек:", text)
            }
            Text { text: "201"; color: "#666"; anchors.centerIn: parent; visible: numberOfPointsInput.text.length === 0 && !numberOfPointsInput.activeFocus; font.pixelSize: 16 }
        }
    }

    // Полоса ПЧ
    Text { color: "#d9d9d9"; text: "Полоса фильтра ПЧ ="; font.pixelSize: 16; anchors.left: parent.left; anchors.top: parent.top; anchors.leftMargin: 22; anchors.topMargin: 529 }
    Rectangle { id: freqBand; x: 186; y: 524; width: 98; height: 36; color: "#2a2a2a"; radius: 6; border.color: "#444444"
        TextInput {
            id: freqBandInput
            color: "#e0e0e0"
            anchors.fill: parent; anchors.margins: 8; font.pixelSize: 16
            onTextChanged: {
                let clean = text.replace(/[^0-9]/g, "")
                if (clean.length > 5) clean = clean.slice(0,5)
                if (clean !== text) text = clean
            }
            Text { visible: freqBandInput.text.length === 0 && !freqBandInput.activeFocus; color: "#666666"; text: "10000"; font.pixelSize: 16; anchors.centerIn: parent }
            Keys.onReturnPressed: {
                let value = parseInt(text)
                if (isNaN(value) || text === "") value = 1
                else if (value < 1) value = 1
                else if (value > 30000) value = 30000
                text = value.toString()
                focus = false
                console.log("✅ Полоса ПЧ:", text)
            }
        }
    }
    Text { color: "#d9d9d9"; text: "Гц"; font.pixelSize: 16; anchors.left: parent.left; anchors.top: parent.top; anchors.leftMargin: 290; anchors.topMargin: 529 }

    // Кнопка включения/выключения отображения мощности
    Button {
         id: powerModeButton
         x: 298; y: 586
         width: 120
         height: 36
         text: "Power Measure"
         font.pixelSize: 14
         checkable: true

         contentItem: Text {
             anchors.centerIn: parent
             text: powerModeButton.checked ? "Stop Power" : "Measure Power"
             color: powerModeButton.checked ? "#ffffff" : "#1e1e1e"
             font.pixelSize: 12
             font.bold: true
             horizontalAlignment: Text.AlignHCenter
             wrapMode: Text.Wrap
         }

         background: Rectangle {
             radius: 6
             border.color: "#555"
             color: powerModeButton.checked ? "#ff9500" : "#d9d9d9"
             Behavior on color { ColorAnimation { duration: 250 } }
         }

         onClicked: {
             if (checked) {
                 console.log("🟠 Activating power measurement mode");
                 if (mainWidget) {
                     mainWidget.setPowerMeasuringMode(true);
                 }
                 // Делаем кнопку Start/Stop неактивной в режиме мощности
                 startStopButton.enabled = false;
             } else {
                 console.log("🔵 Deactivating power measurement mode");
                 if (mainWidget) {
                     mainWidget.setPowerMeasuringMode(false);
                 }
                 // Восстанавливаем активность кнопки Start/Stop
                 startStopButton.enabled = true;
             }
         }
     }
    // Кнопка Start/Stop
    Button {
        id: startStopButton
        property bool running: false
        x: 10; y: 582; width: 260; height: 40; font.pixelSize: 16
        contentItem: Text {
            anchors.centerIn: parent
            text: startStopButton.running ? "Stop" : "Start"
            color: startStopButton.running ? "#ffffff" : "#1e1e1e"
            font.pixelSize: 16
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            enabled: true
        }
        background: Rectangle {
            radius: 6
            border.color: "#555"
            color: startStopButton.running ? "#3a3a3a" : "#d9d9d9"
            Behavior on color { ColorAnimation { duration: 250 } }
        }
        onClicked: {
            if (!running) {
                if (startFreqInput.text === "") startFreqInput.text = "20"
                if (stopFreqInput.text === "") stopFreqInput.text = "4800000"
                if (numberOfPointsInput.text === "") numberOfPointsInput.text = "201"
                if (freqBandInput.text === "") freqBandInput.text = "10000"
                startFreqInput.readOnly = true
                stopFreqInput.readOnly = true
                numberOfPointsInput.readOnly = true
                freqBandInput.readOnly = true
                running = true
                if (vnaClient) {
                    vnaClient.startScan(
                        parseInt(startFreqInput.text),
                        parseInt(stopFreqInput.text),
                        parseInt(numberOfPointsInput.text),
                        parseInt(freqBandInput.text)
                    )
                    notifyC()
                }
            } else {
                startFreqInput.readOnly = false
                stopFreqInput.readOnly = false
                numberOfPointsInput.readOnly = false
                freqBandInput.readOnly = false
                running = false
                if (vnaClient) {
                    vnaClient.stopScan()
                }
                if (mainWidget) {
                            mainWidget.forceDataSync();
                        }
            }
        }
    }


    // Функция уведомления C++
    function notifyC() {
        Qt.callLater(function() {
            let info = []
            for (let i = 0; i < graphModel.count; ++i) {
                let g = graphModel.get(i)
                let type = ["S11","S12","S21","S22"][g.typeIndex]
                let unit = [
                    "Амп.лог","КСВН","Фаза","Фаза>180","ГВЗ",
                    "Амп лин","Реал","Мним"
                ][g.unitIndex]

                info.push({
                    num: g.num,
                    type: type,
                    unit: unit
                })

                console.log(`График #${g.num} → ${type} — ${unit}`)
            }
            let params = {
                startFreq: parseInt(startFreqInput.text),
                stopFreq: parseInt(stopFreqInput.text),
                numberOfPoints: parseInt(numberOfPointsInput.text),
                freqBand: parseInt(freqBandInput.text)
            }

            console.log("Отправка настроек графиков в C++...")
            if (mainWidget) {mainWidget.applyGraphSettings(info, params)
            }
        })
    }
}
