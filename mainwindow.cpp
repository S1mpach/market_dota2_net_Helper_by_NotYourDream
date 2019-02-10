#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QSsl>
#include<QSslSocket>
#include<QSslConfiguration>
#include<openssl/aes.h>
//#include<op
using namespace std;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    this->setFixedSize(573,846);
    isInventoryEmpty=true;


    ui->setupUi(this);


    ui->pushButton_3->setStyleSheet("QPushButton { background: none; border: none; outline: none; color: transparent; } ");
    ui->pushButton_Telegram->setStyleSheet("QPushButton { background: none; border: none; outline: none; color: transparent; } ");
    ui->pushButton_5->setStyleSheet("QPushButton { background: none; border: none; outline: none; color: transparent; } ");


    Error429SlotManager = new QNetworkAccessManager(this);
    connect(Error429SlotManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(HTTPGetPriceByName(QNetworkReply*)),Qt::DirectConnection);

    connect(this, SIGNAL(signalShowStatus(QString)),
            this, SLOT(ShowStatus(QString)),Qt::DirectConnection);

    connect(this, SIGNAL(signalShowCounter(int)),
            this, SLOT(showCounter(int)),Qt::DirectConnection);

    connect(this, SIGNAL(delaySignal(int)),
            this, SLOT(delayHTTP(int)),Qt::DirectConnection);

    connect(this, SIGNAL(signalShowMissed(int)),
            this, SLOT(ShowMissed(int)),Qt::DirectConnection);

    ui->pushButton_ByName->setEnabled(false);
    ui->pushButton_GetPrice->setEnabled(false);
    ui->pushButton_GetInventory->setEnabled(false);
    ui->pushButtonPrintList->setEnabled(false);
    ui->pushButton->setEnabled(false);
    ui->pushButtonSellItem->setEnabled(false);
    ui->pushButton_ByName->setEnabled(false);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::HTTPGetInv(QNetworkReply *reply)
{


    if (reply->error() != QNetworkReply::NoError) {
        //        qDebug() << "Got some error " << reply->error();
        //        QCoreApplication::exit(1);
    }

    const int resultCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << "Received code " << resultCode;
    if (resultCode != 200) {
        //        QCoreApplication::exit(1);
    }


    ////////////////////////////////////////// Parse title , etc //////////////////////////////////
    ///
    QString strReply = QString::fromStdString(reply->readAll().toStdString());



    //    std::cout << "HERE "<<strReply.toStdString()<<endl;

    if(strReply.contains("Bad KEY"))
    {
        QMessageBox msgBox;  //www.itmathrepetitor.ru
        msgBox.setText("Unvalid API Key !");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }
    else if(strReply.contains("http://steamcommunity.com/profiles/"))
    {
        QMessageBox msgBox;  //www.itmathrepetitor.ru
        msgBox.setText("Try send request one more time !");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
    } else if(strReply.contains("\"success\":true,\"items\""))
    {


        //        cout<<"here"<<strReply.toStdString()<<endl;

        QStringList propertyNames;
        QStringList propertyKeys;

        QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());

        //    qDebug()<<strReply.toUtf8()<<endl;

        if(jsonResponse.isEmpty())
            cout<<"Empty"<<endl;



        QJsonObject jsonObj = jsonResponse.object();




        if(  jsonObj.isEmpty())
            cout<<"Empty 3"<<endl;


        QJsonArray jsonArray = jsonObj.value("items").toArray();


        emit signalShowStatus("Try , get your inventory.");
        if(jsonArray.isEmpty())
        {
            qDebug()<<strReply;

            isInventoryEmpty=true;
            numberOfErrors++;
            if(numberOfErrors==100)
                QCoreApplication::exit();
            emit signalShowStatus("Oops , I not get it...");
            return;
        }
        isInventoryEmpty=false;

        QMessageBox msgBox;  //www.itmathrepetitor.ru
        msgBox.setText("Nice , i got your inventory.");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();



        item currentItem;
        for(int i = 0 ; i < jsonArray.size(); i++)
        {


            //        qDebug() << jsonArray[i].toObject().value("i_classid").toString()<<endl;
            //        qDebug() << jsonArray[i].toObject().value("i_instanceid").toString()<<endl;
            //        qDebug()<<endl<<endl;

            currentItem.setName(jsonArray[i].toObject().value("market_hash_name").toString());
            currentItem.setId(jsonArray[i].toObject().value("id").toString());
            currentItem.setI_classid(jsonArray[i].toObject().value("classid").toString());
            currentItem.setI_instanceid(jsonArray[i].toObject().value("instanceid").toString());
            currentItem.setPrice(jsonArray[i].toObject().value("market_price").toDouble()*100);

            itemList.push_back(currentItem);
            if(itemList.size()>500)
                break;


        }

        ui->label_Items->setText(QString::number(itemList.size()));
    }else{
        QMessageBox msgBox;
        msgBox.setText("Unknown error");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        ofstream logs("logs.txt");
        logs<<strReply.toStdString().c_str();

    }


    reply->deleteLater();
}

