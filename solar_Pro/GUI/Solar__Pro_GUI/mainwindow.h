#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QClipboard>
#include "uart.h"

// Packet type flags used in UART/serial transmission.
#define SERIAL_PACKET_TYPE_ANEMOMETER         	0
#define SERIAL_PACKET_TYPE_NODE_SENSORS       	1
#define SERIAL_PACKET_TYPE_NETWORK_DISCOVERY	2
#define SERIAL_PACKET_TYPE_EMERGENCY			3
#define SERIAL_PACKET_TYPE_SERVO_MANUAL			4
#define SERIAL_PACKET_TYPE_SET_WIND_SPEED_THRS  5

namespace Ui {
class MainWindow;
}

typedef struct {
    unsigned char destNode, originNode;
    signed char temp_mC;
    unsigned char battVolt_mV;
    unsigned char lightSensor;
    unsigned char servoPos_degs;
} SensorValue;

typedef struct {
    unsigned char windspeed;
    unsigned char windspeedAvg;
    unsigned char windspeedMax;
    unsigned char windspeedThreshold;
}Anemometer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    QMessageBox error;
    bool m_record;
    Uart *uart;


private slots:
    void on_pushButton_close_clicked();
    void on_pushButton_open_clicked();
    void on_button_NetDiscover_clicked();
    void on_button_Emergency_clicked();
    void on_pushButton_SetWindSpeedThreshold_clicked();
    void on_pushButton_SetManualAngle_clicked();
    void receive(QString str);
    void send(QByteArray data);
    void packet_received(QByteArray str);
    void on_radioButton_AngleAuto_clicked();
    void on_radioButton_AngleManual_clicked();
};


#endif // MAINWINDOW_H
