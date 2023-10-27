#ifndef DATAITEM_H
#define DATAITEM_H

#include <QString>

class DataItem
{
public:
    DataItem();

    QString description() const;
    void setDescription(const QString &newDescription);
    QString name() const;
    void setName(const QString &newName);
    QString unit() const;
    void setUnit(const QString &newUnit);
    float value() const;
    void setValue(float newValue);

private:
    QString mDescription;
    QString mName;
    QString mUnit;
    float mValue;
};

#endif // DATAITEM_H
