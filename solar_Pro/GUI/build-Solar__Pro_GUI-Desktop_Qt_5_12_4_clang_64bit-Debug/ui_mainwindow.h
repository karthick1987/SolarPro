/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
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
    QLabel *label_WindSpeed_2;
    QLCDNumber *lcdWindSpeed_2;
    QLabel *label_WindSpeed_3;
    QLCDNumber *lcdWindSpeed_3;
    QGroupBox *groupBox_2;
    QGraphicsView *graphicsView;
    QWidget *tab_2;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(686, 564);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(0, 0, 681, 541));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        button_NetDiscover = new QPushButton(tab);
        button_NetDiscover->setObjectName(QString::fromUtf8("button_NetDiscover"));
        button_NetDiscover->setGeometry(QRect(10, 470, 141, 32));
        button_Emergency = new QPushButton(tab);
        button_Emergency->setObjectName(QString::fromUtf8("button_Emergency"));
        button_Emergency->setGeometry(QRect(150, 470, 112, 32));
        label_NetGraph = new QLabel(tab);
        label_NetGraph->setObjectName(QString::fromUtf8("label_NetGraph"));
        label_NetGraph->setGeometry(QRect(10, 0, 101, 16));
        groupBox = new QGroupBox(tab);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(430, 10, 231, 241));
        lcdWindSpeed = new QLCDNumber(groupBox);
        lcdWindSpeed->setObjectName(QString::fromUtf8("lcdWindSpeed"));
        lcdWindSpeed->setGeometry(QRect(150, 30, 64, 23));
        label_WindSpeed = new QLabel(groupBox);
        label_WindSpeed->setObjectName(QString::fromUtf8("label_WindSpeed"));
        label_WindSpeed->setGeometry(QRect(10, 30, 121, 16));
        label_WindSpeed_2 = new QLabel(groupBox);
        label_WindSpeed_2->setObjectName(QString::fromUtf8("label_WindSpeed_2"));
        label_WindSpeed_2->setGeometry(QRect(10, 70, 131, 16));
        lcdWindSpeed_2 = new QLCDNumber(groupBox);
        lcdWindSpeed_2->setObjectName(QString::fromUtf8("lcdWindSpeed_2"));
        lcdWindSpeed_2->setGeometry(QRect(150, 70, 64, 23));
        label_WindSpeed_3 = new QLabel(groupBox);
        label_WindSpeed_3->setObjectName(QString::fromUtf8("label_WindSpeed_3"));
        label_WindSpeed_3->setGeometry(QRect(10, 110, 131, 16));
        lcdWindSpeed_3 = new QLCDNumber(groupBox);
        lcdWindSpeed_3->setObjectName(QString::fromUtf8("lcdWindSpeed_3"));
        lcdWindSpeed_3->setGeometry(QRect(150, 110, 64, 23));
        groupBox_2 = new QGroupBox(tab);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(430, 260, 231, 231));
        graphicsView = new QGraphicsView(tab);
        graphicsView->setObjectName(QString::fromUtf8("graphicsView"));
        graphicsView->setGeometry(QRect(20, 30, 391, 431));
        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        tabWidget->addTab(tab_2, QString());
        MainWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Solar Pro", nullptr));
        button_NetDiscover->setText(QApplication::translate("MainWindow", "Network Discovery", nullptr));
        button_Emergency->setText(QApplication::translate("MainWindow", "Emergency", nullptr));
        label_NetGraph->setText(QApplication::translate("MainWindow", "Network Graph", nullptr));
        groupBox->setTitle(QApplication::translate("MainWindow", "Wind Speed", nullptr));
        label_WindSpeed->setText(QApplication::translate("MainWindow", "Actual Wind Speed", nullptr));
        label_WindSpeed_2->setText(QApplication::translate("MainWindow", "Average Wind Speed", nullptr));
        label_WindSpeed_3->setText(QApplication::translate("MainWindow", "Max Wind Speed", nullptr));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "Node Info", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainWindow", "General", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainWindow", "Connections", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
