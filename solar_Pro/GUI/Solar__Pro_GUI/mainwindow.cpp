#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "graphwidget.h"
#include "edge.h"
#include "node.h"
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

    ui->checkBox->setCheckState(Qt::CheckState::Checked);
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
    QRadialGradient gradient(-3, -3, 10);
    static int i = 0;
    QString time_text = time.toString("hh : mm : ss");
    ui->label_systemTime->setText(time_text);
    i++;

   //

   if(i == 1)
   {
        ui->NetworkGraph->addNodes();
        //ui->NetworkGraph->scene()->addItem(new Edge(ui->NetworkGraph->n[1], ui->NetworkGraph->n[0]));
   }
   if(i == 3)
   {
        //ui->NetworkGraph->scene()->clear();
        //ui->NetworkGraph->addNodes();
        //ui->NetworkGraph->scene()->addItem(new Edge(ui->NetworkGraph->n[1], ui->NetworkGraph->n[2]));
        //Node *n9 = new Node(ui->NetworkGraph,'9');
        //ui->NetworkGraph->scene()->addItem(n9);
        //n9->setPos(-100,-100);

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
    int indexStaticRolling;
    switch (str.at(0)) {

    case SERIAL_PACKET_TYPE_NETWORK_DISCOVERY:
        if(static_cast<unsigned char> (str.at(1)) == 0xFF && static_cast<unsigned char> (str.at(2)) == 0xFF && static_cast<unsigned char> (str.at(3)) == 0xFF)
        {
            ui->NetworkGraph->scene()->clear();
            ui->NetworkGraph->addNodes();
        }
        else
        {
            int i;
            for (i=1;i<10;i++)
            {
                uint8_t nodeIndex = static_cast<uint8_t>(str.at(i)) - 1;
                if (ui->NetworkGraph->getNodePtr(nodeIndex) == nullptr)
                {
                    ui->NetworkGraph->addNode(nodeIndex);
                }
                if (static_cast<uint8_t>(str.at(i+1)) == 0xFF)
                    break;

                uint8_t node1 = static_cast<uint8_t>(str.at(i+1)) - 1;
                if (ui->NetworkGraph->getNodePtr(node1) == nullptr)
                {
                    ui->NetworkGraph->addNode(node1);
                }

                uint8_t c1 = static_cast<uint8_t>(str.at(i)) - 1;
                uint8_t c2 = static_cast<uint8_t>(str.at(i+1)) - 1;

                ui->NetworkGraph->scene()->addItem(new Edge(ui->NetworkGraph->n[c1], ui->NetworkGraph->n[c2]));
            }
        }
        break;

    case SERIAL_PACKET_TYPE_ANEMOMETER:
        if(str.length()<5)
            break;
        Anemometer anemometer;
        anemometer.windspeed = static_cast<uint8_t>(str.at(1));
        anemometer.windspeedAvg = static_cast<uint8_t>(str.at(2));
        anemometer.windspeedMax = static_cast<uint8_t>(str.at(3));
        anemometer.windspeedThreshold = static_cast<uint8_t>(str.at(4));


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
        int index = str.at(3);
        if (str.length() <= 12)
        {
            break;
        }
        sensorvalues[index].destNode = static_cast<uint16_t>(str.at(3));
        sensorvalues[index].originNode = static_cast<uint16_t>(str.at(1));
        sensorvalues[index].temp_mC = static_cast<int>( (str.at(5)+(256*str.at(6))) );
        sensorvalues[index].battVolt_mV = static_cast<uint16_t>( (str.at(7)+(256*str.at(8))) );

        sensorvalues[index].lightSensor = static_cast<uint16_t> (str.at(9));
        sensorvalues[index].servoPos_degs = static_cast<uint16_t> (str.at(11));

        if (ui->checkBox->isChecked())
        {
            indexStaticRolling = index;
            ui->lcdNumber_NodeID->setPalette(Qt::black);
            ui->lcdNumber_NodeID->display(sensorvalues[indexStaticRolling].destNode);

            ui->lcdNumber_Temperature->setPalette(Qt::black);
            ui->lcdNumber_Temperature->display(sensorvalues[indexStaticRolling].temp_mC);

            ui->lcdNumber_Voltage->setPalette(Qt::black);
            ui->lcdNumber_Voltage->display(sensorvalues[indexStaticRolling].battVolt_mV);

            ui->lcdNumber_Luminosity->setPalette(Qt::black);
            ui->lcdNumber_Luminosity->display(sensorvalues[indexStaticRolling].lightSensor);

            ui->lcdNumber_Angle->setPalette(Qt::black);
            ui->lcdNumber_Angle->display(sensorvalues[indexStaticRolling].servoPos_degs);
        }
        break;
    }
}

void MainWindow::on_button_NetDiscover_clicked(){
    qDebug() << "NetDiscover clicked";
    QByteArray data = QByteArray((int) 1, (char) 0);
    data[0] = SERIAL_PACKET_TYPE_NETWORK_DISCOVERY;
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





