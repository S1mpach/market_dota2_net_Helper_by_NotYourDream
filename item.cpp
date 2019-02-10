#include "item.h"

item::item()
{

}

QString item::getI_classid() const
{
    return i_classid;
}

void item::setI_classid(const QString &value)
{
    i_classid = value;
}

QString item::getI_instanceid() const
{
    return i_instanceid;
}

void item::setI_instanceid(const QString &value)
{
    i_instanceid = value;
}

void item::printItem()
{

    qDebug()<<"Name :"+name+"\ni_classid :"+i_classid+"\n"+"i_instanceid :"+i_instanceid+"\n"+"price: "+QString::number(price);


}

void item::printPlacedItem()
{
    if(status.contains("true"))
    {
        qDebug()<<"i_classid"<<i_classid;
        qDebug()<<"i_instanceid"<<i_instanceid;
        qDebug()<<"price"<<price;
    }
}

QString item::getStatus() const
{
    return status;
}

void item::setStatus(const QString &value)
{
    status = value;
}

long item::getPrice() const
{
    return price;
}

void item::setPrice(long value)
{
    price = value;
}

QString item::getName() const
{
    return name;
}

void item::setName(const QString &value)
{
    name = value;
}

QString item::getId() const
{
    return id;
}

void item::setId(const QString &value)
{
    id = value;
}

QString item::getUrl() const
{
    return url;
}

void item::setUrl(const QString &value)
{
    url = value;
}

