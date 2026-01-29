import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1

Item {
    id: dialogComponent
    anchors.fill: parent

    property int cwidth: 300
    property int cheight: 100
    property alias label1: label1
    property string func : "none"

    property var funArr : {
        "none" : function(){},
        "goIsp":function(){qapi.goIsp();},
        "startIsp":function(){ qapi.startIsp();}
    };

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
                text: "请输入烧写的起始地址"
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
                    text: qsTr("起始地址0x:")
                    Layout.rowSpan: 0
                    font.pointSize: 14
                }

                TextField {
                    id: textInput1
                    width: 100
                    height: 23
                    text: "08000000"
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

                Button {
                    text:"确定"
                    width: 60
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    onClicked: function(){
                        var str = textInput1.text;
                        var reg = /[A-FAa-f0-9]{1,8}/;
                        if(reg.test(str)){
                            qapi.setStartAddr(parseInt(str, 16));
                            if(func !== "")
                                funArr[func]();

                            dialogComponent.visible = false;
                        }else{
                            textTip.text = "请输入8位16进制地址"
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
