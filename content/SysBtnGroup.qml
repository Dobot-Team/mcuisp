import QtQuick 2.3

Row
{
    id: sysbtngroup
    spacing: 0

    signal min
    signal close

    SysBtn
    {
        id:min
        picHover: "qrc:/content/images/min_hover.png"
        picNormal: "qrc:/content/images/min_normal.png"
        picPressed: "qrc:/content/images/min_pressed.png"
        onClicked:
        {
            console.log("min btn clicked")
            sysbtngroup.min()
        }
    }

    SysBtn
    {
        id:close
        picHover: "qrc:/content/images/close_hover.png"
        picNormal: "qrc:/content/images/close_normal.png"
        picPressed: "qrc:/content/images/close_pressed.png"
        onClicked:
        {
            console.log("close btn clicked")
            sysbtngroup.close()
        }
    }
}
