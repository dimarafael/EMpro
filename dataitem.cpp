#include "dataitem.h"

DataItem::DataItem()
{

}

QString DataItem::description() const
{
    return mDescription;
}

void DataItem::setDescription(const QString &newDescription)
{
    mDescription = newDescription;
}

QString DataItem::name() const
{
    return mName;
}

void DataItem::setName(const QString &newName)
{
    mName = newName;
}

QString DataItem::unit() const
{
    return mUnit;
}

void DataItem::setUnit(const QString &newUnit)
{
    mUnit = newUnit;
}

float DataItem::value() const
{
    return mValue;
}

void DataItem::setValue(float newValue)
{
    mValue = newValue;
}
