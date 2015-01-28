import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.1
import QtQuick.Window 2.0
import XDAT 1.0

ApplicationWindow {
    id: appWin
    minimumWidth: 320
    minimumHeight: 480
    width: 480
    height: 640
    title: qsTr("xdat \u27a5 hdf") //2749
    visible: true

    //color: "ghostwhite"
    color: "aliceblue"

    /*
    toolBar: ToolBar{
        id: toolbar
        RowLayout{
            width: parent.width
            spacing: 0

            CheckBox{
               id: chk_enabled
            }
        }
    }
    */

    function toggleEditing(arg1)
    {
        dims_01.readOnly = arg1;
        dims_02.readOnly = arg1;
        dims_03.readOnly = arg1;

        txt_name.readOnly = arg1;
        txt_deltaTime.readOnly = arg1;
        txt_temperature.readOnly = arg1;
        txt_volAtoms.readOnly = arg1;
        txt_numAtoms.readOnly = arg1;
        txt_numSteps.readOnly = arg1;
    }

    Converter {
        id: xdathdf

        property bool fileIsOpen: false

        onSpecFileChanged: { }
        onSpeciesListChanged: {
            //console.log("Species List changed!!!")
            // Clear and reload the Species Model-View
            //speciesModel.clear()
        }
        onAtomicSystemChanged: { fileIsOpen = true; }
        onAbortExportChanged: {
            if(abortExport)
                btn_convert.enabled = true;
        }
    }

    FileDialog {
        id: filedialog

        property string filename: ""
        property int flag: -1       // User defined to distinct between species and positions file dialog

        //nameFilters: [ "All files (*.*)" ]
        selectFolder: false
        selectMultiple: false

        function getStartFolder()
        {
            if(Qt.platform.os === "osx")
                return "file:///Users/";
            else
                return "";
        }

        folder: getStartFolder()
        onAccepted: {
            filename = fileUrl.toString()
            if(flag == 1) {
                txtPosFile.text = filename
            }
            else if(flag == 0) {
                txtSpeciesFile.text = filename
            }
        }
    }

    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")
            MenuItem {
                text: qsTr("Open Positions File")
                shortcut: "Ctrl+O, P"
                onTriggered: {
                    filedialog.flag = 1
                    filedialog.title = "Positions file"
                    filedialog.open();
                }
            }

            MenuItem {
                text: qsTr("Open Species File")
                shortcut: "Ctrl+O, S"
                onTriggered: {
                    filedialog.flag = 0
                    filedialog.title = "Species file"
                    filedialog.open();
                }
            }

            MenuSeparator{}

            MenuItem {
                text: qsTr("Abort Converter")
                shortcut: "Ctrl+Q"
                onTriggered: { Qt.quit() }
            }
        }
        Menu{
            title: qsTr("&Edit")
            MenuItem {
                text: qsTr("Enable Editing")
                shortcut: "Ctrl+E"
                checkable: true
                checked: false
                onTriggered: {
                    toggleEditing(!checked)
                }
            }
        }
    }

    GroupBox{
        id: group_box1
        y: 0
        //height: 64
        title: "Load Input Files"
        anchors.right: parent.right
        anchors.rightMargin: 6
        anchors.left: parent.left
        anchors.leftMargin: 6

        GridLayout {
            id: grid_layout1
            columns: 2
            rowSpacing: -2
            columnSpacing: 5
            anchors.fill: parent

            TextField {
                id: txtSpeciesFile
                x: 0
                y: 48
                readOnly: true
                Layout.fillWidth: true
                Layout.maximumHeight: 20
                placeholderText: qsTr("Species File")
                font.pointSize: 12

                onTextChanged: {
                    if(xdathdf.specFile != txtSpeciesFile.text)
                        xdathdf.specFile = txtSpeciesFile.text
                }
            }

            Button {
                id: btnSpeciesFile
                y: 48
                text: "Select File"
                Layout.maximumWidth:  80
                anchors.right: parent.right
                //anchors.rightMargin: 0

                onClicked: {
                    //if ( Qt.platform.os === "osx")
                    //    filedialog.folder = "file:///Users/bidurbohara/"
                    filedialog.flag = 0
                    filedialog.title = "Species file"
                    filedialog.open();
                }
            }

            TextField {
                id: txtPosFile
                x: 0
                y: 8
                readOnly: true
                Layout.fillWidth: true
                Layout.maximumHeight: 20
                placeholderText: qsTr("Positions File")
                font.pointSize: 12

                onTextChanged: {
                    if(xdathdf.posFile != txtPosFile.text)
                        xdathdf.posFile = txtPosFile.text
                }
            }

            Button {
                id: btnPosFile
                y: 8
                height: 20
                text: "Select File"
                Layout.maximumWidth:  80
                anchors.right: parent.right
                //anchors.rightMargin: 0

                onClicked: {
                    filedialog.flag = 1
                    filedialog.title = "Positions file"
                    filedialog.open();
                }
            }
        }
    }

    ColumnLayout{
        id: col_infos
        //anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 6
        anchors.right: parent.right
        anchors.rightMargin: 6
        spacing: 12
        anchors.top: group_box1.bottom
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 12

        GroupBox{
            id: group_box2
            checkable: false
            flat: false
            title: qsTr("Species Info.")
            anchors.bottomMargin: 1
            anchors.topMargin: 5

            anchors.top: parent.top
            anchors.right: parent.right
            anchors.left: parent.left
            Layout.maximumHeight: 120

            ListModel{
                id:speciesModel

                ListElement {
                    symbol: "Apple"
                    atomId: 2.45
                    beginIndex: 23
                    endIndex: 25
                }
                ListElement {
                    symbol: "Orange"
                    atomId: 3.25
                    beginIndex: 23
                    endIndex: 25
                }
                ListElement {
                    symbol: "Banana"
                    atomId: 1.95
                    beginIndex: 23
                    endIndex: 25
                }
            }
            TableView{
                id: tblView
                //model: speciesModel
                model: xdathdf.speciesList
                anchors.fill: parent

                property int colWidth: parent.width * 0.24

                TableViewColumn{
                    role: "symbol"
                    title: "Symbol"
                    width: tblView.colWidth
                }
                TableViewColumn {
                    role: "atomId"
                    title: "Atomic Number"
                    width: tblView.colWidth
                }
                TableViewColumn {
                    role: "beginIndex"
                    title: "Start Index"
                    width: tblView.colWidth
                }
                TableViewColumn {
                    role: "endIndex"
                    title: "End Index"
                    width: tblView.colWidth
                }
                Component.onCompleted: {
                    //speciesModel.append({"name": "Si", "elemId": 14, "sindex": 0, "eindex": 12})
                    //speciesModel.append({"name": xdathdf.speciesList[0].symbol, "elemId": 14, "sindex": 0, "eindex": 12})
                    //xdathdf.speciesList[0].symbol;
                }
            }
        }

        GroupBox{
            id: group_box3
            anchors.top: group_box2.bottom
            anchors.topMargin: 6
            anchors.right: parent.right
            anchors.left: parent.left
            title: qsTr("Systems Info.")
            //Layout.maximumHeight: 240

            GridLayout{
                id: grid_layout2
                anchors.fill: parent
                anchors.margins: 3
                columns: 2
                columnSpacing: 5
                rowSpacing: 3

                Label{
                    id: lbl_name
                    text: qsTr("System Name:")
                    transformOrigin: Item.Center
                    font.pointSize: 12
                    //anchors.right: parent

                    Layout.alignment: Qt.AlignRight
                }

                TextField{
                    id: txt_name
                    text: xdathdf.atomicSystem.systemName //qsTr("MgSiO3-Silicate")
                    enabled: true
                    readOnly: true
                    Layout.fillWidth: true
                    Layout.maximumHeight: 20
                    font.pointSize: 12
                    placeholderText: qsTr("System name")

                    onTextChanged: {
                        if(text.length > 0)
                            xdathdf.atomicSystem.systemName = text
                    }
                }

                Label{
                    id: lbl_deltaTime
                    text: qsTr("Delta Time [\u0066\u0073]:")
                    font.pointSize: 12
                    //anchors.left: parent

                    Layout.alignment: Qt.AlignRight
                }

                TextField{
                    id: txt_deltaTime
                    text: xdathdf.fileIsOpen ? xdathdf.atomicSystem.deltaTime : ""
                    enabled: true
                    readOnly: true
                    Layout.fillWidth: true
                    Layout.maximumHeight: 20
                    font.pointSize: 12
                    placeholderText: qsTr("delta time in femtosecond")
                    validator: DoubleValidator{bottom:0.0; decimals: 32}
                    inputMethodHints: Qt.ImhFormattedNumbersOnly | Qt.ImhDigitsOnly

                    onTextChanged: {
                        if(text.length > 0)
                            xdathdf.atomicSystem.deltaTime = text
                    }
                }

                Label{
                    id: lbl_temperature
                    text: qsTr("Temperature [\u212a]:")
                    font.pointSize: 12
                    //anchors.left: parent

                    Layout.alignment: Qt.AlignRight
                }

                TextField{
                    id: txt_temperature
                    text: xdathdf.fileIsOpen ? xdathdf.atomicSystem.kelvin : ""
                    enabled: true
                    readOnly: true
                    Layout.fillWidth: true
                    Layout.maximumHeight: 20
                    font.pointSize: 12
                    placeholderText: qsTr("temperature")
                    validator: DoubleValidator{bottom:0.0; decimals: 32}
                    inputMethodHints: Qt.ImhFormattedNumbersOnly | Qt.ImhDigitsOnly

                    onTextChanged: {
                        if(text.length > 0)
                            xdathdf.atomicSystem.kelvin = text
                    }
                }

                Label{
                    id: lbl_volAtoms
                    text: qsTr("Volumer per Atom:")
                    font.pointSize: 12
                    //anchors.left: parent

                    Layout.alignment: Qt.AlignRight
                }

                TextField{
                    id: txt_volAtoms
                    text: xdathdf.fileIsOpen ? xdathdf.atomicSystem.volPerAtom : ""
                    enabled: true
                    readOnly: true
                    Layout.fillWidth: true
                    Layout.maximumHeight: 20
                    font.pointSize: 12
                    placeholderText: qsTr("volumer per atom")
                    validator: DoubleValidator{bottom:0.0; decimals: 32}
                    inputMethodHints: Qt.ImhFormattedNumbersOnly | Qt.ImhDigitsOnly
                }

                Label{
                    id: lbl_numAtoms
                    text: qsTr("Number of Atoms:")
                    font.pointSize: 12
                    //anchors.left: parent

                    Layout.alignment: Qt.AlignRight
                }

                TextField{
                    id: txt_numAtoms
                    text: xdathdf.fileIsOpen ? xdathdf.atomicSystem.numOfAtoms : ""
                    enabled: true
                    readOnly: true
                    Layout.fillWidth: true
                    Layout.maximumHeight: 20
                    font.pointSize: 12
                    placeholderText: qsTr("# of Atoms")
                    validator: IntValidator{bottom:0;}
                    inputMethodHints: Qt.ImhFormattedNumbersOnly | Qt.ImhDigitsOnly

                    onTextChanged: {
                        if(text.length > 0)
                            xdathdf.atomicSystem.numOfAtoms = text
                    }
                }

                Label{
                    id: lbl_numSteps
                    text: qsTr("Number of Frames:")
                    font.pointSize: 12
                    //anchors.left: parent

                    Layout.alignment: Qt.AlignRight
                }

                TextField{
                    id: txt_numSteps
                    text: xdathdf.fileIsOpen ? xdathdf.atomicSystem.numTimeFrames : ""
                    enabled: true
                    readOnly: true
                    Layout.fillWidth: true
                    Layout.maximumHeight: 20
                    font.pointSize: 12
                    placeholderText: qsTr("# of Time Frames")
                    validator: IntValidator{bottom: 0}
                    inputMethodHints: Qt.ImhFormattedNumbersOnly | Qt.ImhDigitsOnly

                    onTextChanged: {
                        if(text.length > 0)
                            xdathdf.atomicSystem.numTimeFrames = text
                    }
                }

                Label{
                    id: lbl_lattice
                    text: qsTr("Lattice Dims [\u212b]:")
                    font.pointSize: 12
                    //anchors.left: parent

                    Layout.alignment: Qt.AlignRight
                }

                RowLayout{
                    id: row_lattice

                    TextField{
                        id: dims_01
                        font.pointSize: 12
                        placeholderText: qsTr("Dim 1")
                        validator: DoubleValidator{bottom:0.0; decimals: 32}
                        inputMethodHints: Qt.ImhFormattedNumbersOnly | Qt.ImhDigitsOnly
                        text: xdathdf.atomicSystem.latticeDim[0]
                        readOnly: true
                        enabled: true

                        Layout.maximumHeight: 20
                        Layout.fillWidth: true
                    }
                    TextField{
                        id: dims_02
                        font.pointSize: 12
                        placeholderText: qsTr("Dim 2")
                        validator: DoubleValidator{bottom:0.0; decimals: 32}
                        inputMethodHints: Qt.ImhFormattedNumbersOnly | Qt.ImhDigitsOnly
                        text: xdathdf.atomicSystem.latticeDim[1]
                        readOnly: true
                        enabled: true

                        Layout.maximumHeight: 20
                        Layout.fillWidth: true
                    }
                    TextField{
                        id: dims_03
                        font.pointSize: 12
                        placeholderText: qsTr("Dim 3")
                        validator: DoubleValidator{bottom:0.0; decimals: 32}
                        inputMethodHints: Qt.ImhFormattedNumbersOnly | Qt.ImhDigitsOnly
                        text: xdathdf.atomicSystem.latticeDim[2]
                        readOnly: true
                        enabled: true

                        Layout.maximumHeight: 20
                        Layout.fillWidth: true
                    }

                }

                //                TextField{
                //                    id: txt_lattice
                //                    text: qsTr("")
                //                    enabled: false
                //                    Layout.fillWidth: true
                //                    Layout.maximumHeight: 20
                //                    font.pointSize: 12
                //                    placeholderText: qsTr("volumer per atom")
                //                }
            }
        }

        TextArea{
            id: file_content
            anchors.top: group_box3.bottom
            //anchors.bottom: separator //parent.bottom
            anchors.topMargin: 12
            anchors.bottomMargin: 6
            anchors.left: parent.left
            anchors.right: parent.right
            frameVisible: true
            font.pointSize: 12
            text: xdathdf.positionsText
            readOnly: true
            wrapMode: TextEdit.NoWrap

            Layout.fillWidth: true
            Layout.fillHeight: true

        }

        Rectangle{
            id: separator
            color: "lightgrey"
            height: 1
            anchors.topMargin: 6
            anchors.top: file_content.bottom

            Layout.fillWidth: true
        }

        RowLayout{
            id: btm_row
            anchors.top: separator.bottom //file_content.bottom
            anchors.topMargin: 6
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.left: parent.left

            Layout.maximumHeight: 20
            Layout.fillWidth: true

            Button {
                id: btn_cancel
                text: qsTr("Cancel")

                //anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right

                onClicked:
                {
                    xdathdf.abortExport = true
                    xdathdf.statusText = ""
                }
            }

            Button {
                id: btn_convert
                text: qsTr("Convert")
                //anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: btn_cancel.left

                onClicked:
                {
                    xdathdf.abortExport = false
                    xdathdf.export2Hdf()
                    btn_convert.enabled = false
                    //xdathdf.exportPositionsData()
                }
            }

            Label{
                id: lbl_status
                text: xdathdf.statusText
                horizontalAlignment: Text.AlignLeft
                font.pointSize: 11
                color: "Salmon"

                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                //anchors.right: btn_convert.left
            }
        }
    }
}

/*
ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.quit();
            }
        }
    }

    Text {
        text: qsTr("Hello World")
        anchors.centerIn: parent
    }
}
*/
