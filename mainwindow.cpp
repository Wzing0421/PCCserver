#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    recvPort = 5000; //暂定服务器的接受端口是5000
    recvSocket = new QUdpSocket(this);
    bool bindflag=  recvSocket->bind(QHostAddress::Any,recvPort);//注册消息接收端口

    if(!bindflag){
        QMessageBox box;
        box.setText(tr("初始化绑定错误！"));
        box.exec();
    }
    else{//绑定回调函数
        connect(recvSocket,SIGNAL(readyRead()),this,SLOT(recvInfo()));
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::recvInfo(){
    while(recvSocket->hasPendingDatagrams()){
        QByteArray datagram;
        datagram.resize(recvSocket->pendingDatagramSize());
        QHostAddress senderIP;
        quint16 senderPort;//
        recvSocket->readDatagram(datagram.data(),datagram.size(),&senderIP,&senderPort);//发送方的IP和port
        qDebug()<<datagram.size();

        QHostAddress UEaddr;
        UEaddr.setAddress("162.105.85.198");
        quint16 UEport = 10002;
        if(datagram[0] == 0x01){//是注册的,需要判别是否是带有鉴权注册的
            if(datagram.size()==21){//不带鉴权的注册
                datagram[2] = 0x02;
                qDebug()<<"接收到的数据长度 "<<datagram.size();
                int num = recvSocket->writeDatagram(datagram,UEaddr,UEport);
                qDebug()<<"authorized command size: "<<num;
            }
            else{//带有鉴权的注册
                datagram[2] = 0x03;
                qDebug()<<"接收到的数据长度 "<<datagram.size();
                int num = recvSocket->writeDatagram(datagram,UEaddr,UEport);
                qDebug()<<"voice register rsp size: "<<num;
            }
        }

    }
}
