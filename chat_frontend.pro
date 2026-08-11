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
    clickedoncelabel.cpp \
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
    statewidget.cpp \
    searchlist.cpp \
    adduseritem.cpp \
    findsuccessdlg.cpp \
    findfaildlg.cpp \
    authenfriend.cpp \
    friendinfopage.cpp \
    userinfopage.cpp \
    applyfriend.cpp \
    friendlabel.cpp \
    grouptipitem.cpp \
    conuseritem.cpp \
    contactuserlist.cpp \
    applyfriendlist.cpp \
    applyfrienditem.cpp \
    applyfriendpage.cpp \
    userdata.cpp \
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
    clickedoncelabel.h \
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
    statewidget.h \
    searchlist.h \
    adduseritem.h \
    findsuccessdlg.h \
    findfaildlg.h \
    authenfriend.h \
    friendinfopage.h \
    userinfopage.h \
    applyfriend.h \
    friendlabel.h \
    grouptipitem.h \
    conuseritem.h \
    contactuserlist.h \
    applyfriendlist.h \
    applyfrienditem.h \
    applyfriendpage.h \
    userdata.h \
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
    chatuserwid.ui \
    adduseritem.ui \
    findsuccessdlg.ui \
    findfaildlg.ui \
    authenfriend.ui \
    friendinfopage.ui \
    userinfopage.ui \
    applyfriend.ui \
    friendlabel.ui \
    grouptipitem.ui \
    conuseritem.ui \
    applyfrienditem.ui \
    applyfriendpage.ui

RESOURCES += \
    rc.qrc

DISTFILES += \
    config.ini

win32:RC_ICONS = res/app.ico

win32 {
    TargetConfig = $${PWD}/config.ini
    TargetConfig = $$replace(TargetConfig, /, \\)
    OutputDir = $${OUT_PWD}/$${DESTDIR}
    OutputDir = $$replace(OutputDir, /, \\)
    StaticDir = $${PWD}/static
    StaticDir = $$replace(StaticDir, /, \\)
    QMAKE_POST_LINK += $$quote(copy /Y \"$$TargetConfig\" \"$$OutputDir\" $$escape_expand(\\n\\t))
    QMAKE_POST_LINK += $$quote(xcopy /Y /E /I \"$$StaticDir\" \"$$OutputDir\\static\\\" $$escape_expand(\\n\\t))
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
