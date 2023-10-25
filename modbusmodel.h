#ifndef MODBUSMODEL_H
#define MODBUSMODEL_H

#include <QAbstractListModel>
#include <QTcpSocket>
#include "dataitem.h"

class ModbusModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ModbusModel(QObject *parent = nullptr);

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

public slots:
    void fetchData();
    void parseData();

private:
    QList<DataItem*> m_dataList;
    QTcpSocket *m_socket;
    quint16 transactionId;
};

#endif // MODBUSMODEL_H
