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
