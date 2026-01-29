/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.4
import QtQml.Models 2.1
import QtGraphicalEffects 1.0
import "./content"
import "./mcuisp"

Rectangle {//最底层透明图层
    id:mmmm;
    width: 800;
    height: 500;
    color: "transparent"

    property int listViewActive: 0

    Component.onCompleted: {
         if(qapi.isSimple()){
             maxMinBtn.visible = false;
             mmmm.width = 405;
             mmmm.height = 170;
             stocText.text = qapi.getName()
             qtText.text = "";
             stocText.color = "#5caa15";

             if(!qapi.hasMenu()){
                mmmm.height = 130;
             }
         }
    }

    MouseArea {
        id: dragRegion
        anchors.fill: parent
        property point clickPos: "0,0"
        onPressed: {
            clickPos  = Qt.point(mouse.x,mouse.y)
        }
        onPositionChanged: {
            //鼠标偏移量
            var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y)
            //如果mainwindow继承自QWidget,用setPos
            window.setX(window.x+delta.x)
            window.setY(window.y+delta.y)
        }
    }

    Item {//主体窗口
        id: container;
        anchors.centerIn: parent;
        width:  parent.width;
        height: parent.height;
        Rectangle {
            id: mainRect
            width: container.width-(2*rectShadow.radius);
            height: container.height - (2*rectShadow.radius);
            anchors.centerIn: parent;

            property int listViewActive: 0

            Rectangle {
                id: banner
                height: 28
                anchors.top: parent.top
                width: parent.width
                anchors.left: parent.left
                gradient: Gradient
                {
                    GradientStop { position: 0.0; color: '#4D4D4D' }
                    GradientStop { position: 1.0; color: "#292929" }
                }

                Item {
                    id: textItem
                    width: stocText.width + qtText.width
                    height: stocText.height + qtText.height
                    anchors.horizontalCenter: banner.horizontalCenter
                    anchors.verticalCenter: banner.verticalCenter

                    Text {
                        id: stocText
                        anchors.verticalCenter: textItem.verticalCenter
                        color: "#ffffff"
                        font.family: "Microsoft YaHei"
                        font.pointSize: 18
                        text: "Mcuisp"
                    }
                    Text {
                        id: qtText
                        anchors.verticalCenter: textItem.verticalCenter
                        anchors.left: stocText.right
                        color: "#5caa15"
                        font.family: "Microsoft YaHei"
                        font.pointSize: 18
                        text: "-Dobot"
                    }
                }

                Item {//最小化按钮、关闭按钮
                    id: maxMinBtn
                    anchors.top: banner.top
                    anchors.topMargin: 3
                    anchors.right: banner.right
                    anchors.rightMargin: 60
                    SysBtnGroup
                    {
                        id: sysbtngrp

                        onMin: window.showMinimized()
                        onClose: window.close()
                    }
                }
            }

            Cmain {
                id: root
                width: parent.width
                anchors.left:parent.left
                anchors.top: banner.bottom
                anchors.bottom: parent.bottom
            }
        }
    }

    DropShadow {//绘制阴影
        id: rectShadow;
        anchors.fill: source
        cached: true;
        horizontalOffset: 0;
        verticalOffset: 3;
        radius: 8.0;
        samples: 16;
        color: "#80000000";
        smooth: true;
        source: container;
    }

    function notifyProgress(grog){
        root.pg.value = grog;
        root.pgText.text = grog.toString() + "%";
    }

    function notifyText(str, isShowInAnyWay){
        if(qapi.isSimple() && !isShowInAnyWay){
            return;
        }

        root.docum.append(str);
    }
    function notifyData(str){
        var arr = str.toString().toUpperCase().split(""), i;
        var op = "接收数据： ";
        for(i = 2; i <　arr.length - 2; i+= 2)
        {
            op += "0x" + arr[i] + arr[i+1] + " ";
        }
        root.docum.append(op);
    }
}
