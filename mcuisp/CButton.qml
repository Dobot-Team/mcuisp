import QtQuick 2.3
import QtGraphicalEffects 1.0

Rectangle
{
    id:cbtn

    property string picCurrent: "qrc:/mcuisp/images/btn_noraml.png";
    property string picNormal: "qrc:/mcuisp/images/btn_normal.png";
    property string picHover: "qrc:/mcuisp/images/btn_hover.png";
    property string picPressed: "qrc:/mcuisp/images/btn_pressed.png";
    property string text;

    signal clicked

    width: 79
    height: 25
    color:"#00000000"
    state:"normal"

    Image
    {
        anchors.fill: parent
        source: picNormal;
    }

    MouseArea
    {
        hoverEnabled: true
        anchors.fill: parent
        onEntered: cbtn.state == "pressed" ? cbtn.state = "pressed" : cbtn.state = "hover"
        onExited: cbtn.state == "pressed" ? cbtn.state = "pressed" : cbtn.state = "normal"
        onPressed: cbtn.state = "pressed"
        onReleased:
        {
            cbtn.state = "normal"
            cbtn.clicked()
        }
    }

    Text {
        id: tipPath
        anchors.top: cbtn.top
        anchors.left: cbtn.left
        text : cbtn.text
        color: "#ffffff"
        anchors.centerIn: parent
        anchors.topMargin: 8
        font.pixelSize: 14
    }

    states:
    [
        State{
            name:"hover"
            PropertyChanges {
                target: cbtn
                picCurrent:picHover
            }
        },
        State {
            name: "normal"
            PropertyChanges {
                target: cbtn
                picCurrent:picNormal
            }
        },
        State {
            name: "pressed"
            PropertyChanges {
                target: cbtn
                picCurrent:picPressed
            }
        }

    ]
}
