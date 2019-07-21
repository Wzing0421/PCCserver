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
    //QUdpSocket *sendSocket;//用于发送注册信息的socket
    quint16 recvPort;//接收注册信息用的端口
    //quint16 regsendPort;//向PCC发送注册信息的目的端口


private:
    Ui::MainWindow *ui;

private slots:
    void recvInfo();//接收注册消息的回调函数
};

#endif // MAINWINDOW_H
