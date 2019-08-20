#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QtDebug>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_portNameList = getPortNameList();//获取可用串口名
    ui->comboBox->addItems(m_portNameList);//将可用串口添加到comboBox中

    //连接到数据库
    //connect_mysql();

    //创建一个QSerialPort串口对象
    m_serialPort = new QSerialPort();

    receivecoutner = 0;

    //绘制图表
    line1.setName("current");
    line2.setName("entrance");
    line3.setName("exit");
    line4.setName("flow");
    c.createDefaultAxes();
    c.setTitle("检测结果");
    ui->chartwidget->setRenderHint(QPainter::Antialiasing);
    ui->chartwidget->setChart(&c);


    //进度条
    ui->progressBar->setRange(0,100);
    ui->progressBar->reset();

    //连接信号和槽
    connect(ui->btn_open,SIGNAL(clicked()),this,SLOT(openPort()));
    connect(ui->btn_send,SIGNAL(clicked()),this,SLOT(send()));
    connect(m_serialPort, &QSerialPort::readyRead, this, &MainWindow::read);
    connect(ui->btn_refresh,SIGNAL(clicked()),this,SLOT(refresh()));

}

MainWindow::~MainWindow()
{
    disconnect(m_serialPort,SIGNAL(readyRead()),this,SLOT(read()));
    if (m_serialPort->isOpen())
        {
            m_serialPort->close();
        }
        delete m_serialPort;



    delete ui;
}

//获取可用串口名
QStringList MainWindow::getPortNameList()
{
    QStringList m_serialPortName;
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        m_serialPortName << info.portName();
    }
    return m_serialPortName;
}

void MainWindow::refresh()
{
    ui->comboBox_2->clear();
    m_portNameList = getPortNameList();//获取可用串口名
    ui->comboBox->addItems(m_portNameList);//将可用串口添加到comboBox中
}


//打开和关闭串口
void MainWindow::openPort()
{


    if(ui->btn_open->text()=="打开串口")//如果串口未打开，则打开串口
    {
        //设置串口连接参数
        m_serialPort->setPortName(ui->comboBox->currentText());//当前选择的串口名字
        m_serialPort->setBaudRate(QSerialPort::Baud9600);//设置波特率和读写方向
        m_serialPort->setDataBits(QSerialPort::Data8);      //数据位为8位
        m_serialPort->setFlowControl(QSerialPort::NoFlowControl);//无流控制
        m_serialPort->setParity(QSerialPort::NoParity); //无校验位
        m_serialPort->setStopBits(QSerialPort::OneStop); //一位停止位
        //m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
        //m_serialPort->setReadBufferSize(1024);

        if(m_serialPort->isOpen())//如果串口已经打开了 先给他关闭了
        {
            m_serialPort->clear();
            m_serialPort->close();
        }



        if(m_serialPort->open(QIODevice::ReadWrite))//用ReadWrite 的模式尝试打开串口
        {
            m_serialPort->setDataTerminalReady(true);
            ui->btn_open->setText("关闭串口");
            return;
        }
        else
        {
            QMessageBox::critical(this, tr("Error"), m_serialPort->errorString());

        }



        //ui->statusBar->

    }
    if(ui->btn_open->text()=="关闭串口")//如果串口处于打开状态，再点击则为关闭串口
    {
        if (m_serialPort->isOpen())
            {
                m_serialPort->close();
            }
        ui->btn_open->setText("打开串口");

    }
}


