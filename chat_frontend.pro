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
    resetdialog.cpp \
    timerbtn.cpp \
    clickedlabel.cpp \
    clickedbtn.cpp \
    customizeedit.cpp \
    chatuserlist.cpp \
    listitembase.cpp \
    chatuserwid.cpp \
    chatdialog.cpp \
    chatpage.cpp \
    loadingdlg.cpp \
    chatview.cpp \
    chatitembase.cpp \
    bubbleframe.cpp \
    textbubble.cpp \
    picturebubble.cpp \
    messagetextedit.cpp \
    global.cpp \
    httpmgr.cpp \
    tcpmgr.cpp \
    usermgr.cpp

HEADERS += \
    mainwindow.h \
    logindialog.h \
    registerdialog.h \
    resetdialog.h \
    timerbtn.h \
    clickedlabel.h \
    clickedbtn.h \
    customizeedit.h \
    chatuserlist.h \
    listitembase.h \
    chatuserwid.h \
    chatdialog.h \
    chatpage.h \
    loadingdlg.h \
    chatview.h \
    chatitembase.h \
    bubbleframe.h \
    textbubble.h \
    picturebubble.h \
    messagetextedit.h \
    global.h \
    httpmgr.h \
    tcpmgr.h \
    usermgr.h \
    singleton.h

FORMS += \
    mainwindow.ui \
    logindialog.ui \
    registerdialog.ui \
    resetdialog.ui \
    chatdialog.ui \
    chatpage.ui \
    loadingdlg.ui \
    chatuserwid.ui

RESOURCES += \
    rc.qrc

DISTFILES += \
    config.ini

win32:RC_ICONS = res/app.ico

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
