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

CONFIG += c++14

SOURCES += \
        src/QuadIP.pb.cc \
        src/csvfile.cpp \
        src/datasource.cpp \
        src/datasourceemul.cpp \
        src/itemviewdelegate.cpp \
        src/main.cpp \
        src/mainwindow.cpp \
        src/plotcentralwidget.cpp \
        src/plotconfig.cpp \
        src/plotwidget.cpp \
        src/plotwindow.cpp \
        src/qcustomplot.cpp

HEADERS += \
        src/QuadIP.pb.h \
        src/csvfile.h \
        src/datasource.h \
        src/datasourceemul.h \
        src/itemviewdelegate.h \
        src/mainwindow.h \
        src/pconstants.h \
        src/plotcentralwidget.h \
        src/plotconfig.h \
        src/plotwidget.h \
        src/plotwindow.h \
        src/qcustomplot.h

FORMS += \
        src/mainwindow.ui \
        src/plotconfig.ui \
        src/plotwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    src/artplot.qrc



#win32:INCLUDEPATH += "d:/Projects/include"
unix:INCLUDEPATH += "/usr/local/include"
unix:INCLUDEPATH += "/home/sean/.local/include"

unix:DEPENDPATH += "/usr/local/include"
unix:DEPENDPATH += "/home/sean/.local/include"


unix: LIBS += -L/usr/local/lib/
unix: LIBS += -L/home/sean/.local/lib/
unix: LIBS += -lprotobuf
unix: LIBS += -lgrpc++
#unix: LIBS += -lgrpc++_reflection
unix: LIBS += -ldtproto
unix: LIBS += -ldtproto_grpc
unix: LIBS += -lecal_core
