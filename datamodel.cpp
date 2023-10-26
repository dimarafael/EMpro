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
    QByteArray data;
    data.resize(12);
    transactionId++;

    data[0] = (transactionId & 0b11111111<<8)>>8; // Transaction Identifier High Byte
    data[1] = transactionId & 0b11111111; // Transaction Identifier Low Byte
    data[2] = 0; // Protocol Identifier
    data[3] = 0; // Protocol Identifier
    data[4] = 0; // Length
    data[5] = 6; // Length
    data[6] = 1; // Unit Identifier
    data[7] = 3; // Function Code
    data[8] = 0; // Data Address
    data[9] = 0; // Data Address
    data[10] = 0; // Number of registers
    data[11] = 10;// Number of registers
    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::readyRead, this, &DataModel::parseModbus);

    m_socket->connectToHost(host(), 502);
    if(m_socket->waitForConnected()){
        m_socket->write(data);
    }
}

void DataModel::parseModbus()
{
    QByteArray data = m_socket->readAll();
    m_socket->disconnect();
    qDebug() << data;
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
