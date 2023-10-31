#ifndef DATAMODEL_H
#define DATAMODEL_H

#include "dataitem.h"

#include <QAbstractListModel>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>

class DataModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged FINAL)

public:
    explicit DataModel(QObject *parent = nullptr);

    enum Role {
        DataDescriptionRole = Qt::UserRole + 1,
        DataNameRole,
        DataUnitRole,
        DataValueRole
    };

    // QAbstractItemModel interface
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual QHash<int, QByteArray> roleNames() const override;

    QString host() const;
    void setHost(const QString &newHost);
    static quint8 lowByte(quint16 val);
    static quint8 hiByte(quint16 val);
    static quint16 twoBytesToWord(quint8 hi, quint8 lo);
    static float forBytesToFloat(QByteArray* arr, int pos);

public slots:
    void fetchData(QString url);
    void parseData();
    void fetchModbus();
    void sendToModbus();
    void parseModbus();

signals:
    void hostChanged();

private:
    QList<DataItem*> m_dataList;
    QNetworkAccessManager m_networkManager;
    QNetworkReply *m_reply = nullptr;
    QString m_host;
    QTcpSocket *m_socket;
    quint16 transactionId = 0;
    quint16 m_modbusAddress;
    quint16 m_modbusLength;
    QTimer m_timeoutTimer;
};

#endif // DATAMODEL_H