void MainWindow::HTTPGetBestSelllOfferUrl(QNetworkReply *reply)
{

    qDebug()<<reply->url();
    auto myobjectiterator = my_map.find(reply->url().toString());

    qDebug()<<"Take it";

    if (reply->error() != QNetworkReply::NoError) {
        //        qDebug() << "Got some error " << reply->error();
        //        QCoreApplication::exit(1);
    }

    const int resultCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //    qDebug() << "Received code " << resultCode;
    if (resultCode != 200) {
        //        QCoreApplication::exit(1);
    }

    QString strReply = QString::fromStdString(reply->readAll().toStdString());


    QStringList propertyNames;
    QStringList propertyKeys;

    QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());


    QJsonObject jsonObj = jsonResponse.object();

    itemPrice=jsonObj.value("best_offer").toString().toInt();


    if(itemPrice == 0)
    {
        missedItems++;

    }
    else if(itemPrice < 50)
    {
        myobjectiterator.value()->setPrice(50);

    } else
        myobjectiterator.value()->setPrice(itemPrice);









    //    ui->textEditLogPAnel->setText(ui->textEditLogPAnel->toPlainText()+"Items scanned :"+
    //                                  QString::number(itemPosition)+"/"+ QString::number(itemList.size())+" | Missed:"+ QString::number(missedItems)+"\n");
    counter++;
    emit showCounter(counter);



    reply->deleteLater();
}

void MainWindow::HTTPGetBestSelllOfferByPercent(QNetworkReply *reply)
{

    qDebug()<<reply->url();
    auto myobjectiterator = my_map.find(reply->url().toString());

    myobjectiterator.value()->setUrl(reply->url().toString());
    qDebug()<<"Take it";

    if (reply->error() != QNetworkReply::NoError) {
        //        qDebug() << "Got some error " << reply->error();
        //        QCoreApplication::exit(1);
    }

    const int resultCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //    qDebug() << "Received code " << resultCode;
    if (resultCode != 200) {
        //        QCoreApplication::exit(1);
    }

    QString strReply = QString::fromStdString(reply->readAll().toStdString());


    QStringList propertyNames;
    QStringList propertyKeys;

    QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());


    QJsonObject jsonObj = jsonResponse.object();

    itemPrice=jsonObj.value("best_offer").toString().toInt();

    qDebug()<<myobjectiterator.value()->getName();
    qDebug()<<itemPrice;

    if(itemPrice == 0 )
    {

        myobjectiterator.value()->setPrice(-1);
        missedItems++;
    }else
    {
        itemPrice = itemPrice - itemPrice*percent;


        if(itemPrice < 50 && itemPrice > 1 )
        {
            myobjectiterator.value()->setPrice(50);

        } else
            myobjectiterator.value()->setPrice(itemPrice);







        //    ui->textEditLogPAnel->setText(ui->textEditLogPAnel->toPlainText()+"Items scanned :"+
        //                                  QString::number(itemPosition)+"/"+ QString::number(itemList.size())+" | Missed:"+ QString::number(missedItems)+"\n");
        counter++;

        emit showCounter(counter);

        emit delaySignal(10);

    }

    reply->deleteLater();
}

