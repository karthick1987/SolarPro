#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "graphwidget.h"
#include "edge.h"
#include <QDebug>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    m_record = false;
    ui->setupUi(this);

    //Get available COM Ports
    this->uart = new Uart(this);
    QList<QextPortInfo> ports = uart->getUSBPorts();
    for (int i = 0; i < ports.size(); i++) {
        ui->comboBox_Interface->addItem(ports.at(i).portName.toLocal8Bit().constData());
    }
    QObject::connect(uart, SIGNAL(debugReceived(QString)), this, SLOT(receive(QString)));
    QObject::connect(uart, SIGNAL(packetReceived(QByteArray)), this, SLOT(packet_received(QByteArray)));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),this,SLOT(systemTime()));
    timer->start(1000);
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::systemTime()
{
   QTime time = QTime::currentTime();
   static int i = 0;
   QString time_text = time.toString("hh : mm : ss");
   ui->label_systemTime->setText(time_text);
   i++;
   if(i == 1)
   {
        ui->NetworkGraph->addNodes();
        ui->NetworkGraph->scene()->addItem(new Edge(ui->NetworkGraph->centerNode, ui->NetworkGraph->n[1]));
   }
   if(i == 5)
   {
        ui->NetworkGraph->scene()->clear();
   }
   if(i == 7)
   {
        ui->NetworkGraph->addNodes();
        ui->NetworkGraph->scene()->addItem(new Edge(ui->NetworkGraph->centerNode, ui->NetworkGraph->n[2]));
   }
}

void MainWindow::changeEvent(QEvent *e) {
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::on_pushButton_open_clicked() {
    QString portname = "/dev/";
    portname.append(ui->comboBox_Interface->currentText());
    uart->open(portname);
    if (!uart->isOpen())
    {
        error.setText("Unable to open port!");
        error.show();
        return;
    }


    ui->pushButton_close->setEnabled(true);
    ui->pushButton_open->setEnabled(false);
    ui->comboBox_Interface->setEnabled(false);
}

void MainWindow::on_pushButton_close_clicked() {
    if (uart->isOpen()) uart->close();
    ui->pushButton_close->setEnabled(false);
    ui->pushButton_open->setEnabled(true);
    ui->comboBox_Interface->setEnabled(true);
}

void MainWindow::receive(QString str) {
    ui->textEdit_Status->append(str);
    ui->textEdit_Status->ensureCursorVisible();
}

void MainWindow::send(QByteArray data) {
    uart->send(data);
}

void MainWindow::packet_received(QByteArray str) {
    if (str.length() == 0) return;

    switch (str.at(0)) {
    case SERIAL_PACKET_TYPE_ANEMOMETER:
        Anemometer anemometer;
        anemometer.windspeed = str.at(1);
        anemometer.windspeedAvg = str.at(2);
        anemometer.windspeedMax = str.at(3);
        anemometer.windspeedThreshold = str.at(4);


        ui->lcdWindSpeed->setPalette(Qt::black);
        ui->lcdWindSpeed->display(anemometer.windspeed);

        ui->lcdWindSpeed_Avg->setPalette(Qt::black);
        ui->lcdWindSpeed_Avg->display(anemometer.windspeedAvg);

        ui->lcdWindSpeed_Max->setPalette(Qt::black);
        ui->lcdWindSpeed_Max->display(anemometer.windspeedMax);

        ui->lcdWindSpeed_Threshold->setPalette(Qt::black);
        ui->lcdWindSpeed_Threshold->display(anemometer.windspeedThreshold);

        break;

    case SERIAL_PACKET_TYPE_NODE_SENSORS:
        SensorValue sensorvalues[AMOUNT_OF_MOTES];
        int index = str.at(1);
        sensorvalues[index].destNode = str.at(1);
        sensorvalues[index].originNode = str.at(2);
        sensorvalues[index].temp_mC = str.at(3);
        sensorvalues[index].battVolt_mV = str.at(4);
        sensorvalues[index].lightSensor = str.at(5);
        sensorvalues[index].servoPos_degs = str.at(6);

        ui->lcdNumber_NodeID->setPalette(Qt::black);
        ui->lcdNumber_NodeID->display(sensorvalues[index].destNode);

        ui->lcdNumber_Temperature->setPalette(Qt::black);
        ui->lcdNumber_Temperature->display(sensorvalues[index].temp_mC);

        ui->lcdNumber_Voltage->setPalette(Qt::black);
        ui->lcdNumber_Voltage->display(sensorvalues[index].battVolt_mV);

        ui->lcdNumber_Luminosity->setPalette(Qt::black);
        ui->lcdNumber_Luminosity->display(sensorvalues[index].lightSensor);

        ui->lcdNumber_Angle->setPalette(Qt::black);
        ui->lcdNumber_Angle->display(sensorvalues[index].servoPos_degs);

        break;
    }
}

void MainWindow::on_button_NetDiscover_clicked(){
    qDebug() << "NetDiscover clicked";
    QByteArray data = QByteArray((int) 1, (char) 0);
    data[0] = SERIAL_PACKET_TYPE_HOP_HISTORY;
    this->send(data);
}

void MainWindow::on_button_Emergency_clicked(){
    qDebug() << "Emergency clicked";
    QByteArray data = QByteArray((int) 1, (char) 0);
    data[0] = SERIAL_PACKET_TYPE_EMERGENCY;
    this->send(data);
}

void MainWindow::on_pushButton_SetWindSpeedThreshold_clicked(){
    qDebug() << "Threshold clicked";
    qDebug() << (signed char) ui->spinBox_WindSpeedThreshold->value();
    QByteArray data = QByteArray((int) 2, (char) 0);
    data[0] = SERIAL_PACKET_TYPE_SET_WIND_SPEED_THRS;
    data[1] = (signed char) ui->spinBox_WindSpeedThreshold->value();
    this->send(data);
}

void MainWindow::on_pushButton_SetManualAngle_clicked(){
    qDebug() << "Set Manual Angle clicked";
    QByteArray data = QByteArray((int) 2, (char) 0);
    data[0] = SERIAL_PACKET_TYPE_SERVO_MANUAL;
    data[1] = (signed char) ui->spinBox_ManualAngle->value();
    this->send(data);
}

void MainWindow::on_radioButton_AngleAuto_clicked(){
    ui->pushButton_SetManualAngle->setEnabled(false);
}

void MainWindow::on_radioButton_AngleManual_clicked(){
    ui->pushButton_SetManualAngle->setEnabled(true);
}





