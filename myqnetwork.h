#ifndef MYQNETWORK_H
#define MYQNETWORK_H

#include <QObject>
#include <QNetworkAccessManager>

class MyQnetwork : public QNetworkAccessManager
{
    Q_OBJECT
public:
    explicit MyQnetwork(QObject *parent = nullptr);

signals:

public slots:


};

#endif // MYQNETWORK_H