void MainWindow::HTTPRefresh(QNetworkReply *reply)
{

    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Got some error " << reply->error();
        //        QCoreApplication::exit(1);

        ui->textEditLogPAnel->setText("Error ... Inventory not refreshed...");
    }

    const int resultCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << "Received code " << resultCode;
    if (resultCode != 200) {
        //        QCoreApplication::exit(1);

        ui->textEditLogPAnel->setText("Error ... Inventory not refreshed...");
    }
    QMessageBox msgBox;
    msgBox.setText("Inventory refreshed...");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();

    ////////////////////////////////////////// Parse title , etc //////////////////////////////////
    ///
    QString strReply = QString::fromStdString(reply->readAll().toStdString());


    qDebug()<<reply->url();

    qDebug()<<strReply;

    ui->textEditLogPAnel->setText("Inventory refreshed...");

    itemPosition=0;
    missedItems=0;
    counterSell=0;

    reply->deleteLater();
}

void MainWindow::HTTPSetPrice(QNetworkReply *reply)
{


    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Got some error " << reply->error();
        //        QCoreApplication::exit(1);
    }

    const int resultCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << "Received code " << resultCode;
    if (resultCode != 200) {
        //        QCoreApplication::exit(1);
    }


    ////////////////////////////////////////// Parse title , etc //////////////////////////////////
    ///
    QString strReply = QString::fromStdString(reply->readAll().toStdString());


    qDebug() <<reply->url();

    qDebug() <<strReply;

    if(strReply.contains("\"success\":true"))
    {
        counterSell++;
        ui->textEditLogPAnel->setText("Items sold:"+QString::number(counterSell)+" | "+QString::number(itemList.size())+"\n"
                                      +ui->textEditLogPAnel->toPlainText());
    }else if(strReply.contains("item_not_inserted")||strReply.contains("Too Many Requests") )
        repeatTrade(reply->url());
    else if(strReply.contains("item_not_in_inventory"))
        sellItemOldApi(reply->url().toString());


    reply->deleteLater();
}

void MainWindow::HTTPGetPriceByName(QNetworkReply *reply)
{

    if (reply->error() != QNetworkReply::NoError) {
        //        qDebug() << "Got some error " << reply->error();
        //        QCoreApplication::exit(1);
    }

    const int resultCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << "Received code " << resultCode;
    if (resultCode != 200) {
        //        QCoreApplication::exit(1);
    }

    QString urlTemp=reply->url().toString();

    auto myobjectiterator = map_Name.find(urlTemp);

    qDebug()<<"UrlTemp"<<urlTemp;

    QString strReply = QString::fromStdString(reply->readAll().toStdString());


    std::cout << "HEaRE "<<strReply.toStdString()<<endl;


    if(resultCode==429)
    {

        if(counter<itemList.size())
        {
            emit delaySignal(250);
            emit Error429SLot(urlTemp);
        }
    }else if(strReply.contains("price")){
        ////////////////////////////////////////// Parse title , etc //////////////////////////////////
        ///




        QStringList propertyNames;
        QStringList propertyKeys;

        QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());

        //    qDebug()<<strReply.toUtf8()<<endl;




        if(jsonResponse.isEmpty())
            cout<<"Empty"<<endl;



        QJsonObject jsonObj = jsonResponse.object();




        if(  jsonObj.isEmpty())
            cout<<"Empty 3"<<endl;


        QJsonArray jsonArray = jsonObj.value("data").toArray();


        myobjectiterator.value()->setPrice(jsonArray[0].toObject().value("price").toInt());

        qDebug()<<"Price--------------------------------------------------------";
        qDebug()<<"Price: "<<jsonArray[0].toObject().value("price").toInt()-jsonArray[0].toObject().value("price").toInt()*percent;

        qDebug()<<"Price--------------------------------------------------------";


        counter++;
        emit signalShowCounter(counter);

    }

    reply->deleteLater();

}

void MainWindow::delayHTTP(int ms)
{
    Sleep(ms);

}

void MainWindow::showCounter(int counter)
{
    ui->textEditLogPAnel->setText("Items scanned :"+
                                  QString::number(counter)+"/"+ QString::number(itemList.size())+"\n"+
                                  ui->textEditLogPAnel->toPlainText());
}

void MainWindow::Error429SLot(QString url)
{



    QNetworkRequest request;
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1_2);
    request.setSslConfiguration(config);

    request.setHeader(QNetworkRequest::ServerHeader, "application");





    emit delaySignal(200);


    request.setUrl(url);
    Error429SlotManager->get(request);
}

