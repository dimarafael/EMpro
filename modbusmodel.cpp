#include "modbusmodel.h"

ModbusModel::ModbusModel(QObject *parent)
    : QAbstractListModel{parent}
{
    transactionId = 0;
}

int ModbusModel::rowCount(const QModelIndex &parent) const
{
    return m_dataList.size();
}

QVariant ModbusModel::data(const QModelIndex &index, int role) const
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

QHash<int, QByteArray> ModbusModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[DataDescriptionRole] = "dataDescription";
    names[DataNameRole] = "dataName";
    names[DataUnitRole] = "dataUnit";
    names[DataValueRole] = "dataValue";

    return names;
}

void ModbusModel::fetchData()
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
    connect(m_socket, &QTcpSocket::readyRead, this, &ModbusModel::parseData);

    m_socket->connectToHost("10.0.5.41", 502);
    if(m_socket->waitForConnected()){
        m_socket->write(data);
    }
}

void ModbusModel::parseData()
{
    QByteArray data = m_socket->readAll();
    m_socket->disconnect();
    qDebug() << data;
}
