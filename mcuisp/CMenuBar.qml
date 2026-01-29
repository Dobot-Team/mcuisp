import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.4

Rectangle {
    id:root

    property int controlsHeight:75;
    property int controlsWidth:150;
    property bool isBinFile : false;
    property string file : "";

    property alias port: areaCom.currentText
    property alias baud: areaBps.currentText
    property alias reCheckFile: isReloadFile

    ListModel{
        id:areaBpsModel
    }

    Component.onCompleted: {
        areaBpsModel.append([{"value": "560800", "text":"560800"},
                             {"value": "256000", "text":"256000"},
                             {"value": "230400", "text":"230400"},
                             {"value": "128000", "text":"128000"},
                             {"value": "115200", "text":"115200"},
                             {"value": "76800", "text":"76800"},
                             {"value": "57600", "text":"57600"},
                             {"value": "38400", "text":"38400"},
                             {"value": "19200", "text":"19200"},
                             {"value": "14400", "text":"14400"},
                             {"value": "9600", "text":"9600"},
                             {"value": "4800", "text":"4800"},
                             {"value": "1200", "text":"1200"},
                             {"value": "600", "text":"600"}]);
        if(qapi.isSimple()){
            layoutBottom.visible = false;
            height = 40;
        }
    }

    Component {
       id: combostyle
       ComboBoxStyle{
           id:style
           background:Rectangle {
               id:background
               radius: 12
               color: "#141517"
               Image {
                   id: imageDropdown
                   width: height
                   height: 0.8 * parent.height
                   anchors.right: parent.right
                   anchors.rightMargin: 2
                   anchors.verticalCenter: parent.verticalCenter
                   fillMode: Image.Stretch
                   source: "qrc:/mcuisp/images/selectDobot_pressed.png"
               }
           }

           label: Text {
               id: text1
               height: control.height
               color: "#ffffff"
               text:control.currentText
               verticalAlignment: Text.AlignVCenter
               horizontalAlignment: Text.left
               font.pixelSize: 14
           }
       }
   }

    Row{
        id:layoutTop
        anchors.top: parent.top
        anchors.topMargin: 2
        width:controlsWidth
        spacing: 8

        Text{
            id:comText
            text:"  Serial："
            color: "#ffffff"
            anchors.top: parent.top
            anchors.topMargin: 5
        }

        ComboBox{
            id:areaCom
            width: 70
            height: 25
            model:comModel
            onPressedChanged: function(){
                if(qapi.updateAvailabeCom(0) < 1)
                    currentIndex = -1;
            }
            onCurrentIndexChanged: function(){
                qapi.checkSerialChanged(currentText, areaBps.currentText);
            }
            style: combostyle
        }

        Text{
            id:comBps
            color: "#ffffff"
            text:"Baudrate：";
            anchors.top: parent.top
            anchors.topMargin: 5
        }

        ComboBox{
            id:areaBps
            width: 80
            height: 25
            model:areaBpsModel
            currentIndex: 4
            onCurrentIndexChanged: function(){
                qapi.checkSerialChanged(areaCom.currentText, currentText);
            }
            style: combostyle
        }

        CButton{
            id:openSerail
            text: "check"
            onClicked: function(){
                qapi.checkSerialChanged(areaCom.currentText, areaBps.currentText);
                var ret = qapi.checkSerial(areaCom.currentText);
                if(ret == 1){
                    showMsg.tipText = "Serial check failure!";
                }else if(ret == 2){
                    showMsg.tipText = "Serial is occupied!";
                }else{
                    showMsg.tipText = "Serial check success!";
                }
                showMsg.visible = true;
            }
        }
    }

    Item{
        id: layoutBottom
        anchors.left: root.left
        anchors.top: layoutTop.bottom

        FileDialog {
            id: fileDialog
            nameFilters: ["文件格式 (*.hex *.bin)", "All files (*)"]
            title: "请选择一个文件"
            folder: shortcuts.home
            onAccepted: {
                var fend, url, prefix = "file:///";
                url = fileDialog.fileUrl.toString();

                url = url.substring(prefix.length);
                filePath.text = url;

                fend = url.substring(url.length - 3);
                isBinFile = (fend === "bin");
                file = url;
                qapi.seIspFileData(url);
            }
            onRejected: {

            }
            Component.onCompleted: visible = false
        }

        Text {
            id: tipPath
            color: "#ffffff"
            anchors.top: layoutBottom.top
            anchors.topMargin: 8
            text: qsTr("  Firware Path:")
            font.pixelSize: 14
        }

        TextField {
            id: filePath
            width: 380
            anchors.left: tipPath.right
            anchors.leftMargin: 4
            anchors.top: layoutBottom.top
            anchors.topMargin: 3
            text: qsTr("")
            readOnly: true
            font.pixelSize: 12
            style: TextFieldStyle {
              textColor: "#00C9CD"
              background: Rectangle {
                  radius: 9
                  border.color: "#4D4D4D"
                  border.width: 1
                  color: "#141517"
              }
            }
        }

        CButton {
            id: openFile
            anchors.top: layoutBottom.top
            anchors.topMargin: 3
            anchors.left:filePath.right
            anchors.leftMargin: 4
            text: qsTr("Open File")
            onClicked: function(){
                fileDialog.visible = true;
            }
        }

        CheckBox {
            id: isReloadFile
            anchors.top: layoutBottom.top
            anchors.topMargin: 10
            anchors.left:openFile.right
            anchors.leftMargin: 15
            text: qsTr("")
        }
        Text {
            color: "#ffffff"
            anchors.top: layoutBottom.top
            anchors.left: isReloadFile.right
            anchors.topMargin: 8
            text: qsTr("Override file before isp")
            font.pixelSize: 14
        }
    }
}
