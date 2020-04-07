#-------------------------------------------------
#
# Project created by QtCreator 2017-04-04T15:11:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CubInt
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp drawarea.cpp\
        R2Graph.cpp ../cubint.cpp ../bandmatrix.cpp

HEADERS  += mainwindow.h drawarea.h R2Graph.h ../cubint.h ../bandmatrix.h

FORMS    += mainwindow.ui

CONFIG(release, debug|release): DEFINES += NDEBUG
