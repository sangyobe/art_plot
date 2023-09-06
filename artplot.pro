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

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../usr/local/lib/release/ -lprotobuf
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../usr/local/lib/debug/ -lprotobuf
else:unix: LIBS += -L$$PWD/../../../../../../usr/local/lib/ -lprotobuf

INCLUDEPATH += $$PWD/../../../../../../usr/local/include
DEPENDPATH += $$PWD/../../../../../../usr/local/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../usr/local/lib/release/libprotobuf.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../usr/local/lib/debug/libprotobuf.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../usr/local/lib/release/protobuf.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../usr/local/lib/debug/protobuf.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../../../../../usr/local/lib/libprotobuf.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../usr/local/lib/release/ -ldtproto
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../usr/local/lib/debug/ -ldtproto
else:unix: LIBS += -L$$PWD/../../../../../../usr/local/lib/ -ldtproto

INCLUDEPATH += $$PWD/../../../../../../usr/local/include
DEPENDPATH += $$PWD/../../../../../../usr/local/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../usr/local/lib/release/libdtproto.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../usr/local/lib/debug/libdtproto.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../usr/local/lib/release/dtproto.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../usr/local/lib/debug/dtproto.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../../../../../usr/local/lib/libdtproto.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../usr/local/lib/release/ -lecal_core
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../usr/local/lib/debug/ -lecal_core
else:unix: LIBS += -L$$PWD/../../../../../../usr/local/lib/ -lecal_core

INCLUDEPATH += $$PWD/../../../../../../usr/local/include
DEPENDPATH += $$PWD/../../../../../../usr/local/include
