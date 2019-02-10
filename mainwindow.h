#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QNetworkReply>
#include <QNetworkAccessManager>
#include<QJsonDocument>
#include<QJsonArray>
#include<QJsonObject>
#include<QList>
#include<fstream>
#include <QFile>
#include<QMessageBox>
#include<vector>
#include <windows.h>
#include<thread>
#include<QTimer>
#include<QThread>
#include <pthread.h>
#include<QMutex>
#include <chrono>
#include <ctime>
#include<Qt>
#include<iostream>
#include<vector>
#include<item.h>
#include<QDesktopServices>
#include<QTextEdit>
#include<QScrollBar>
#include<QMutex>
#include <ctime>
#include<QString>
#include<QEventLoop>
#include<memory.h>
namespace Ui {
class MainWindow;
}





static QString apiKey ="53dFjbGt8u81O52L94IK2044Gd0lZWY";


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    size_t itemPosition = 0;
    int counter = 0;
    int missedItems = 0;
    size_t counterSell = 0;
    QMutex mutex;

    std::vector<int> priceList;

    double percent = 0;

    int itemPrice=-1;

    int numberOfErrors=0;
    std::vector<item> itemList;
    QMap<QString,item*> my_map;


    QMap <QString,item*> map_repeatSell;
    QMap<QString,item*> map_Name;

    void repeatTrade(QUrl url);
    void getPriceByName(QString url);
    bool isInventoryEmpty;

    void sellItemOldApi(QString url);
    QEventLoop OldApiloop;
    QNetworkAccessManager*  Error429SlotManager;
    QNetworkAccessManager*  ManagerBysellItemOldApi ;

signals:

    void isAvaible();
    void isError429Signal(QString url);
    void delaySignal(int ms);
    void signalShowCounter(int counter);
    void signalShowMissed(int counter);
  void signalShowStatus(QString status);
private slots:
    void HTTPGetInv(QNetworkReply *reply);
    void HTTPGetBestSelllOfferUrl(QNetworkReply *reply);
    void HTTPGetBestSelllOfferByPercent(QNetworkReply *reply);
    void HTTPRefresh(QNetworkReply *reply);
    void HTTPSetPrice(QNetworkReply *reply);
    void HTTPGetPriceByName(QNetworkReply *reply);
    void delayHTTP(int ms);
    void showCounter(int counter);
    void Error429SLot(QString url);
    void ShowMissed(int counter);
     void ShowStatus(QString status);

    void on_pushButton_GetInventory_clicked();
    void on_pushButton_GetPrice_clicked();
    void on_pushButtonPrintList_clicked();
    void on_pushButtonSellItem_clicked();
    void on_pushButtonSetKey_clicked();
    void on_doubleSpinBox_editingFinished();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_Telegram_clicked();
    void on_pushButton_refresh_clicked();
    void on_pushButton_ByName_clicked();
    void on_pushButton_5_clicked();
};

#endif // MAINWINDOW_H
