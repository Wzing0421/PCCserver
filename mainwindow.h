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
#include <string>
#include <cstring>
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

    quint16 recvPort;//接收注册信息用的端口

    unsigned char authCommand[16]; //我先暂定是16字节，其中Nonce是8字节，前面信息占8字节
    unsigned char voiceRegisterRsp[8];//这是voice Register Rsp, 一期不携带IMPU(用户IMS标识)
    unsigned char voiceDeRegisterReq[10];//表示的是PCC端发送的voice DeRegister Req
    unsigned char voiceDeRegisterRsp[8];

    void init_auth();
    void init_Rsp();
    void init_DeRegisterReq();
    void init_DeRegisterRsp();
private:
    Ui::MainWindow *ui;

private slots:
    void recvInfo();//接收注册消息的回调函数
    void on_pushButton_clicked();
};

#endif // MAINWINDOW_H
