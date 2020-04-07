#-------------------------------------------------
#
# Project created by QtCreator 2017-03-21T15:15:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ImView
TEMPLATE = app


SOURCES += main.cpp \
        mainwindow.cpp drawarea.cpp RealPixel.cpp \
        CubicInterpol/cubint.cpp CubicInterpol/bandmatrix.cpp

HEADERS  += mainwindow.h drawarea.h RealPixel.h \
        CubicInterpol/cubint.h CubicInterpol/bandmatrix.h CubicInterpol/R2Graph.h

FORMS    += mainwindow.ui

CONFIG(release, debug|release): DEFINES += NDEBUG
