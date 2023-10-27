#include "datamodel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>


DataModel::DataModel(QObject *parent)
    : QAbstractListModel{parent}
{
    setHost("10.0.5.41");
}

int DataModel::rowCount(const QModelIndex &parent) const
{
    return m_dataList.size();
}

QVariant DataModel::data(const QModelIndex &index, int role) const
{
    if(index.isValid() && index.row() >= 0 && index.row() < m_dataList.size()){
        DataItem *dataItem = m_dataList[index.row()];

        switch ((Role)role) {
        case DataDescriptionRole:
            return dataItem->description();
        case DataNameRole:
            return dataItem->name();
        case DataUnitRole:
            return dataItem->unit();
        case DataValueRole:
            return dataItem->value();
        }
    }
    return {};
}

QHash<int, QByteArray> DataModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[DataDescriptionRole] = "dataDescription";
    names[DataNameRole] = "dataName";
    names[DataUnitRole] = "dataUnit";
    names[DataValueRole] = "dataValue";

    return names;
}

void DataModel::fetchData(QString url)
{
    if(m_reply){
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = nullptr;
    }

    m_reply = m_networkManager.get(QNetworkRequest(QUrl("http://" + host() + "/api/v1/" + url)));
    connect(m_reply, &QNetworkReply::finished, this, &DataModel::parseData);
}

void DataModel::parseData()
{
    if(m_reply->error() == QNetworkReply::NoError){
        beginResetModel();

        qDeleteAll(m_dataList);
        m_dataList.clear();

        QByteArray data = m_reply->readAll();
        QJsonDocument jsonDocument = QJsonDocument::fromJson(data);
        QJsonArray items = jsonDocument["items"].toArray();

        for(const auto &item : items){
            QJsonObject objItem = item.toObject();
            DataItem *dataItem = new DataItem();
            dataItem->setDescription(objItem["description"].toString());
            dataItem->setName(objItem["name"].toString());
            dataItem->setUnit(objItem["unit"].toString());
            dataItem->setValue(objItem["value"].toDouble());
            m_dataList.append(dataItem);
        }

        endResetModel();
    } else if (m_reply->error() != QNetworkReply::OperationCanceledError){
        qCritical() << "Fetch data reply error: " << m_reply->errorString();
    }

    m_reply->deleteLater();
    m_reply = nullptr;
    
}

void DataModel::fetchModbus()
{

    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::readyRead, this, &DataModel::parseModbus);
    connect(m_socket, &QTcpSocket::connected, this, &DataModel::sendToModbus);

    connect(m_socket, &QTcpSocket::stateChanged,this,
            [&](QAbstractSocket::SocketState st){
                qDebug()<<"state: " << st;
                if(st == QAbstractSocket::UnconnectedState){
                    m_socket->deleteLater();
                    m_socket = nullptr;
                    qDebug() << "delete socket";
                }
            });

    m_socket->connectToHost(host(), 502);
    m_timeoutTimer.singleShot(1000, this, [&](){
        if(m_socket != nullptr){
            m_socket->deleteLater();
            qDebug() << "Modbus timeout";
        }
    });
}

void DataModel::sendToModbus()
{
    m_timeoutTimer.stop();
    QByteArray data;
    data.resize(12);

    (transactionId >= 65535)?transactionId=1:transactionId++;
    m_modbusAddress = 32790;
    m_modbusLength = 6;

    data[0] = DataModel::hiByte(transactionId);//(transactionId & 0b11111111<<8)>>8; // Transaction Identifier High Byte
    data[1] = DataModel::lowByte(transactionId);//transactionId & 0b11111111; // Transaction Identifier Low Byte
    data[2] = 0; // Protocol Identifier
    data[3] = 0; // Protocol Identifier
    data[4] = 0; // Length
    data[5] = 6; // Length
    data[6] = 1; // Unit Identifier
    data[7] = 3; // Function Code
    data[8] = DataModel::hiByte(m_modbusAddress); // Data Address
    data[9] = DataModel::lowByte(m_modbusAddress); // Data Address
    data[10] = DataModel::hiByte(m_modbusLength);; // Number of registers
    data[11] = DataModel::lowByte(m_modbusLength);;// Number of registers
    m_socket->write(data);
}

void DataModel::parseModbus()
{
    QByteArray data = m_socket->readAll();
    m_socket->close();
    qDebug() << data;
    if((data.size() != m_modbusLength*2+9) || (data[8] != m_modbusLength*2)){
        qDebug() << "Modbus response length wrong";
        return;
    }
    if(transactionId != DataModel::twoBytesToWord(data[0], data[1])){
        qDebug() << "Modbus transaction Id wrong";
        return;
    }
//    qDebug() << DataModel::forBytesToFloat(&data, 9);
//    qDebug() << DataModel::forBytesToFloat(&data, 13);
//    qDebug() << DataModel::forBytesToFloat(&data, 17);

    beginResetModel();
    qDeleteAll(m_dataList);
    m_dataList.clear();

    DataItem *dataItem = new DataItem();
    dataItem->setDescription("Total active power");
    dataItem->setName("P");
    dataItem->setUnit("kW");
    dataItem->setValue(DataModel::forBytesToFloat(&data, 9)/1000);
    m_dataList.append(dataItem);

    dataItem = new DataItem();
    dataItem->setDescription("Total reactive power");
    dataItem->setName("R");
    dataItem->setUnit("kvar");
    dataItem->setValue(DataModel::forBytesToFloat(&data, 13)/1000);
    m_dataList.append(dataItem);

    dataItem = new DataItem();
    dataItem->setDescription("Total apparent power");
    dataItem->setName("A");
    dataItem->setUnit("kVA");
    dataItem->setValue(DataModel::forBytesToFloat(&data, 17)/1000);
    m_dataList.append(dataItem);

    endResetModel();
}

QString DataModel::host() const
{
    return m_host;
}

void DataModel::setHost(const QString &newHost)
{
    if (m_host == newHost)
        return;
    m_host = newHost;
    emit hostChanged();
}

quint8 DataModel::lowByte(quint16 val)
{
    return val & 0b11111111;
}

quint8 DataModel::hiByte(quint16 val)
{
    return (val & 0b11111111 << 8) >> 8;
}

quint16 DataModel::twoBytesToWord(quint8 hi, quint8 lo)
{
    return hi << 8 | lo;
}

float DataModel::forBytesToFloat(QByteArray* arr, int pos)
{
    if(arr->size() < pos+4){
        qDebug() << "Wrong input data for convertation to float";
        return 0;
    }

    char bb[4];
    bb[0] = arr->data()[pos+1];
    bb[1] = arr->data()[pos];
    bb[2] = arr->data()[pos+3];
    bb[3] = arr->data()[pos+2];

    float fRes;
    std::memcpy(&fRes, &bb, 4);
    return fRes;
}
