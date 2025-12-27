QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bookpanel.cpp \
    borrowpanel.cpp \
    database_manager.cpp \
    file_exporter.cpp \
    main.cpp \
    mainwindow.cpp \
    readerpanel.cpp

HEADERS += \
    bookpanel.h \
    borrowpanel.h \
    database_manager.h \
    file_exporter.h \
    mainwindow.h \
    readerpanel.h

FORMS += \
    bookpanel.ui \
    borrowpanel.ui \
    mainwindow.ui \
    readerpanel.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
