import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1

Item {
    id: dialogComponent
    anchors.fill: parent

    property int cwidth: 300
    property int cheight: 260

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
                text: "请输入擦除参数"
                font.bold: true
                font.pointSize: 12
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                verticalAlignment: Text.AlignTop
                horizontalAlignment: Text.AlignHCenter
            }

            //定义的互斥分组
            ExclusiveGroup{
                id: mos;
            }

            RowLayout {
                id: rowLayout1
                width: cwidth
                height: 100
                spacing: 90
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                Layout.fillWidth: true

                RadioButton{
                    id: isGlobal;
                    text: "使用全局擦除";
                    exclusiveGroup: mos;
                    activeFocusOnPress: true;
                    onClicked: function(){
                        textAddr.visible = false;
                        textTip1.visible = false;
                        cheight = 100;
                    }
                }

                RadioButton{
                    id: isNonGlobal;
                    text:"按选项擦除";
                    exclusiveGroup: mos;
                    activeFocusOnPress: true;
                    checked: true
                    onClicked: function(){
                        textAddr.visible = true;
                        textTip1.visible = true;
                        cheight = 260;
                    }
                }
            }
            Text{
                id:textTip1
                height: 26
                anchors.left: colume.left
                anchors.leftMargin: 10
                text: "请输入擦除页地址，以逗号隔开，比如0x67,0xA6:"
            }

            TextArea {
                id: textAddr
                anchors.top: textTip1.bottom
                anchors.left: colume.left
                anchors.topMargin: 10
                anchors.leftMargin: 30;
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
                        if(mos.current){
                            var isGlobalErase = mos.current.text === "使用全局擦除";
                            var str = textAddr.text;
                            var ret;
                            if((ret = qapi.eraseIspCheckParam(isGlobalErase, str)) !== ""){
                                textTip.text = "请输入合法的地址数据err[" + ret + "]";
                                textTip.color = "#FF0000";
                                return;
                            }
                            dialogComponent.visible = false;
                            qapi.eraseIsp(isGlobalErase, str);
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
