/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDial>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QTabWidget *tabWidget;
    QWidget *tab;
    QPushButton *button_NetDiscover;
    QPushButton *button_Emergency;
    QLabel *label_NetGraph;
    QGroupBox *groupBox;
    QLCDNumber *lcdWindSpeed;
    QLabel *label_WindSpeed;
    QLabel *label_WindSpeed_Avg;
    QLCDNumber *lcdWindSpeed_Avg;
    QLabel *label_WindSpeed_Max;
    QLCDNumber *lcdWindSpeed_Max;
    QLabel *label_EmergencyThreshold;
    QPushButton *pushButton_SetWindSpeedThreshold;
    QLCDNumber *lcdWindSpeed_Threshold;
    QDoubleSpinBox *doubleSpinBox;
    QGroupBox *groupBox_2;
    QLabel *label_NodeID;
    QLabel *label_Temperature;
    QLabel *label_Voltage;
    QLabel *label_Light;
    QLabel *label_Angle;
    QDial *dial_SetManualAngle;
    QRadioButton *radioButton_AngleAuto;
    QRadioButton *radioButton_AngleManual;
    QLCDNumber *lcdNumber_NodeID;
    QLCDNumber *lcdNumber_Temperature;
    QLCDNumber *lcdNumber_Voltage;
    QLCDNumber *lcdNumber_Luminosity;
    QLCDNumber *lcdNumber_Angle;
    QGraphicsView *graphicsView;
    QWidget *tab_2;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(686, 564);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setGeometry(QRect(0, 0, 681, 541));
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        button_NetDiscover = new QPushButton(tab);
        button_NetDiscover->setObjectName(QStringLiteral("button_NetDiscover"));
        button_NetDiscover->setGeometry(QRect(10, 470, 141, 32));
        button_Emergency = new QPushButton(tab);
        button_Emergency->setObjectName(QStringLiteral("button_Emergency"));
        button_Emergency->setGeometry(QRect(150, 470, 112, 32));
        label_NetGraph = new QLabel(tab);
        label_NetGraph->setObjectName(QStringLiteral("label_NetGraph"));
        label_NetGraph->setGeometry(QRect(160, 10, 101, 16));
        groupBox = new QGroupBox(tab);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(430, 10, 231, 241));
        lcdWindSpeed = new QLCDNumber(groupBox);
        lcdWindSpeed->setObjectName(QStringLiteral("lcdWindSpeed"));
        lcdWindSpeed->setGeometry(QRect(160, 30, 64, 23));
        label_WindSpeed = new QLabel(groupBox);
        label_WindSpeed->setObjectName(QStringLiteral("label_WindSpeed"));
        label_WindSpeed->setGeometry(QRect(10, 30, 121, 16));
        label_WindSpeed_Avg = new QLabel(groupBox);
        label_WindSpeed_Avg->setObjectName(QStringLiteral("label_WindSpeed_Avg"));
        label_WindSpeed_Avg->setGeometry(QRect(10, 70, 131, 16));
        lcdWindSpeed_Avg = new QLCDNumber(groupBox);
        lcdWindSpeed_Avg->setObjectName(QStringLiteral("lcdWindSpeed_Avg"));
        lcdWindSpeed_Avg->setGeometry(QRect(160, 70, 64, 23));
        label_WindSpeed_Max = new QLabel(groupBox);
        label_WindSpeed_Max->setObjectName(QStringLiteral("label_WindSpeed_Max"));
        label_WindSpeed_Max->setGeometry(QRect(10, 110, 131, 16));
        lcdWindSpeed_Max = new QLCDNumber(groupBox);
        lcdWindSpeed_Max->setObjectName(QStringLiteral("lcdWindSpeed_Max"));
        lcdWindSpeed_Max->setGeometry(QRect(160, 110, 64, 23));
        label_EmergencyThreshold = new QLabel(groupBox);
        label_EmergencyThreshold->setObjectName(QStringLiteral("label_EmergencyThreshold"));
        label_EmergencyThreshold->setGeometry(QRect(10, 160, 141, 16));
        pushButton_SetWindSpeedThreshold = new QPushButton(groupBox);
        pushButton_SetWindSpeedThreshold->setObjectName(QStringLiteral("pushButton_SetWindSpeedThreshold"));
        pushButton_SetWindSpeedThreshold->setGeometry(QRect(90, 190, 61, 32));
        lcdWindSpeed_Threshold = new QLCDNumber(groupBox);
        lcdWindSpeed_Threshold->setObjectName(QStringLiteral("lcdWindSpeed_Threshold"));
        lcdWindSpeed_Threshold->setGeometry(QRect(160, 160, 64, 23));
        doubleSpinBox = new QDoubleSpinBox(groupBox);
        doubleSpinBox->setObjectName(QStringLiteral("doubleSpinBox"));
        doubleSpinBox->setGeometry(QRect(20, 190, 62, 31));
        groupBox_2 = new QGroupBox(tab);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(430, 260, 231, 241));
        label_NodeID = new QLabel(groupBox_2);
        label_NodeID->setObjectName(QStringLiteral("label_NodeID"));
        label_NodeID->setGeometry(QRect(10, 30, 58, 16));
        label_Temperature = new QLabel(groupBox_2);
        label_Temperature->setObjectName(QStringLiteral("label_Temperature"));
        label_Temperature->setGeometry(QRect(10, 60, 81, 16));
        label_Voltage = new QLabel(groupBox_2);
        label_Voltage->setObjectName(QStringLiteral("label_Voltage"));
        label_Voltage->setGeometry(QRect(10, 90, 81, 16));
        label_Light = new QLabel(groupBox_2);
        label_Light->setObjectName(QStringLiteral("label_Light"));
        label_Light->setGeometry(QRect(10, 120, 81, 16));
        label_Angle = new QLabel(groupBox_2);
        label_Angle->setObjectName(QStringLiteral("label_Angle"));
        label_Angle->setGeometry(QRect(10, 150, 81, 16));
        dial_SetManualAngle = new QDial(groupBox_2);
        dial_SetManualAngle->setObjectName(QStringLiteral("dial_SetManualAngle"));
        dial_SetManualAngle->setEnabled(false);
        dial_SetManualAngle->setGeometry(QRect(100, 170, 61, 61));
        dial_SetManualAngle->setMaximum(180);
        dial_SetManualAngle->setSingleStep(5);
        radioButton_AngleAuto = new QRadioButton(groupBox_2);
        radioButton_AngleAuto->setObjectName(QStringLiteral("radioButton_AngleAuto"));
        radioButton_AngleAuto->setGeometry(QRect(10, 180, 100, 20));
        radioButton_AngleAuto->setChecked(true);
        radioButton_AngleManual = new QRadioButton(groupBox_2);
        radioButton_AngleManual->setObjectName(QStringLiteral("radioButton_AngleManual"));
        radioButton_AngleManual->setGeometry(QRect(10, 200, 100, 20));
        lcdNumber_NodeID = new QLCDNumber(groupBox_2);
        lcdNumber_NodeID->setObjectName(QStringLiteral("lcdNumber_NodeID"));
        lcdNumber_NodeID->setGeometry(QRect(160, 30, 64, 23));
        lcdNumber_Temperature = new QLCDNumber(groupBox_2);
        lcdNumber_Temperature->setObjectName(QStringLiteral("lcdNumber_Temperature"));
        lcdNumber_Temperature->setGeometry(QRect(160, 60, 64, 23));
        lcdNumber_Voltage = new QLCDNumber(groupBox_2);
        lcdNumber_Voltage->setObjectName(QStringLiteral("lcdNumber_Voltage"));
        lcdNumber_Voltage->setGeometry(QRect(160, 90, 64, 23));
        lcdNumber_Luminosity = new QLCDNumber(groupBox_2);
        lcdNumber_Luminosity->setObjectName(QStringLiteral("lcdNumber_Luminosity"));
        lcdNumber_Luminosity->setGeometry(QRect(160, 120, 64, 23));
        lcdNumber_Angle = new QLCDNumber(groupBox_2);
        lcdNumber_Angle->setObjectName(QStringLiteral("lcdNumber_Angle"));
        lcdNumber_Angle->setGeometry(QRect(160, 150, 64, 23));
        graphicsView = new QGraphicsView(tab);
        graphicsView->setObjectName(QStringLiteral("graphicsView"));
        graphicsView->setGeometry(QRect(20, 30, 391, 431));
        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        tabWidget->addTab(tab_2, QString());
        MainWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Solar Pro", Q_NULLPTR));
        button_NetDiscover->setText(QApplication::translate("MainWindow", "Network Discovery", Q_NULLPTR));
        button_Emergency->setText(QApplication::translate("MainWindow", "Emergency", Q_NULLPTR));
        label_NetGraph->setText(QApplication::translate("MainWindow", "Network Graph", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("MainWindow", "Wind Speed", Q_NULLPTR));
        label_WindSpeed->setText(QApplication::translate("MainWindow", "Actual Wind Speed", Q_NULLPTR));
        label_WindSpeed_Avg->setText(QApplication::translate("MainWindow", "Average Wind Speed", Q_NULLPTR));
        label_WindSpeed_Max->setText(QApplication::translate("MainWindow", "Max Wind Speed", Q_NULLPTR));
        label_EmergencyThreshold->setText(QApplication::translate("MainWindow", "Emeregency Threshold", Q_NULLPTR));
        pushButton_SetWindSpeedThreshold->setText(QApplication::translate("MainWindow", "Set", Q_NULLPTR));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "Node Info", Q_NULLPTR));
        label_NodeID->setText(QApplication::translate("MainWindow", "Node ID", Q_NULLPTR));
        label_Temperature->setText(QApplication::translate("MainWindow", "Temperature", Q_NULLPTR));
        label_Voltage->setText(QApplication::translate("MainWindow", "Voltage", Q_NULLPTR));
        label_Light->setText(QApplication::translate("MainWindow", "Luminosity", Q_NULLPTR));
        label_Angle->setText(QApplication::translate("MainWindow", "Panel Angle", Q_NULLPTR));
        radioButton_AngleAuto->setText(QApplication::translate("MainWindow", "Automatic", Q_NULLPTR));
        radioButton_AngleManual->setText(QApplication::translate("MainWindow", "&Manual", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainWindow", "General", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainWindow", "Connections", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
