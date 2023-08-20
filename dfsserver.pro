QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    common/common.cpp \
    common/des.cpp \
    common/downloadlayout.cpp \
    common/logininfoinstance.cpp \
    common/uploadtask.cpp \
    login.cpp \
    main.cpp \
    mainwindow.cpp \
    mytitlebar.cpp

HEADERS += \
    common/common.h \
    common/des.h \
    common/downloadlayout.h \
    common/logininfoinstance.h \
    common/uploadtask.h \
    login.h \
    mainwindow.h \
    mytitlebar.h

FORMS += \
    login.ui \
    mainwindow.ui \
    mytitlebar.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images/res.qrc
