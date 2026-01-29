import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4

Rectangle {
    id:cmain
    color: "#313237"

    property alias docum: textArea1;
    property alias pg: pggress;
    property alias pgText: pggressText;

    Component.onCompleted: {
         if(qapi.isSimple()){
             cmenubar.file = qapi.defaultAddr();
             leftContains.visible = false;
             rightContains.anchors.left = cmain.left;
             rightContains.width = 380;
             cmenubar.height = 25;
             startBtnBySimpleMode.visible = true;
             pggress.visible = true;
             pggressText.visible = true;
             quitBtnBySimpleMode.visible = true;
             textArea1.anchors.fill = undefined;
             textArea1.anchors.top = startBtnBySimpleMode.bottom;
             textArea1.anchors.topMargin = 5;
             textArea1.height = 305;
             textArea1.visible = false;
             //startIspBtn();

             if(!qapi.hasMenu()){
                 rightContains.anchors.top = cmain.top;
                 rightContains.height = 75;
                 cmenubar.visible = false;
             }
         }
    }

    function startIspBtn(port, baud){
        port = port?port:cmenubar.port;
        baud = baud?baud:cmenubar.baud;
        qapi.checkSerialChanged(port, baud);
        if(cmenubar.file === ""){
            showMsg.tipText = "File not found!";
            showMsg.visible = true;
        }else{
            if(!cmenubar.isBinFile)
            {
                if(cmenubar.reCheckFile.checked)
                {
                    qapi.seIspFileData(cmenubar.file);
                }
                qapi.startIsp();
            }else{
                writeStartAddr.func = "startIsp";
                writeStartAddr.visible = true;
            }
        }
    }

    CMenuBar {
        id:cmenubar
        width:parent.width
        height:55
        color: "#313237"
        controlsWidth:parent.width
    }

    GroupBox {
        id:leftContains
        width:300
        height: cmain.height - cmenubar.height - 8
        anchors.top: cmenubar.bottom
        anchors.left: parent.left
        anchors.margins: 4

        CButton {
            id:startBtn
            width: 285
            height: 90
            text : "Start Isp"
            onClicked: function(){
                textArea1.text = ""
                startIspBtn();
            }
        }

        GridLayout {
            id: affordFun
            anchors.top: startBtn.bottom
            anchors.topMargin: 4
            width: leftContains.width - 16
            height: 80
            columns: 3

            CButton {
                id: get
                text: qsTr("查看命令")
                onClicked: function(){
                    qapi.getIsp();
                }
            }
            CButton {
                id: getVersionIsp
                anchors.leftMargin: 10
                text: qsTr("版本信息")
                onClicked: function(){
                    qapi.getVersionIsp();
                }
            }
            CButton {
                id: gidIsp
                anchors.leftMargin: 10
                text: qsTr("获取gid号")
                onClicked: function(){
                    qapi.gidIsp();
                }
            }
            CButton {
                id:readIsp
                anchors.leftMargin: 10
                text: qsTr("读FLASH")
                onClicked: function(){
                    readFlash.visible = true;
                    //qapi.readIsp();
                }
            }

            CButton {
                id: goIsp
                anchors.topMargin: 10;
                text: qsTr("指定地址执行")
                onClicked: function(){
                    writeStartAddr.func = "goIsp";
                    writeStartAddr.visible = true;
                }
            }
            CButton {
                id: cleanFlash
                anchors.topMargin: 10;
                anchors.leftMargin: 10
                text: qsTr("擦除FLASH")
                onClicked: function(){
                    eraseFlash.visible = true;
                }
            }
            CButton {
                id: excleanFlash
                anchors.topMargin: 10;
                anchors.leftMargin: 10
                text: qsTr("E擦除FLASH")
                onClicked: function(){
                    textArea1.append("这个功能暂时没什么用");
                }
            }

            CButton {
                id: btnWriteProetected
                text: qsTr("写保护")
                onClicked: function(){
                    writeProtected.visible = true;
                }
            }

            CButton {
                id: btnWriteUnProetected
                anchors.leftMargin: 10
                text: qsTr("去写保护")
                onClicked: function(){
                    qapi.writeUnProtected();
                }
            }
            CButton {
                id: btnReadProetected
                anchors.leftMargin: 10
                text: qsTr("读保护")
                onClicked: function(){
                    qapi.readProtected();
                }
            }
            CButton {
                id: btnReadUnProetected
                anchors.leftMargin: 10
                text: qsTr("去读保护")
                onClicked: function(){
                    qapi.readUnProtected();
                }
            }
        }
    }

    GroupBox{
        id:rightContains
        width: cmain.width - 300 - 12
        height: cmain.height - cmenubar.height - 8
        anchors.left: leftContains.right
        anchors.top: cmenubar.bottom
        anchors.margins: 4
        objectName: "groupBox"

        CButton {
            id:startBtnBySimpleMode
            visible: false
            width: 164
            height: 40
            anchors.bottomMargin: 5
            text : "YES"
            onClicked: function(){
                textArea1.text = ""
                pggressText.text = "0%";
                var ret = qapi.checkSerial(cmenubar.port)
                if(ret == 1){
                    showMsg.tipText = "Serial check failure!";
                    showMsg.visible = true;
                    return;
                }else if(ret == 2){
                    showMsg.tipText = "Serial is occupied!";
                    showMsg.visible = true;
                    return;
                }

                if(!qapi.hasMenu()){
                    if(qapi.checkSerialChanged(qapi.getPort(), 115200)){
                        qapi.startIsp();
                    }
                }else{
                    startIspBtn();
                }
            }
        }

        CButton {
            id:quitBtnBySimpleMode
            anchors.left: startBtnBySimpleMode.right
            anchors.leftMargin: 35
            visible: false
            width: 164
            height: 40
            anchors.bottomMargin: 5
            text : "NO"
            onClicked: function(){
                Qt.quit();
            }
        }

        ProgressBar {
            id:pggress
            visible: false
            anchors.top: startBtnBySimpleMode.bottom
            anchors.margins: 4
            width: 335
            minimumValue: 0
            maximumValue: 100
            value:0
            style: ProgressBarStyle {
              background: Rectangle {
                  radius: 2
                  color: "#D2E1F0"
                  border.color: "gray"
                  border.width: 0
                  implicitWidth: 335
                  implicitHeight: 20
              }
              progress: Rectangle {
                  color: "#00A0E6"
              }
          }
        }

        Text {
            id: pggressText
            visible: false
            anchors.top: startBtnBySimpleMode.bottom
            anchors.left: pggress.right
            anchors.topMargin: 3
            width: 30
            color: "#00C9CD"
            font.family: "Microsoft YaHei"
            font.pointSize: 12
            text: "0%"
        }

        TextArea {
            id: textArea1
            readOnly: true
            anchors.fill: parent
            text : ""
            style: TextAreaStyle {
              textColor: "#00C9CD"
              backgroundColor: "#141517"
            }
        }
    }

    CDialog{
        id: showMsg
        cwidth: 300
        cheight: 100
        anchors.fill: parent
        visible: false
    }

    CPopupInput {
        id: writeStartAddr
        cwidth: 300
        cheight: 100
        anchors.fill: parent
        visible: false
    }

    CPopupReadMemorySetting {
        id: readFlash
        cwidth: 300
        cheight: 130
        anchors.fill: parent
        visible: false
    }
    CPopupEraseSetting {
        id: eraseFlash
        cwidth: 300
        cheight: 260
        anchors.fill: parent
        visible: false
    }
    CPopupWriteProtected{
        id: writeProtected
        cwidth: 300
        cheight: 240
        anchors.fill: parent
        visible: false
    }
}
