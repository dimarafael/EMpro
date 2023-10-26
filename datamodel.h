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
    QNetworkReply *m_reply;
    QString m_host;
    QTcpSocket *m_socket;
    quint16 transactionId;
    QTimer m_timeoutTimer;
};

#endif // DATAMODEL_H
