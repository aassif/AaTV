# -------------------------------------------------
# Project created by QtCreator 2010-05-31T16:17:14
# -------------------------------------------------
QT += network
TARGET = AaTV
TEMPLATE = app
SOURCES += AaTV.cc \
    AaTVDB.cc \
    ../../wrk/qwidgets/src/qfilewidget.cc \
    ../../wrk/toolkit/src/AaXml.cc \
    ../../wrk/toolkit/src/AaException.cc
HEADERS += AaTV.h \
    ../../wrk/qwidgets/include/qfilewidget.h \
    ../../wrk/toolkit/include/bits/AaXml.h \
    ../../wrk/toolkit/include/bits/AaException.h
INCLUDEPATH += ../../wrk/toolkit/include \
    ../../wrk/qwidgets/include
