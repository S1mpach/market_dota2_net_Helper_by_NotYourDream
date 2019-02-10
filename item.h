#ifndef ITEM_H
#define ITEM_H
#include<iostream>
#include<vector>
#include<QDebug>
#include<QString>
#include<QTextEdit>
class item
{
public:
    item();
    QString getI_classid() const;
    void setI_classid(const QString &value);

    QString getI_instanceid() const;
    void setI_instanceid(const QString &value);




    void printItem();
    void printPlacedItem();

    QString getStatus() const;
    void setStatus(const QString &value);

    long getPrice() const;
    void setPrice(long value);

    QString getName() const;
    void setName(const QString &value);

    QString getId() const;
    void setId(const QString &value);

    QString getUrl() const;
    void setUrl(const QString &value);

private:
    QString name;

    QString url;
    QString i_classid;
    QString i_instanceid;
    QString status;
    QString id;
    long int price=0;
};

#endif // ITEM_H
