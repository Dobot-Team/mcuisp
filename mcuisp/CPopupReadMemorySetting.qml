import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1

Item {
    id: dialogComponent
    anchors.fill: parent

    property int cwidth: 300
    property int cheight: 120
    property alias label1: label1

    // Add a simple animation to fade in the popup
    // let the opacity go from 0 to 1 in 400ms
    PropertyAnimation {
        target: dialogComponent;
        property: "opacity";
        duration: 400;
        from: 0;
        to: 1;
        easing.type: Easing.InOutQuad ;
        running: true
    }

    // This rectange is the a overlay to partially show the parent through it
    // and clicking outside of the 'dialog' popup will do 'nothing'
    Rectangle {
        anchors.fill: parent
        id: overlay
        color: "#000000"
        opacity: 0.6
        // add a mouse area so that clicks outside
        // the dialog window will not do anything
        MouseArea {
            anchors.fill: parent
        }
    }

    // This rectangle is the actual popup
    Rectangle {
        id: dialogWindow
        width: cwidth
        height: cheight
        radius: 10
        anchors.centerIn: parent

        ColumnLayout {
            id:colume
            width: parent.width
            height: parent.height
            spacing: 10
            anchors.fill: parent

            Text {
                id:textTip
                text: "请输入读取的地址和长度"
                font.bold: true
                font.pointSize: 12
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                verticalAlignment: Text.AlignTop
                horizontalAlignment: Text.AlignHCenter
            }

            RowLayout {
                id: rowLayout1
                width: 100
                height: 100
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.fillWidth: true

                Label {
                    id: label1
                    anchors.top: parent.top
                    anchors.topMargin: 1
                    text: "起始地址0x08000000："
                    Layout.rowSpan: 0
                    font.pointSize: 12
                }

                TextField {
                    id: textInput1
                    width: 100
                    height: 23
                    text: ""
                    Layout.fillWidth: false
                    font.pixelSize: 14
                }
            }
            RowLayout {
                id: rowLayout2
                width: 100
                height: 100
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.fillWidth: true

                Label {
                    id: label2
                    anchors.top: parent.top
                    anchors.topMargin: 1
                    text: qsTr("读取数据长度0x00-0xFF：")
                    Layout.rowSpan: 0
                    font.pointSize: 12
                }

                TextField {
                    id: textInput2
                    height: 23
                    text: ""
                    Layout.maximumHeight: 50
                    Layout.maximumWidth: 80
                    Layout.rowSpan: 1
                    Layout.fillWidth: false
                    font.pixelSize: 14
                }
            }

            RowLayout {
                id: rowLayout3
                width: 100
                height: 100
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.fillWidth: true

                Button {
                    text:"确定"
                    width: 60
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    onClicked: function(){
                        var addr = textInput1.text;
                        var len = textInput2.text;
                        var reg = /[A-FAa-f0-9]{1,8}/;
                        var reg1 = /[A-FAa-f0-9]{1,2}/;
                        var iAddr = parseInt(addr, 16), iLen = parseInt(len, 16);
                        if(reg.test(addr) && reg1.test(len) && iLen > 0 && iLen <= 0xFF){
                            qapi.setReadMSetting(iAddr, iLen);
                            qapi.readIsp();
                            dialogComponent.visible = false;
                        }else{
                            textTip.text = "请输入合法的数据"
                            textTip.color = "#FF0000";
                        }
                    }
                }
                Button{
                    text:"取消"
                    width: 60
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    onClicked: function(){
                        dialogComponent.visible = false;
                    }
                }
            }
        }
    }
}
