TEMPLATE = app
TARGET = siilihai

isEmpty(PREFIX) {
  PREFIX = /usr
}

BINDIR = $$PREFIX/bin
DATADIR = $$PREFIX/share
DESTDIR = .

ICON = ../../data/siilihai.icns

!contains(QMAKE_HOST.arch, x86_64) {
   LIBDIR = $$PREFIX/lib
} else {
   LIBDIR = $$PREFIX/lib64
}

exists(../../siilihai-version.h) {
     DEFINES += INCLUDE_SIILIHAI_VERSION
}

CONFIG(debug) {
    message(Debug build - enabling some extra stuff)
    DEFINES += DEBUG_INFO
}

QT += core webkit webkitwidgets gui network xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

HEADERS += messageviewwidget.h \
    threadlistwidget.h \
    settingsdialog.h \
    reportparser.h \
    forumlistwidget.h \
    favicon.h \
    groupsubscriptiondialog.h \
    subscribewizard.h \
    mainwindow.h \
    siilihai.h \
    loginwizard.h \
    forumproperties.h \
    threadproperties.h \
    threadlistmessageitem.h \
    threadlistthreaditem.h \
    threadlistshowmoreitem.h \
    useraccountdialog.h

SOURCES += messageviewwidget.cpp \
    threadlistwidget.cpp \
    settingsdialog.cpp \
    reportparser.cpp \
    forumlistwidget.cpp \
    favicon.cpp \
    groupsubscriptiondialog.cpp \
    subscribewizard.cpp \
    mainwindow.cpp \
    siilihai.cpp \
    loginwizard.cpp \
    main.cpp \
    forumproperties.cpp \
    threadproperties.cpp \
    threadlistmessageitem.cpp \
    threadlistthreaditem.cpp \
    threadlistshowmoreitem.cpp \
    useraccountdialog.cpp

DEPENDPATH += ../parsermaker ../common

target.path = $$BINDIR

INSTALLS += target
FORMS = settingsdialog.ui \
    reportparser.ui \
    groupsubscriptiondialog.ui \
    subscribeforum_verify.ui \
    subscribeforum_login.ui \
    subscribeforum.ui \
    mainwindow.ui \
    forumproperties.ui \
    threadproperties.ui \
    useraccountdialog.ui
RESOURCES = ../../siilihairesources.qrc

# Needed so that uic-generated headers for the forms in
# common and parsermaker work when shadow building
INCLUDEPATH += $$OUT_PWD/../common $$OUT_PWD/../parsermaker

win32 {
    INCLUDEPATH += ../../../libsiilihai
    DEFINES += STORE_FILES_IN_APP_DIR
    message("Win32 build - storing files in app dir")
}
win32:debug {
    LIBS += -L../../../libsiilihai/src/debug
    LIBS += -L../common/debug
    LIBS += -L../parsermaker/debug
    DEPENDPATH += ../../../libsiilihai/src/
    DEPENDPATH += -L../common/debug
}
win32:release {
    LIBS += -L../../../libsiilihai/src/release
    LIBS += -L../common/release
    LIBS += -L../parsermaker/release
    DEPENDPATH += ../../../libsiilihai/src/
    DEPENDPATH += -L../common/release
}

LIBS += -L../parsermaker \
    -L../common \
    -lsiilihai \
    -lsiilihai-common \
    -lsiilihai-parsermaker
