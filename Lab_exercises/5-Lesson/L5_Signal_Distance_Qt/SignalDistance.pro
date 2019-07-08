#-------------------------------------------------
#
# Project created by QtCreator 2014-09-16T15:43:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SignalDistance
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp\
        uart.cpp

HEADERS  += mainwindow.h\
            uart.h

FORMS    += mainwindow.ui

#-------------------------------------------------
# This section will include QextSerialPort in
# your project:

HOMEDIR = $$(HOME)
include($$HOMEDIR/group1/solar_Pro/GUI/qextserialport/src/qextserialport.pri)

CONFIG += extserialport

# Before running the project, run qmake first:
# In Qt Creator, right-click on the project
# and choose "run qmake".
# The qextserialport folder should appear in the
# directory tree.
# Now your project is ready to run.
#-------------------------------------------------
