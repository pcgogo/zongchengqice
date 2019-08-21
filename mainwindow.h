#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QComboBox>
#include <QPushButton>
#include <QMessageBox>
#include <QTextCodec>
#include <QChartView>
#include <QLineSeries>
//#include <QPieSeries>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <math.h>
using namespace QtCharts;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QStringList getPortNameList();//获取所有可用的串口列表
    void connect_mysql();



private slots:

    //void on_btn_open_clicked();
    void refresh();
    void openPort();//打开串口
    void read();
    void send();

private:
    Ui::MainWindow *ui;

    QSerialPort* m_serialPort; //串口类
    QStringList m_portNameList;//保存可用的串口名
    QSqlDatabase db;
    QByteArray sendbuff;//发送缓冲器
    QByteArray dataarray;
    int receivecoutner;//接收计数
    QByteArray buf;//接收缓冲器
    QLineSeries line1; //创建图表折线对象1 显示电流数据
    QLineSeries line2; //创建图表折线对象2 显示入口气压数据
    QLineSeries line3; //创建图表折线对象3 显示出口气压数据
    QLineSeries line4; //创建图表折线对象4 显示流量数据
    QChart c;   //创建图表对象
    int errorflag;

    struct RESULT //测试结果结构体
    {
        QByteArray currentstr;   //用于保存接收的字符串
        QByteArray entrancestr;
        QByteArray exitstr;
        QByteArray flowstr;
        double current;           //用于保存传感器的实际数据
        double entrance;
        double exit;
        double flow;

    }result1,result2,result3; //创建3个测试结果结构体，分别保存三档风速下的测试结果



};

#endif // MAINWINDOW_H
