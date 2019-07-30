#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    UEaddr.setAddress("162.105.85.198");
    UEport = 10002;

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
    /*初始化呼叫信令*/
    string calledBCDNumber = "15650709603";
    init_callSetup(calledBCDNumber);

    init_callSetupAck();
    init_callAlerting();
    init_callConnect();
    init_callConnectAck();
    int cause = 27;
    init_callDisconnect(cause);//UE正常呼叫释放
    init_callReleaseRsp(cause);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete recvSocket;
}
void MainWindow::recvInfo(){
    while(recvSocket->hasPendingDatagrams()){
        QByteArray datagram;
        datagram.resize(recvSocket->pendingDatagramSize());
        QHostAddress senderIP;
        quint16 senderPort;//
        recvSocket->readDatagram(datagram.data(),datagram.size(),&senderIP,&senderPort);//发送方的IP和port
        char judge = datagram[10];

        if(judge == 0x01){//是注册的,需要判别是否是带有鉴权注册的
            if(datagram.size()==29){//不带鉴权的注册
                qDebug()<<"接收到的数据长度 "<<datagram.size();
                int num = recvSocket->writeDatagram((char*)authCommand,sizeof(authCommand),UEaddr,UEport);
                qDebug()<<"authorized command size: "<<num;
                qDebug()<<"------------";
            }
            else{//带有鉴权的注册
                qDebug()<<"接收到的数据长度 "<<datagram.size();
                qDebug()<<"------------";
                int num = recvSocket->writeDatagram((char*)voiceRegisterRsp,sizeof(voiceRegisterRsp),UEaddr,UEport);
                qDebug()<<"发送voice register rsp size: "<<num;
                qDebug()<<"------------";
            }
        }
        else if(judge == 0x04){
            qDebug()<<"接收到voice DeRegister Req ,长度 "<<datagram.size();
            qDebug()<<"------------";
            int num = recvSocket->writeDatagram((char*)voiceDeRegisterRsp,sizeof(voiceRegisterRsp),UEaddr,UEport);
            qDebug()<<"voice DeRegister rsp size: "<<num;
            qDebug()<<"------------";
        }
        else if(judge == 0x05){
            qDebug()<<"------------";
            qDebug()<<"接收到voice DeRegister Rsp ,长度是： "<<datagram.size();
            qDebug()<<"------------";
        }
        else if(judge == 0x06){//收到call setup
            qDebug()<<"收到call set up, 长度是: "<<datagram.size();
            qDebug()<<"------------";
            //send call setup ack

            int num = recvSocket->writeDatagram((char*)callSetupAck,sizeof(callSetupAck),UEaddr,UEport);
            qDebug()<<"send call setup ack size: "<<num;
            qDebug()<<"------------";

            QThread::sleep(2);
            //send call alerting
            num = recvSocket->writeDatagram((char*)callAllerting,sizeof(callAllerting),UEaddr,UEport);
            qDebug()<<"send call allerting size: "<<num;
            qDebug()<<"------------";

            QThread::sleep(2);
            //send call connect
            num = recvSocket->writeDatagram((char*)callConnect,sizeof(callConnect),UEaddr,UEport);
            qDebug()<<"send call connect size: "<<num;
            qDebug()<<"------------";
        }
        else if(judge == 0x0a){
            qDebug()<<"收到 call connect ack ";
            qDebug()<<"------------";
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
void MainWindow::init_callSetup(string calledBCDNumber){
    if(calledBCDNumber.size()!=11){
        qDebug()<<"电话号码长度有误！";
        return;
    }
    callSetup[0] = 0x00;//Protocol version
    callSetup[1] = 0x15;//Message length == 21
    callSetup[2] = 0x06;//Message type
    //call ID 4个字节填全F
    memset(callSetup+3, 255,4);
    //STMSI
    memset(callSetup+7,0,5);
    //call type
    callSetup[12] = 0x01;
    //init called Party BCD Number
    callSetup[13] = 0x03;// tag
    callSetup[14] = 0x08;// length of called BCD number

    /*init BCD number*/
    calledBCDNumber += '?';//为了最后可以补一个1111
    unsigned char nums[6];
    memset(nums,0,6);
    for(int i=0;i<=5;i++){//注意大端
        int index1 = 2*i; //低位数字存在一个字节里面的低4位
        int index2 = 2*i+1;	//高位数字
        int num1 = int(calledBCDNumber[index1]- '0');
        unsigned char num1c = num1;
        nums[i] = nums[i] | num1c;
        int num2 = int(calledBCDNumber [index2] -'0');
        unsigned char num2c = (num2<<4);
        nums[i] = nums[i] | num2c;
    }
    memcpy(callSetup+15,nums,6);
}

void MainWindow::init_callSetupAck(){
    callSetupAck[0] = 0x00;//protocol version
    callSetupAck[1] = 0x07;//message length
    callSetupAck[2] = 0x07;//message type
    //后面的需要memcpy以下从PCC发送来的call ID
}

void MainWindow::init_callAlerting(){
    callAllerting[0] = 0x00;//protocol version
    callAllerting[1] = 0x07;//message length
    callAllerting[2] = 0x08;//message type
    //后面的需要memcpy以下从PCC发送来的call ID
}

void MainWindow::init_callConnect(){
    callConnect[0] = 0x00;//protocol version
    callConnect[1] = 0x08;//message length
    callConnect[2] = 0x09;//message type
    //后面的需要memcpy以下从PCC发送来的call ID
    callConnect[7] = 0x01;//call type
}

void MainWindow::init_callConnectAck(){
    callConnectAck[0] = 0x00;//protocol version
    callConnectAck[1] = 0x07;//message length
    callConnectAck[2] = 0x0a;//message type
    //后面的需要memcpy以下从PCC发送来的call ID
}

void MainWindow::init_callDisconnect(int cause){

    callDisconnect[0] = 0x00;//protocol version
    callDisconnect[1] = 0x07;//message length
    callDisconnect[2] = 0x0b;//message type
    //后面的需要memcpy以下从PCC发送来的call ID

    callDisconnect[8] = char(cause);//casue

}
void MainWindow::init_callReleaseRsp(int cause){

    callReleaseRsp[0] = 0x00;//protocol version
    callReleaseRsp[1] = 0x08;//message length
    callReleaseRsp[2] = 0x0d;//message type
    //后面的需要memcpy以下从PCC发送来的call ID

    callReleaseRsp[8] = char(cause);//casue
}
//注意初始化还需要改成是PCC专用的初始化