//接收到单片机发送的数据进行解析
void MainWindow::read()
{
    //m_serialPort->waitForReadyRead(10);
    QByteArray buf = m_serialPort->readAll();
    //ui->edit_receive->setText(buf);
    qDebug()<<buf;
    if(buf[0]=='K')
    {
        dataarray.clear();        //清空存放数据的数组
        ui->progressBar->reset(); //复位进度条
        ui->test_status->setText("测试开始");
        try
        {                      //    首先删除图表中上次添加的折线。程序初次启动时图表中没有折线，
            c.removeSeries(&line1);//因此使用try...catch...避免报错
            c.removeSeries(&line2);
            c.removeSeries(&line3);
            c.removeSeries(&line4);
        } catch (...)
        {
        }
        line1.clear(); //首先将上次的数据清空
        line2.clear();
        line3.clear();
        line4.clear();
        c.createDefaultAxes();
        ui->chartwidget->setRenderHint(QPainter::Antialiasing);
        ui->chartwidget->setChart(&c);

    }
    if(buf[0]=='A')               //下位机发送A后开始1档测试
    {
        ui->progressBar->setValue(10);
        ui->test_status->setText("1档测试进行中...");
    }
    if(buf[0]=='B')               //下位机发送B后开始2档测试
    {
        ui->progressBar->setValue(40);
        ui->test_status->setText("2档测试进行中...");
    }
    if(buf[0]=='C')               //下位机发送C后开始3档测试
    {
        ui->progressBar->setValue(70);
        ui->test_status->setText("3档测试进行中...");
    }
    if(buf.contains('T'))                 //如果收到T，开始保存数据
    {
        //m_serialPort->setDataTerminalReady(true);
        receivecoutner = 1;
    }
    if(receivecoutner == 1 and !buf.isNull())
    {
        dataarray.append(buf);

    }
    if(buf.contains('W'))                 //如果收到W，表示数据已经发送完成
    {
        receivecoutner = 0;
        //dataarray.append(buf);
        ui->edit_send->setText(dataarray);
        qDebug()<<dataarray;
    }
    if(dataarray.length()==74)
    {
        //ui->edit_receive->setText("ok");
        ui->progressBar->setValue(100);
        ui->test_status->setText("测试完成!");
        //分离接收到的长数组中的数据
        for (int i=0;i<6;i++)
        {
            result1.currentstr[i] = dataarray[i+1];
            result1.entrancestr[i] = dataarray[i+7];
            result1.exitstr[i] = dataarray[i+13];
            result1.flowstr[i] = dataarray[i+19];
            result2.currentstr[i] = dataarray[i+25];
            result2.entrancestr[i] = dataarray[i+31];
            result2.exitstr[i] = dataarray[i+37];
            result2.flowstr[i] = dataarray[i+43];
            result3.currentstr[i] = dataarray[i+49];
            result3.entrancestr[i] = dataarray[i+55];
            result3.exitstr[i] = dataarray[i+61];
            result3.flowstr[i] = dataarray[i+67];
        }
        //将收到的字符串数组转化为float型
        result1.current = result1.currentstr.toFloat();
        result1.entrance = result1.entrancestr.toFloat();
        result1.exit = result1.exitstr.toFloat();
        result1.flow = result1.flowstr.toFloat();
        result2.current = result2.currentstr.toFloat();
        result2.entrance = result2.entrancestr.toFloat();
        result2.exit = result2.exitstr.toFloat();
        result2.flow = result2.flowstr.toFloat();
        result3.current = result3.currentstr.toFloat();
        result3.entrance = result3.entrancestr.toFloat();
        result3.exit = result3.exitstr.toFloat();
        result3.flow = result3.flowstr.toFloat();
        //qDebug()<<"%f"<<result3.current;
        qDebug()<<result1.current<<"\t"<<result2.current<<"\t"<<result3.current;

        /*try {                      //    首先删除图表中上次添加的折线。程序初次启动时图表中没有折线，
            c.removeSeries(&line1);//因此使用try...catch...避免报错
            c.removeSeries(&line2);
            c.removeSeries(&line3);
            c.removeSeries(&line4);
        } catch (...)
        {

        }*/
        line1.clear(); //首先将上次的数据清空
        line2.clear();
        line3.clear();
        line4.clear();

        //将坐标点数据添加到折线中
        line1.append(1.0,static_cast<double>(result1.current));//    QLineSeries的append方法中参数为double型，
        line1.append(2.0,static_cast<double>(result2.current));//因此进行强制类型转换。并且由float型强制转换为double型
        line1.append(3.0,static_cast<double>(result3.current));//时不会丢失精度。
        line2.append(1,static_cast<double>(result1.entrance));
        line2.append(2,static_cast<double>(result2.entrance));
        line2.append(3,static_cast<double>(result3.entrance));
        line3.append(1,static_cast<double>(result1.exit));
        line3.append(2,static_cast<double>(result2.exit));
        line3.append(3,static_cast<double>(result3.exit));
        line4.append(1,static_cast<double>(result1.flow));
        line4.append(2,static_cast<double>(result2.flow));
        line4.append(3,static_cast<double>(result3.flow));

        c.addSeries(&line1);
        c.addSeries(&line2);
        c.addSeries(&line3);
        c.addSeries(&line4);

        c.createDefaultAxes();
        ui->chartwidget->setRenderHint(QPainter::Antialiasing);
        ui->chartwidget->setChart(&c);

    }

    //m_serialPort->setDataTerminalReady(true);
}

//串口发送数据函数
void MainWindow::send()
{
    sendbuff = ui->edit_send->toPlainText().toUtf8();
    m_serialPort->write(sendbuff);

}



//连接数据库
void MainWindow::connect_mysql()
{
    QSqlDatabase db=QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("192.168.50.169");      //连接数据库主机名，这里需要注意（若填的为”127.0.0.1“，出现不能连接，则改为localhost)
    db.setPort(3306);                 //连接数据库端口号，与设置一致
    db.setDatabaseName("acme");      //连接数据库名，与设置一致
    db.setUserName("qice");          //数据库用户名，与设置一致
    db.setPassword("qice");    //数据库密码，与设置一致

    //尝试连接数据库
    try
    {
        db.open();
    }
    catch (...)
    {
        qDebug()<<"不能连接"<<"connect to mysql error"<<db.lastError().text();
    }

    if(!db.open())
    {
        qDebug()<<"不能连接"<<"connect to mysql error"<<db.lastError().text();
        return ;
    }
    else
    {
        qDebug()<<"连接成功"<<"connect to mysql OK";
    }
    QSqlQuery query(db);
    query.exec("USE acme;");
    query.exec("SELECT * FROM acme.总成气测;");
    while(query.next()){
        qDebug()<<query.value("name").toString();
    }
}
