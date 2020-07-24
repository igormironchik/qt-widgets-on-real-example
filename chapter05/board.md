# Board

To draw chess board I will use Grid QML element with Repeater with model from C\+\+. Each place
on the board is a Cell item. Cell is a square that can be white and black colors with image
of figure on it, and that can be highlighted with another colors to indicate possible moves, hits
and check state. Cell item should send clicked() signal when it's clicked by user. And in QML
is very easy to implement such cell.

```qml
import QtQuick 2.0

Item {
    id: cell

    property alias cellColor: rectangle.color
    property alias border: rectangle.border
    property alias blueProp: blue.visible
    property alias redProp: red.visible
    property alias checkProp: check.visible
    property int chessX
    property int chessY

    signal clicked( int x, int y )
    signal hovered( int x, int y )

    Rectangle {
        id: rectangle
        anchors.fill: parent
        border.width: 1
    }

    Rectangle {
        id: check
        visible: false
        anchors.fill: parent
        color: "#88FFFF00"
    }

    Rectangle {
        id: red
        visible: false
        anchors.fill: parent
        color: "#88FF0000"
    }

    Rectangle {
        id: blue
        visible: false
        anchors.fill: parent
        color: "#880000FF"
    }

    property alias source: image.source

    Image {
        width: parent.width - 5
        height: parent.height - 5
        id: image
        anchors.centerIn: parent
        fillMode: Image.PreserveAspectFit
        horizontalAlignment: Image.AlignHCenter
        verticalAlignment: Image.AlignVCenter
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        onClicked: cell.clicked( chessX, chessY )
    }
}
```

As you can see I use Rectangle element to draw the cell and unvisible by default rectangles
for highlighting. For drawing chess figure I use Image QML element, that a little smaller then cell
and centered in it. And for mouse handling I have MouseArea. It's very simple, isn't it? Imagine
how much code you would write in C\+\+ for such item.

Ok, we have the Cell item and we need to place 8x8 cells on the square board. And we want to show labels
of columns and rows.

```qml
import QtQuick 2.7

Rectangle {
    property int offset: 16
    property int cellWidth: ( width - offset * 2 ) / 8
    property int cellHeight: ( height - offset * 2 ) / 8

    id: board
    transform: rot

    property alias rotation: rot.angle

    Rotation {
        id: rot
        origin.x: board.width / 2
        origin.y: board.height / 2
        angle: 0
    }

    PropertyAnimation {
        id: anim
        duration: 300
        target: board
        from: ( rot.angle === 0 ? 0 : ( rot.angle === 360 ? 0 : 180 ) )
        to: ( rot.angle === 0 ? 180 : ( rot.angle === 360 ? 180 : 360 ) )
        property: "rotation"
    }

    Connections {
        target: game

        onRotate: {
            if( angle === -1 )
                anim.start()
            else
                rot.angle = 0
        }
    }

    Connections {
        target: anim

        onStarted: {
            rotationStarted()
        }

        onStopped: {
            rotationDone()
        }
    }

    signal clicked( int x, int y )
    signal hovered( int x, int y )
    signal newGame()
    signal transformation( int figure, int color, int x, int y );
    signal undo()
    signal rotationDone()
    signal rotationStarted()

    Row {
        id: top
        height: offset

        // Top letters.
        Item { width: offset; height: offset; }
        Repeater {
            model: [ "A", "B", "C", "D", "E", "F", "G", "H" ]
            delegate: Text {
                width: cellWidth
                height: offset
                text: modelData
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                font.pixelSize: 14
                transform: Rotation {
                    origin.x: width / 2
                    origin.y: height / 2
                    angle: rot.angle
                }
            }
        }
        Item { width: offset; height: offset; }
    }

    Column {
        y: top.y + top.height

        // Left letters.
        Repeater {
            model: 8

            delegate: Text {
                width: offset
                height: cellHeight
                text: 8 - index
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                font.pixelSize: 14
                transform: Rotation {
                    origin.x: width / 2
                    origin.y: height / 2
                    angle: rot.angle
                }
           }
        }
    }

    Grid {
        id: grid
        rows: 8
        columns: 8
        x: offset
        y: top.y + top.height
        objectName: "grid"

        // Chess board.
        Repeater {
            model: chessBoard

            delegate: Cell {
                cellColor: model.CurrentPieceColor
                border.color: model.BorderColor
                chessX: index % 8
                chessY: index / 8
                onClicked: board.clicked( x, y )
                //onHovered: board.hovered( x, y )
                objectName: "c"+ chessX + chessY
                width: cellWidth
                height: cellHeight
                source: model.CellImageSource
                blueProp: model.BluePieceColor
                redProp: model.RedPieceColor
                checkProp: model.CheckPieceColor
                transform: Rotation {
                    origin.x: width / 2
                    origin.y: height / 2
                    angle: rot.angle
                }
            }
        }
    }

    Column {
        y: grid.y
        x: grid.x + grid.width

        // Right numbers.
        Repeater {
            model: 8

            delegate: Text {
                width: offset
                height: cellHeight
                text: 8 - index
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                font.pixelSize: 14
                transform: Rotation {
                    origin.x: width / 2
                    origin.y: height / 2
                    angle: rot.angle
                }
           }
        }
    }

    Row {
        id: bottom
        height: offset
        y: grid.y + grid.height

        // Bottom letters.
        Item { width: offset; height: offset; }
        Repeater {
            model: [ "A", "B", "C", "D", "E", "F", "G", "H" ]
            delegate: Text {
                width: cellWidth
                height: offset
                text: modelData
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                font.pixelSize: 14
                transform: Rotation {
                    origin.x: width / 2
                    origin.y: height / 2
                    angle: rot.angle
                }
            }
        }
        Item { width: offset; height: offset; }
    }
}
```

The board will rotate after each move. This game is designed for two players, and it's very
usefull to rotate the board every time for each player. For this I have Rotation and PropertyAnimation,
and some stuff for synchronization of the rotation animation.

The board starts from the row of letters from A to H, and column of numbers. Cells are placed
with Grid element, Repeatre and the model on C\+\+ side. And again the column of numbers,
and the row of letters. Voila, this is a board for the chess game written with QML.

It's very easy to write such UI with animation on QML, and bindings of properties help
a lot in it. Look at angle property of transformation of labels...

```qml
transform: Rotation {
    origin.x: width / 2
    origin.y: height / 2
    angle: rot.angle
}
```

I just bind angle property to rot.angle, where rot is a id of our Rotation of the board.
This is amazing instrument in QML!

[Back](c++-qml.md) | [Contents](../README.md) | [Next](mainwindow.md)
