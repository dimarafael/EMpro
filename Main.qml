import QtQuick
import QtQuick.Window
import com.cmp.DataModel
import com.cmp.ModbusModel

Window {
    id: window
    width: 480
    height: 640
    visible: true
    title: qsTr("EMpro")
    color: "gray"
    Row{
        id: topRow
        height: 50
        width: parent.width
        anchors{
            top: parent.top
            margins: 20
        }

        Rectangle{
            id: hostTextInputRectangle
            height: parent.height/2
            width: parent.width - parent.height*4

            anchors{
                left: parent.left
                verticalCenter: parent.verticalCenter
                margins: 20
            }
            color: "white"
        }

        TextInput{
            id: hostTextInput
            height: parent.height/2
            width: parent.width - parent.height*4

            anchors{
                left: parent.left
                verticalCenter: parent.verticalCenter
                margins: 25
            }
            color: "#1e1e1e"
            verticalAlignment: TextInput.AlignVCenter
            font.pixelSize: 14
            onTextEdited: {
                DataModel.host = text
            }
            text: DataModel.host
        }

        Rectangle{
            id: updateButtonMeters
            height: parent.height
            width: parent.height
            anchors{
                right: parent.right
                verticalCenter: parent.verticalCenter
                margins: 20
            }
            color: "#1e1e1e"
            Text{
                anchors.fill: parent
                text: "METERS"
                color: "white"
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    DataModel.fetchData("meters")
                    ModbusModel.fetchData()
                }
            }
        }

        Rectangle{
            id: updateButtonMeasur
            height: parent.height
            width: parent.height
            anchors{
                right: updateButtonMeters.left
                verticalCenter: parent.verticalCenter
                margins: 20
            }
            color: "#1e1e1e"
            Text{
                anchors.fill: parent
                text: "MEASUR"
                color: "white"
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    DataModel.fetchData("measurements")
                }
            }
        }

    }

    ListView{
        id: listViev
        anchors{
            top: topRow.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        anchors.margins: 20

        model: DataModel
        spacing: 10
        clip: true

        delegate: Rectangle{
            id: delegate

            required property string dataDescription
            required property string dataName
            required property string dataUnit
            required property real dataValue

            width: listViev.width
            height: 50
            color: "#1e1e1e"

            Column{
                id: column
                anchors.fill: parent
                anchors.margins: 5
                spacing: 5

                Text {
                    id: textDescription
                    width: column.width
                    color: "white"
                    font {
                      pixelSize: 14
                      bold: true
                    }
                    text: delegate.dataDescription
                }
                Row{
                    id: row

                    Text{
                        id: textName
                        color: "white"
                        text: delegate.dataName + " = "
                        font.pixelSize: 14
                    }
                    Text{
                        id: textValue
                        color: "white"
                        text: Math.round((delegate.dataValue + Number.EPSILON)*100)/100
                        font.pixelSize: 14
                        font.bold: true
                    }
                    Text{
                        id: textUnit
                        color: "white"
                        text: " " + delegate.dataUnit
                        font.pixelSize: 14
                    }
                }
            }

        }
    }

}
