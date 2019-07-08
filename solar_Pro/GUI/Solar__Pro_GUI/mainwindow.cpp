#include "mainwindow.h"
#include "ui_mainwindow.h"

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
}

MainWindow::~MainWindow()
{
    delete ui;
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

        /*int row = ui->tableWidget->rowCount();
        row++;
        ui->tableWidget->setRowCount(row);
        ui->tableWidget->scrollToBottom();
        QTableWidgetItem *index = new QTableWidgetItem(QString::number(radioTest.number));
        ui->tableWidget->setItem(row-1, 0, index);
        QTableWidgetItem *distance = new QTableWidgetItem(QString::number(ui->doubleSpinBox_distance->value())); //tr("%1 %2").arg(row).arg(str)
        ui->tableWidget->setItem(row-1, 1, distance);
        QTableWidgetItem *TxPower = new QTableWidgetItem(QString::number(radioTest.tx_power));
        ui->tableWidget->setItem(row-1, 2, TxPower);
        QTableWidgetItem *RxPower = new QTableWidgetItem(QString::number(radioTest.rssi));
        ui->tableWidget->setItem(row-1, 3, RxPower);*/

        break;
    case SERIAL_PACKET_TYPE_NODE_SENSORS:
        SensorValue sensorvalue;
        sensorvalue.destNode = str.at(1);
        sensorvalue.originNode = str.at(2);
        sensorvalue.temp_mC = str.at(3);
        sensorvalue.battVolt_mV = str.at(4);
        sensorvalue.lightSensor = str.at(5);
        sensorvalue.servoPos_degs = str.at(6);

        break;
    }
}

void MainWindow::on_button_NetDiscover_clicked(){

}

void MainWindow::on_button_Emergency_clicked(){

}

void MainWindow::on_pushButton_SetWindSpeedThreshold_clicked(){

}

void MainWindow::on_dial_SetManualAngle_clicked(){

}

void MainWindow::on_radioButton_AngleAuto_clicked(){
    ui->dial_SetManualAngle->setEnabled(false);
}

void MainWindow::on_radioButton_AngleManual_clicked(){
    ui->dial_SetManualAngle->setEnabled(true);
}