void MainWindow::ShowMissed(int counter)
{
    ui->textEditLogPAnel->setText("Items *MISSED* :"+
                                  QString::number(counter)+"\n"+
                                  ui->textEditLogPAnel->toPlainText());

}

void MainWindow::ShowStatus(QString status)
{
    ui->textEditLogPAnel->setText("Status :"+ status+"\n"+
                                  ui->textEditLogPAnel->toPlainText());

}

void MainWindow::on_pushButton_GetInventory_clicked()
{

    ui->pushButton_GetInventory->setEnabled(false);
    QString getInvUrl = "https://market.dota2.net/api/v2/my-inventory/?key=";



    QNetworkRequest request;
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1_2);
    request.setSslConfiguration(config);

    QEventLoop loop;



    counter=0;
    qDebug()<<getInvUrl+apiKey<<endl;
    request.setUrl(QUrl(getInvUrl+apiKey));

    request.setHeader(QNetworkRequest::ServerHeader, "application");
    QNetworkAccessManager*  nam = new QNetworkAccessManager(this);
    connect(nam, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(HTTPGetInv(QNetworkReply*)),Qt::DirectConnection);




    while(isInventoryEmpty)
    {

        QNetworkReply* reply= nam->get(request);

        connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));

        loop.exec();


    }
    ui->pushButton_GetInventory->setEnabled(false);


    ui->pushButton_GetPrice->setEnabled(true);
    ui->pushButtonPrintList->setEnabled(true);
    ui->pushButton->setEnabled(true);
    ui->pushButtonSellItem->setEnabled(true);


}

void MainWindow::on_pushButton_GetPrice_clicked()
{
    QString getBestSelllOfferUrl = "https://market.dota2.net/api/BestBuyOffer/";



    QNetworkRequest request;
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1_2);
    request.setSslConfiguration(config);



    request.setHeader(QNetworkRequest::ServerHeader, "application");

    QNetworkAccessManager*  nam = new QNetworkAccessManager(this);
    connect(nam, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(HTTPGetBestSelllOfferUrl(QNetworkReply*)),Qt::DirectConnection);



    //    connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(waitFinish()) , Qt::QueuedConnection);

    for(size_t i = 0 ; i<itemList.size();i++)
    {
        my_map.insert(getBestSelllOfferUrl + itemList[i].getI_classid()+"_"+itemList[i].getI_instanceid()+"/?key="+apiKey,&itemList[i]);
        request.setUrl(QUrl(getBestSelllOfferUrl + itemList[i].getI_classid()+"_"+itemList[i].getI_instanceid()+"/?key="+apiKey));
        nam->get(request);
        qDebug()<<i;
        Sleep(1);
    }






}

void MainWindow::on_pushButtonPrintList_clicked()
{
    for(size_t i = 0 ; i<itemList.size();i++)
    {
        //        itemList[i].printItem();
        //        qDebug()<<priceList[i]<<endl<<endl;

        emit signalShowStatus("\n"+itemList[i].getName()+
                              "\nPrice:"+QString::number(itemList[i].getPrice())+"\nId:"+itemList[i].getId()+"\n"
                              +itemList[i].getI_classid()
                              +"\n"+itemList[i].getI_instanceid()+"\n\n");

    }
    //    ui->textEditLogPAnel->setText(ui->textEditLogPAnel->toPlainText()+"MissedItems: "+QString::number(missedItems)+"\n");

    //    ui->textEditLogPAnel->verticalScrollBar()->setValue(ui->textEditLogPAnel->verticalScrollBar()->maximum());


}

