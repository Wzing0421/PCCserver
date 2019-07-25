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

    init_auth();
    init_Rsp();
    init_DeRegisterReq();
    init_DeRegisterRsp();
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
        if(datagram[10] == 0x01){//是注册的,需要判别是否是带有鉴权注册的
            if(datagram.size()==29){//不带鉴权的注册
                qDebug()<<"接收到的数据长度 "<<datagram.size();
                int num = recvSocket->writeDatagram((char*)authCommand,sizeof(authCommand),UEaddr,UEport);
                qDebug()<<"authorized command size: "<<num;
            }
            else{//带有鉴权的注册
                qDebug()<<"接收到的数据长度 "<<datagram.size();
                int num = recvSocket->writeDatagram((char*)voiceRegisterRsp,sizeof(voiceRegisterRsp),UEaddr,UEport);
                qDebug()<<"发送voice register rsp size: "<<num;
            }
        }
        else if(datagram[10] == 0x04){
            qDebug()<<"接收到voice DeRegister Req ,长度 "<<datagram.size();
            int num = recvSocket->writeDatagram((char*)voiceDeRegisterRsp,sizeof(voiceRegisterRsp),UEaddr,UEport);
            qDebug()<<"voice DeRegister rsp size: "<<num;
        }
        else if(datagram[10] == 0x05){
            qDebug()<<"接收到voice DeRegister Rsp ,长度是： "<<datagram.size();
        }

    }
}

void MainWindow::init_auth(){//初始化鉴权信息

    memset(authCommand,0,sizeof(authCommand));
    authCommand[0] = 0x00;//Protocal version
    authCommand[1] = 0x10;//Message length == 16 byte
    authCommand[2] = 0x02;//Message type
    //后面的STMSI和Nonce我先都置成0
    authCommand[15] = 0x01;//算了我把nonce最后一位置为1吧

}

void MainWindow::init_Rsp(){//初始化voice register Rsp
    memset(voiceRegisterRsp,0,sizeof(voiceRegisterRsp));
    voiceRegisterRsp[0] = 0x00;//Protocol version
    voiceRegisterRsp[1] = 0x0a;//Message length == 10 byte
    voiceRegisterRsp[2] = 0x03;//Message type
    //STMSI 先设置成0
    voiceRegisterRsp[8] = 0x00;//casue : 成功
    voiceRegisterRsp[9] = 0x01;//单位是3600s，置为0x01表示每3600s周期注册一次
}

void MainWindow::init_DeRegisterReq(){//初始化voice DeRegister Req
    voiceDeRegisterReq[0] = 0x00;
    voiceDeRegisterReq[1] = 0x0a;//message length == 10byte
    voiceDeRegisterReq[2] = 0x04;//message type;
    //略去STMSI部分
    voiceDeRegisterReq[8] = 0x00;//网络侧注销
    voiceDeRegisterReq[9] = 0x01;
}

void MainWindow::init_DeRegisterRsp(){
    memset(voiceDeRegisterRsp,0,sizeof(voiceDeRegisterRsp));
    voiceDeRegisterRsp[0] = 0x00;
    voiceDeRegisterRsp[1] = 0x08;//message length
    voiceDeRegisterRsp[2] = 0x05;//message type
    //后面5个字节的STMSI目前就是0，是和前面RegMsg是一样的
}

void MainWindow::on_pushButton_clicked()
{
    QHostAddress UEaddr;
    UEaddr.setAddress("162.105.85.198");
    quint16 UEport = 10002;
    int num = recvSocket->writeDatagram((char*)voiceDeRegisterReq,sizeof(voiceDeRegisterReq),UEaddr,UEport);
    qDebug()<<"voice DeRegister Req size: "<<num;

}
