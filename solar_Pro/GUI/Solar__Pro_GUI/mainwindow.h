#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QClipboard>
#include <QGraphicsView>
#include <QTimer>
#include "uart.h"

// Packet type flags used in UART/serial transmission.
#define SERIAL_PACKET_TYPE_ANEMOMETER         	1
#define SERIAL_PACKET_TYPE_NODE_SENSORS       	2
#define SERIAL_PACKET_TYPE_NETWORK_DISCOVERY    3
#define SERIAL_PACKET_TYPE_EMERGENCY			4
#define SERIAL_PACKET_TYPE_SERVO_MANUAL			5
#define SERIAL_PACKET_TYPE_SET_WIND_SPEED_THRS  6

#define AMOUNT_OF_MOTES 20

class Node;

namespace Ui {
class MainWindow;
}

typedef struct {
    uint16_t destNode, originNode;
    int temp_mC;
    uint16_t battVolt_mV;
    unsigned char lightSensor;
    unsigned char servoPos_degs;
} SensorValue;

typedef struct {
    unsigned char windspeed;
    unsigned char windspeedAvg;
    unsigned char windspeedMax;
    unsigned char windspeedThreshold;
}Anemometer;

typedef struct {
    int hopHist[8];
}Network;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    Ui::MainWindow * getUiPtr(void);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);
    //void keyPressEvent(QKeyEvent *event) override;

private:
    Ui::MainWindow *ui;
    QMessageBox error;
    bool m_record;
    Uart *uart;
    Node *centerNode;
    QTimer *timer;

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
    void systemTime();
};


#endif // MAINWINDOW_H
