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
    property bool isRunning: false

    //типы измерений
    property var measurementTypes: [
        "S11", "S12", "S21", "S22", "A(1)", "B(1)", "A(2)", "B(2)", "R1(1)", "R1(2)", "R2(1)", "R2(2)"
    ]

    // Единицы измерений
    property var measurementUnits: [
        "Амп.лог", "КСВН", "Фаза", "Фаза>180", "ГВЗ",
        "Амп лин", "Реал", "Мним"
    ]

    function getPortFromType(type) {
        if (type.endsWith("(1)")) return 1
        if (type.endsWith("(2)")) return 2
        return 0
    }
    function getSweepTypeFromCombo(comboIndex) {
        switch(comboIndex) {
            case 0: return "LIN"  // Лин → LINear
            case 1: return "LOG"  // Лог → LOGarithmic
            case 2: return "SEGM" // Сегм → SEGMent
            case 3: return "POW"  // Мощн → POWer
            default: return "LIN"
        }
    }

    function getCleanType(type) {

        if (type.endsWith("(1)") || type.endsWith("(2)")) {
            return type.substring(0, type.length - 3)
        }
        return type
    }
    function getPowerLabel() {
        let sweepType = getSweepTypeFromCombo(stimCombo.currentIndex)
        if (sweepType === "POW") {
            return "Частота (кГц)"
        } else {
            return "Мощность (дБм)"
        }
    }


    function getPowerPlaceholder() {
        let sweepType = getSweepTypeFromCombo(stimCombo.currentIndex)
        if (sweepType === "POW") {
            return "100"
        } else {
            return "0"
        }
    }


    function getCurrentPowerValue() {
        let sweepType = getSweepTypeFromCombo(stimCombo.currentIndex)
        if (sweepType === "POW") {

            return parseInt(powerBandInput.text || "100")
        } else {

            return parseFloat(powerBandInput.text || "0")
        }
    }
    ListModel {id: graphModel}

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
        Component.onCompleted: {
            Qt.callLater(function() {
                popup.open()
            })
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
                Layout.preferredHeight: 250
                clip: true
                spacing: 4
                boundsBehavior: Flickable.StopAtBounds

                delegate: Rectangle {
                    id: delegateItem
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

                        // Тип измерения
                        ComboBox {
                            id: typeCombo
                            width: 70
                            height: 26
                            model: measurementTypes
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
                                height: 170
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
                                let originalType = measurementTypes[currentIndex]
                                let cleanType = getCleanType(originalType)
                                let port = getPortFromType(originalType)

                                graphModel.setProperty(index, "typeIndex", currentIndex)
                                graphModel.setProperty(index, "port", port)
                                graphModel.setProperty(index, "cleanType", cleanType)

                                console.log(`Тип: ${originalType} -> ${cleanType}, порт: ${port}`)
                                notifyC()
                            }
                        }

                        // Единицы измерений
                        ComboBox {
                            id: unitCombo
                            width: 150
                            height: 26
                            model: measurementUnits
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
                                height: 170
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
                                graphModel.setProperty(index, "unitIndex", currentIndex)
                                notifyC()
                            }
                        }

                        // Кнопка удаления
                        Rectangle {
                            width: 22
                            height: 22
                            radius: 4
                            color: "transparent"
                            border.color: "#555"
                            z: 999

                            Rectangle {
                                anchors.centerIn: parent
                                width: 10
                                height: 2
                                color: "#fff"
                                rotation: 45
                            }
                            Rectangle {
                                anchors.centerIn: parent
                                width: 10
                                height: 2
                                color: "#fff"
                                rotation: -45
                            }

                            MouseArea {
                                anchors.fill: parent
                                z: 1000
                                preventStealing: true
                                propagateComposedEvents: true

                                onClicked: {
                                    console.log("❌ DELETE graph", index)
                                    graphModel.remove(index)

                                    for (let i = 0; i < graphModel.count; ++i)
                                        graphModel.setProperty(i, "num", i + 1)

                                    notifyC()
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
                        let typeIndex = 0
                        let originalType = measurementTypes[typeIndex]
                        let cleanType = getCleanType(originalType)
                        let port = getPortFromType(originalType)

                        graphModel.append({
                            num: graphModel.count + 1,
                            typeIndex: typeIndex,
                            unitIndex: 0,
                            port: port,
                            cleanType: cleanType
                        })
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
    Text { text: "Начальная ƒ (кГц)"; color: "#666"; font.pixelSize: 10; anchors.left: parent.left; anchors.top: parent.top; anchors.leftMargin: 8; anchors.topMargin: 428 }
    Rectangle {
        id: startFreq; x: 8; y: 447; width: 78; height: 36; radius: 6; color: "#2a2a2a"; border.color: "#444"
        onActiveFocusChanged: { if (activeFocus) Qt.inputMethod.hide() }
        TextInput {
            id: startFreqInput
            readOnly: isRunning
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            anchors.fill: parent; anchors.margins: 8; color: isRunning ? "#708090" : "#e0e0e0"; font.pixelSize: 16
            onTextChanged: {
                if (readOnly) return
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
            Text { text: "100"; color: Qt.rgba(0.87, 0.87, 0.87, 0.1); anchors.centerIn: parent; visible: startFreqInput.text.length === 0 && !startFreqInput.activeFocus; font.pixelSize: 16 }
        }
    }

    // Конечная частота
    Text { color: "#666666"; text: "Конечная ƒ (кГц)"; font.pixelSize: 10; anchors.left: parent.left; anchors.top: parent.top; anchors.leftMargin: 8; anchors.topMargin: 489 }
    Rectangle {
        id: stopFreq; x: 8; y: 509; width: 78; height: 36; radius: 6; color: "#2a2a2a"; border.color: "#444"
        onActiveFocusChanged: { if (activeFocus) Qt.inputMethod.hide() }
        TextInput {
            id: stopFreqInput
            readOnly: isRunning
            anchors.fill: parent; anchors.margins: 8; color: isRunning ? "#708090" : "#e0e0e0"; font.pixelSize: 16
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            onTextChanged: {
                if (readOnly) return
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
            Text { text: "4800000";  color: Qt.rgba(0.87, 0.87, 0.87, 0.1); anchors.centerIn: parent; visible: stopFreqInput.text.length === 0 && !stopFreqInput.activeFocus; font.pixelSize: 16 }
        }
    }

    // Количество точек
    Text { color: "#666666"; text: "Кол-во точек"; font.pixelSize: 10; anchors.left: parent.left; anchors.top: parent.top; anchors.leftMargin: 120; anchors.topMargin: 489 }
    Rectangle {
        id: numberOfPoints; x: 111; y: 509; width: 78; height: 36; radius: 6; color: "#2a2a2a"; border.color: "#444"
        onActiveFocusChanged: { if (activeFocus) Qt.inputMethod.hide() }
        TextInput {
            readOnly: isRunning
            id: numberOfPointsInput
            anchors.fill: parent; anchors.margins: 8; color: isRunning ? "#708090" : "#e0e0e0"; font.pixelSize: 16
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            onTextChanged: {
                if (readOnly) return
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
            Text { text: "201";  color: Qt.rgba(0.87, 0.87, 0.87, 0.1); anchors.centerIn: parent; visible: numberOfPointsInput.text.length === 0 && !numberOfPointsInput.activeFocus; font.pixelSize: 16 }
        }
    }

    // Полоса ПЧ
    Rectangle { id: freqBand; x: 111; y: 447; width: 78; height: 36; color: "#2a2a2a"; radius: 6; border.color: "#444444"
        TextInput {
            id: freqBandInput
            readOnly: isRunning
            color: isRunning ? "#708090" : "#e0e0e0"
            anchors.fill: parent; anchors.margins: 8; font.pixelSize: 16
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            onTextChanged: {
                if (readOnly) return
                let clean = text.replace(/[^0-9]/g, "")
                if (clean.length > 5) clean = clean.slice(0,5)
                if (clean !== text) text = clean
            }
            Text { visible: freqBandInput.text.length === 0 && !freqBandInput.activeFocus; color: Qt.rgba(0.87, 0.87, 0.87, 0.1); text: "10000"; font.pixelSize: 16; anchors.centerIn: parent }
            Keys.onReturnPressed: {
                let value = parseInt(text)
                if (isNaN(value) || text === "") value = 10000
                else if (value < 1) value = 1
                else if (value > 30000) value = 30000
                text = value.toString()
                focus = false
                console.log("✅ Полоса ПЧ:", text)
            }
        }
    }

    // Кнопка Start/Stop
    Button {
        id: startStopButton
        property bool running: false
        x: 8; y: 585; width: 419; height: 40; font.pixelSize: 16
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
                if (startFreqInput.text === "") startFreqInput.text = "100"
                if (stopFreqInput.text === "") stopFreqInput.text = "4800000"
                if (numberOfPointsInput.text === "") numberOfPointsInput.text = "201"
                if (freqBandInput.text === "") freqBandInput.text = "10000"
                if (numberOf_IP_Input.text === "") numberOf_IP_Input.text = "127.0.0.1"
                if (numberOfPortInput.text === "") numberOfPortInput.text = "5025"
                if (powerBandInput.text === "") powerBandInput.text = getPowerPlaceholder()
                let sweepType = getSweepTypeFromCombo(stimCombo.currentIndex)
                let powerValue, frequencyValue
                if (sweepType === "POW") {
                    frequencyValue = parseInt(powerBandInput.text)
                    powerValue = 0
                } else {
                    powerValue = parseFloat(powerBandInput.text)
                    frequencyValue = 100
                }
                if (isNaN(powerValue)) powerValue = 0
                if (isNaN(frequencyValue)) frequencyValue = 100
                running = true
                isRunning = true
                if (mainWidget) {
                    mainWidget.startScanFromQml(numberOf_IP_Input.text,
                                                parseInt(numberOfPortInput.text),
                                                parseInt(startFreqInput.text),
                                                parseInt(stopFreqInput.text),
                                                parseInt(numberOfPointsInput.text),
                                                parseInt(freqBandInput.text),
                                                powerValue,
                                                frequencyValue)
                    notifyC()
                }
            } else {
                running = false
                isRunning = false
                if (vnaClient) {
                    vnaClient.stopScan()
                }
            }
        }
    }
    //Данные порта
    Rectangle {
        id: numberOfPort
        x: 356
        y: 447
        width: 71
        height: 36
        color: "#2a2a2a"
        radius: 6
        border.color: "#444444"
        TextInput {
            readOnly: isRunning
            id: numberOfPortInput
            color: isRunning ? "#708090" : "#e0e0e0"
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            anchors.margins: 8
            font.pixelSize: 16
            onTextChanged: {
                if (readOnly) return
                let clean = text.replace(/[^0-9]/g, "")
                if (clean.length > 4) clean = clean.slice(0,4)
                if (clean !== text) text = clean

            }
            Text {
                visible: numberOfPortInput.text.length === 0 && !numberOfPortInput.activeFocus
                color: Qt.rgba(0.87, 0.87, 0.87, 0.1)
                text: "5025"
                font.pixelSize: 16
                anchors.centerIn: parent
            }
            Keys.onReturnPressed: {
                let value = parseInt(text)
                if (isNaN(value) || text === "") value = 5025
                text = value.toString()
                focus = false

            }
        }
    }
    //IP Устройства
    Rectangle {
        id: numberOf_IP
        x: 211
        y: 447
        width: 128
        height: 36
        color: "#2a2a2a"
        radius: 6
        border.color: "#444444"

        TextInput {
            id: numberOf_IP_Input
            color: isRunning ? "#708090" : "#e0e0e0"
            anchors.fill: parent
            anchors.margins: 8
            font.pixelSize: 16
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            inputMethodHints: Qt.ImhDigitsOnly

            onTextChanged: {
                let clean = text.replace(/[^0-9.]/g, "");


                let parts = clean.split(".");
                let validParts = [];

                for (let i = 0; i < parts.length && i < 4; i++) {
                    let p = parts[i];

                    if (p.length > 3) p = p.slice(0, 3);
                    validParts.push(p);
                }


                let formatted = validParts.join(".");
                formatted = formatted.replace(/\.{2,}/g, ".");

                if (formatted !== text)
                    text = formatted;
            }

            Text {
                visible: numberOf_IP_Input.text.length === 0 && !numberOf_IP_Input.activeFocus
                color: Qt.rgba(0.87, 0.87, 0.87, 0.1)      // плейсхолдер тоже темнеет
                text: "127.0.0.1"

                font.pixelSize: 16
                anchors.centerIn: parent
            }

            Keys.onReturnPressed: {
                if (text === "" || !/^(\d{1,3}\.){3}\d{1,3}$/.test(text))
                    text = "127.0.0.1"; // значение по умолчанию
                focus = false;
                console.log("✅ IP введён:", text);
            }
        }
    }

    Text {
        color: "#666666"
        text: "IP устройства"
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 244
        anchors.topMargin: 428
        font.pixelSize: 10
    }

    Text {
        width: 28
        height: 16
        color: "#666666"
        text: "Порт"
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 378
        anchors.topMargin: 427
        font.pixelSize: 10
    }

    Text {
        width: 34
        height: 14
        color: "#666666"
        text: "ПЧ (Гц)"
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 133
        anchors.topMargin: 428
        font.pixelSize: 10
    }

    Rectangle {
        id: powerBand
        x: 211
        y: 509
        width: 72
        height: 36
        color: "#2a2a2a"
        radius: 6
        border.color: "#444444"

        TextInput {
            id: powerBandInput
            readOnly: isRunning
            color: isRunning ? "#708090" : "#e0e0e0"
            anchors.fill: parent
            anchors.margins: 8
            font.pixelSize: 16
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter

            onTextChanged: {
                if (readOnly) return

                let sweepType = getSweepTypeFromCombo(stimCombo.currentIndex)
                let clean

                if (sweepType === "POW") {

                    clean = text.replace(/[^0-9]/g, "")
                    if (clean.length > 7) clean = clean.slice(0,7)
                } else {

                    clean = text.replace(/[^0-9.-]/g, "")
                    if (clean.length > 5) clean = clean.slice(0,5)
                }

                if (clean !== text) text = clean
            }

            Text {
                visible: powerBandInput.text.length === 0 && !powerBandInput.activeFocus
                color: Qt.rgba(0.87, 0.87, 0.87, 0.1)
                text: getPowerPlaceholder()
                font.pixelSize: 16
                anchors.centerIn: parent
            }

            Keys.onReturnPressed: {
                let sweepType = getSweepTypeFromCombo(stimCombo.currentIndex)
                let value

                if (sweepType === "POW") {
                    value = parseInt(text)
                    if (isNaN(value) || text === "") value = 100
                    else if (value < 100) value = 100
                    else if (value > 4800000) value = 4800000
                } else {

                    value = parseFloat(text)
                    if (isNaN(value) || text === "") value = 0
                    else if (value < -60) value = -60
                    else if (value > 15) value = 15
                }

                text = value.toString()
                focus = false
            }
        }
    }

    Text {
        id: powerLabel
        color: "#666666"
        text: getPowerLabel()
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 211
        anchors.topMargin: 489
        font.pixelSize: 10
    }
//тип сканирования
    ComboBox {
        id: stimCombo
        x: 310
        y: 509
        width: 103
        height: 36
        model: ["Лин", "Лог", "Сегм", "Мощн"]
        currentIndex: 0
        font.pixelSize: 16
        indicator: null

        background: Rectangle {
            color: "#282828"
            opacity: 0.7
            radius: 4
        }


        contentItem: Text {
            text: stimCombo.currentText
            color: "#e0e0e0"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        popup: Popup {
            id: popupStim
            y: stimCombo.height
            width: stimCombo.width
            height: 90
            padding: 15
            implicitWidth: stimCombo.width

            background: Rectangle {
                color: "#b3282828"
                radius: 4
            }

            contentItem: ListView {
                model: stimCombo.model
                clip: true
                delegate: ItemDelegate {
                    width: parent.width
                    height: 26
                    background: Rectangle {
                        color: hovered ? "#505050" : Qt.rgba(40/255,40/255,40/255,0.7)
                    }
                    contentItem: Text {
                        text: modelData
                        padding: 15
                        color: "#e0e0e0"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            stimCombo.currentIndex = index
                            popupStim.close()
                        }
                    }
                }
            }
        }
        onCurrentIndexChanged: {
            let sweepType = getSweepTypeFromCombo(currentIndex)
                   console.log("Тип сканирования изменен на:", sweepType)
                   powerLabel.text = getPowerLabel()
                   powerBandInput.text = ""
                   notifyC()
           }
    }
    // Функция уведомления C++
    function notifyC() {
        Qt.callLater(function() {
            let info = []
            for (let i = 0; i < graphModel.count; ++i) {
                let g = graphModel.get(i)
                let originalType = measurementTypes[g.typeIndex]
                let cleanType = getCleanType(originalType)
                let unit = measurementUnits[g.unitIndex]
                let port = getPortFromType(originalType)

                info.push({
                    num: g.num,
                    type: cleanType,
                    unit: unit,
                    port: port
                })

                console.log(`График #${g.num} → ${cleanType} — ${unit} [Порт: ${port}]`)
            }
            let sweepType = getSweepTypeFromCombo(stimCombo.currentIndex)
            let powerSpanValue = 0
            if (sweepType === "POW") {
                powerSpanValue = parseFloat(powerBandInput.text || "100")
            }
            let params = {
                startFreq: parseInt(startFreqInput.text),
                stopFreq: parseInt(stopFreqInput.text),
                numberOfPoints: parseInt(numberOfPointsInput.text),
                freqBand: parseInt(freqBandInput.text),
                sweepType: sweepType,
                powerSpan: powerSpanValue
            }
            console.log("Отправка настроек графиков в C++... Тип сканирования:", sweepType)
            if (mainWidget) {
                mainWidget.applyGraphSettings(info, params)
            }
        })
    }
}