void MainWindow::on_pushButtonSellItem_clicked()
{











    itemPosition = 0;

    ////////////////////////////////////////

    QNetworkAccessManager*  ManagerBysellItemOldApi = new QNetworkAccessManager(this);

    connect(ManagerBysellItemOldApi,SIGNAL(finished(QNetworkReply*)),&OldApiloop, SLOT(quit()), Qt::DirectConnection);

    ////////////////////////////////////////


    QString getSetPriceUrl = "https://market.dota2.net/api/v2/add-to-sale?key=";
    //    &cur=[currency]

    QNetworkRequest request;
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1_2);
    request.setSslConfiguration(config);

    QEventLoop loop;




    request.setHeader(QNetworkRequest::ServerHeader, "application");

    QNetworkAccessManager*  nam = new QNetworkAccessManager(this);

    connect(nam,SIGNAL(finished(QNetworkReply*)),&loop, SLOT(quit()), Qt::DirectConnection);

    connect(nam, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(HTTPSetPrice(QNetworkReply*)));


    QString tempUrl;
    for(size_t i = 0 ; i<itemList.size();i++)
    {

        if(itemList[i].getPrice()>=50)
        {
            tempUrl=getSetPriceUrl+apiKey+"&id="+itemList[i].getId()+"&price="+QString::number(itemList[i].getPrice())+"&cur=RUB";
            map_repeatSell.insert(tempUrl,&itemList[i]);
            request.setUrl(QUrl(tempUrl));
            //                       request.setUrl(QUrl("https://market.dota2.net/api/SetPrice/new_1987653240_948149725/50/?key=Eq7Vj1jdIl0a8aXZ6YpPffLV04Y4H9z"));
            nam->get(request);
            loop.exec();
        }

        //            ui->textEditLogPAnel->setText(ui->textEditLogPAnel->toPlainText()+getSetPriceUrl
        //                                          + itemList[i].getI_classid()+"_"+itemList[i].getI_instanceid()+"/"
        //                                          +QString::number(itemList[i].getPrice())+"/?key="+apiKey+"\n");

        //            Sleep(100);

        //        itemPosition++;

    }


}


void MainWindow::on_pushButtonSetKey_clicked()
{
    apiKey=ui->lineEdit->text();

    QMessageBox msgBox;
    msgBox.setText("New API Key is set !");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();

    ui->pushButton_ByName->setEnabled(false);
    ui->pushButton_GetPrice->setEnabled(false);
    ui->pushButton_GetInventory->setEnabled(true);
    ui->pushButtonPrintList->setEnabled(false);
    ui->pushButton->setEnabled(false);
    ui->pushButtonSellItem->setEnabled(false);
    ui->pushButton_ByName->setEnabled(false);

}

void MainWindow::on_doubleSpinBox_editingFinished()
{
    percent=ui->doubleSpinBox->value();
    qDebug()<<percent;
}

void MainWindow::on_pushButton_clicked()
{
    QString getBestSelllOfferUrl = "https://market.dota2.net/api/BestSellOffer/";



    QNetworkRequest request;
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1_2);
    request.setSslConfiguration(config);



    request.setHeader(QNetworkRequest::ServerHeader, "application");

    QNetworkAccessManager*  nam = new QNetworkAccessManager(this);
    connect(nam, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(HTTPGetBestSelllOfferByPercent(QNetworkReply*)),Qt::DirectConnection);



    //    connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(waitFinish()) , Qt::QueuedConnection);

    for(size_t i = 0 ; i<itemList.size();i++)
    {
        my_map.insert(getBestSelllOfferUrl + itemList[i].getI_classid()+"_"+itemList[i].getI_instanceid()+"/?key="+apiKey,&itemList[i]);
        request.setUrl(QUrl(getBestSelllOfferUrl + itemList[i].getI_classid()+"_"+itemList[i].getI_instanceid()+"/?key="+apiKey));
        nam->get(request);
        qDebug()<<i;

        emit delaySignal(10);
    }


    ui->pushButton_ByName->setEnabled(true);
}

void MainWindow::on_pushButton_2_clicked()
{
    itemList.clear();
    itemPosition=0;
    missedItems=0;
    counterSell=0;
    ui->pushButton_ByName->setEnabled(false);
    ui->pushButton_GetPrice->setEnabled(false);
    ui->pushButton_GetInventory->setEnabled(true);
    ui->pushButtonPrintList->setEnabled(false);
    ui->pushButton->setEnabled(false);
    ui->pushButtonSellItem->setEnabled(false);
    ui->pushButton_ByName->setEnabled(false);
}

void MainWindow::on_pushButton_3_clicked()
{
    QString link = "https://steamcommunity.com/profiles/76561198098658992";
    QDesktopServices::openUrl(QUrl(link));
}

void MainWindow::on_pushButton_4_clicked()
{
    ui->textEditLogPAnel->clear();
}



