QT += core gui widgets network

CONFIG += c++17

TARGET = IM_frontend
TEMPLATE = app

DESTDIR = ./bin

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    logindialog.cpp \
    registerdialog.cpp \
    global.cpp \
    httpmgr.cpp

HEADERS += \
    mainwindow.h \
    logindialog.h \
    registerdialog.h \
    global.h \
    httpmgr.h \
    singleton.h

FORMS += \
    mainwindow.ui \
    logindialog.ui \
    registerdialog.ui

RESOURCES += \
    rc.qrc

DISTFILES += \
    config.ini

# day05: 构建后将 config.ini 拷贝到运行目录 bin
win32:CONFIG(release, debug|release) {
    TargetConfig = $${PWD}/config.ini
    TargetConfig = $$replace(TargetConfig, /, \\)
    OutputDir = $${OUT_PWD}/$${DESTDIR}
    OutputDir = $$replace(OutputDir, /, \\)
    QMAKE_POST_LINK += $$quote(copy /Y \"$$TargetConfig\" \"$$OutputDir\" $$escape_expand(\\n\\t))
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
