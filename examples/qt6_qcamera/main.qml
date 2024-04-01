
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick 2.7
import QtQuick.Window 2.2
import Frames 0.1
import QtMultimedia

ApplicationWindow {
    id: appWindow
    visible: true
    width: 600
    height: 600

    Frames {
        id: frames
        videoSink: view.videoSink
    }

    Column {
        anchors.centerIn:  parent
        width: parent.width

        Text {
            text: frames.formatString
            anchors.horizontalCenter: parent.horizontalCenter
            font.bold: true
            font.pointSize: 15
        }

        VideoOutput {
            id: view
            focus : visible
            width: parent.width
            transform: [ Scale { origin.x: appWindow.width / 2; origin.y: appWindow.height / 2;
                    xScale: frames.xScale; yScale: frames.yScale },
                Rotation { origin.x: appWindow.width / 2; origin.y: appWindow.height / 2;
                    angle: frames.angle } ]
        }

        Text {
            text: frames.fpsString
            anchors.horizontalCenter: parent.horizontalCenter
            font.pointSize: 24
            font.bold: true
        }
    }
}