void MainWindow::on_pushButton_Telegram_clicked()
{
    QString link = "https://telegram.me/notYourDream";
    QDesktopServices::openUrl(QUrl(link));
}

void MainWindow::on_pushButton_refresh_clicked()
{
    QString refreshUrl = "https://market.dota2.net/api/UpdateInventory/?key=";



    QNetworkRequest request;
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1_2);
    request.setSslConfiguration(config);



    request.setHeader(QNetworkRequest::ServerHeader, "application");

    QNetworkAccessManager*  nam = new QNetworkAccessManager(this);
    connect(nam, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(HTTPRefresh(QNetworkReply*)),Qt::DirectConnection);
    request.setUrl(QUrl(refreshUrl+apiKey));
    nam->get(request);

}

void MainWindow::repeatTrade(QUrl url)
{

    QNetworkRequest request;
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1_2);
    request.setSslConfiguration(config);

    request.setHeader(QNetworkRequest::ServerHeader, "application");

    QNetworkAccessManager*  nam = new QNetworkAccessManager(this);
    connect(nam, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(HTTPSetPrice(QNetworkReply*)),Qt::DirectConnection);

    request.setUrl(url);

    emit delaySignal(100);
    nam->get(request);






}

void MainWindow::getPriceByName( QString url)
{

    qDebug()<<"getPriceByName";
    QString getPriceByNameUrl="https://market.dota2.net/api/v2/search-item-by-hash-name?key=";
    auto myobjectiterator = my_map.find(url);


    QNetworkRequest request;
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1_2);
    request.setSslConfiguration(config);

    request.setHeader(QNetworkRequest::ServerHeader, "application");

    QNetworkAccessManager*  nam = new QNetworkAccessManager(this);
    connect(nam, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(HTTPGetPriceByName(QNetworkReply*)),Qt::DirectConnection);




    emit delaySignal(250);

    QString hash_Name=myobjectiterator.value()->getName();

    hash_Name=hash_Name.replace(" ","+");
    QString urlEnd;
    qDebug()<<hash_Name;

    urlEnd=getPriceByNameUrl+apiKey+"&hash_name="+hash_Name;

    map_Name.insert(urlEnd , myobjectiterator.value());


    qDebug()<<urlEnd;

    request.setUrl(urlEnd);
    nam->get(request);

}

void MainWindow::sellItemOldApi(QString url)
{
    auto iterator_map = map_repeatSell.find(url);
    qDebug()<<"sellItemOldApi";
    if (iterator_map != map_repeatSell.end())
    {
        //https://market.dota2.net/api/SetPrice/new_[classid]_[instanceid]/[price]/?key=[your_secret_key]

        QNetworkRequest request;
        QSslConfiguration config = QSslConfiguration::defaultConfiguration();
        config.setProtocol(QSsl::TlsV1_2);
        request.setSslConfiguration(config);




        QString getSetPriceUrl = "https://market.dota2.net/api/SetPrice/new_"+iterator_map.value()->getI_classid()+
                iterator_map.value()->getI_instanceid()+"/"+QString::number(iterator_map.value()->getPrice())+"/?key="+apiKey;


        request.setHeader(QNetworkRequest::ServerHeader, "application");




        QNetworkReply *replyReq;
        request.setUrl(QUrl(getSetPriceUrl));
        replyReq = ManagerBysellItemOldApi->get(request);
        OldApiloop.exec();
        QString strReply=replyReq->readAll();
        qDebug()<<strReply;

        if(strReply.contains("\"result\":true,\"item_id\""))
            ui->textEditLogPAnel->setText("Items sold V1:"+QString::number(counterSell)+" | "+QString::number(itemList.size())+"\n"
                                          +ui->textEditLogPAnel->toPlainText());


    }else
        qDebug()<<"Not found";


}


void MainWindow::on_pushButton_ByName_clicked()
{


    for(size_t i = 0 ; i<itemList.size();i++)
    {
        if(itemList[i].getPrice()==-1)
        {
            getPriceByName(itemList[i].getUrl());

            emit delaySignal(10);
        }
    }








}

void MainWindow::on_pushButton_5_clicked()
{
    QString link = "https://qiwi.me/market-helper-by-notyourdream";
    QDesktopServices::openUrl(QUrl(link));
}
