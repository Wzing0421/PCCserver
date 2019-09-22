#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QString>
#include <QtNetwork/QUdpSocket>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QMessageBox>
#include <QDebug>
#include <QThread>

#include <string>
#include <cstring>
using namespace std;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QUdpSocket *recvSocket;

    QHostAddress ANCaddr;
    quint16 ANCport;
    quint16 recvPort;//接收注册信息用的端口

    unsigned char authCommand[17]; //我先暂定是17字节，其中Nonce是9字节，前面信息占8字节
    unsigned char voiceRegisterRsp[10];//这是voice Register Rsp, 一期不携带IMPU(用户IMS标识)
    unsigned char voiceDeRegisterReq[10];//表示的是PCC端发送的voice DeRegister Req
    unsigned char voiceDeRegisterRsp[8];

    void init_auth();
    void init_Rsp();
    void init_DeRegisterReq();
    void init_DeRegisterRsp();

    /*呼叫状态的变量*/
    unsigned char callSetup[16];//主叫和被叫的callsetup是不同的
    unsigned char callSetupAck[7];
    unsigned char callAllerting[7];
    unsigned char callConnect[8];
    unsigned char callConnectAck[7];
    unsigned char callDisconnect[8];
    unsigned char callReleaseRsp[8];
    unsigned char callReleaseReq[8];
    /*以下是呼叫过程的初始化*/
    void init_callSetup(string calledBCDNumber);
    void init_callSetupAck();
    void init_callAlerting();
    void init_callConnect();
    void init_callConnectAck();
    void init_callDisconnect(int cause);
    void init_callReleaseRsp(int cause);
    void init_callReleaseReq(int cause);

private:
    Ui::MainWindow *ui;

private slots:
    void recvInfo();//接收注册消息的回调函数
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
};

#endif // MAINWINDOW_H
