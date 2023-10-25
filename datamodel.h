#ifndef DATAMODEL_H
#define DATAMODEL_H

#include "dataitem.h"

#include <QAbstractListModel>
#include <QNetworkAccessManager>
#include <QNetworkReply>

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

signals:
    void hostChanged();

private:
    QList<DataItem*> m_dataList;
    QNetworkAccessManager m_networkManager;
    QNetworkReply *m_reply;
    QString m_host;
};

#endif // DATAMODEL_H
