#-------------------------------------------------
#
# Project created by QtCreator 2023-05-20T20:52:19
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = artplot
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        csvfile.cpp \
        datasource.cpp \
        datasourceemul.cpp \
        itemviewdelegate.cpp \
        main.cpp \
        mainwindow.cpp \
        plotcentralwidget.cpp \
        plotconfig.cpp \
        plotwidget.cpp \
        plotwindow.cpp \
        qcustomplot.cpp

HEADERS += \
        csvfile.h \
        datasource.h \
        datasourceemul.h \
        itemviewdelegate.h \
        mainwindow.h \
        pconstants.h \
        plotcentralwidget.h \
        plotconfig.h \
        plotwidget.h \
        plotwindow.h \
        qcustomplot.h

FORMS += \
        mainwindow.ui \
        plotconfig.ui \
        plotwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    artplot.qrc



#win32:INCLUDEPATH += "d:/Projects/include"
unix:INCLUDEPATH += "/usr/local/include"
unix:INCLUDEPATH += "/home/sean/.local/include"

unix:DEPENDPATH += "/usr/local/include"
unix:DEPENDPATH += "/home/sean/.local/include"


unix: LIBS += -L/usr/local/lib/ -lprotobuf
unix: LIBS += -L/usr/local/lib/ -ldtproto
unix: LIBS += -L/usr/local/lib/ -ldtproto_grpc
unix: LIBS += -L/usr/local/lib/ -lecal_core
unix: LIBS += -L/home/sean/.local/lib/ -lgrpc++
unix: LIBS += -L/home/sean/.local/lib/ -lgrpc++_reflection
