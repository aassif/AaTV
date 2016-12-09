# -------------------------------------------------
# Project created by QtCreator 2010-05-31T16:17:14
# -------------------------------------------------
QT += widgets network
TARGET = AaTV
TEMPLATE = app
SOURCES += \
    ../qwidgets/src/qfilewidget.cc \
    AaTV.cc \
    AaTVDB.cc
HEADERS += \
    ../toolkit/include/bits/AaException.hh \
    ../toolkit/include/bits/AaText.hh \
    ../toolkit/include/bits/AaXml.hh \
    ../qwidgets/include/qfilewidget.h \
    AaTV.h \
    AaTVDB.h
INCLUDEPATH += ../toolkit/include \
    ../qwidgets/include
LIBS += -lQt5Concurrent
RESOURCES += AaTV.qrc
RC_FILE += AaTV.rc
