QT      += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += sql
QT += printsupport

TEMPLATE = app
QMAKE_CXXFLAGS = -std=c++11
TARGET = SchoolSystem

HEADERS += \
    dialog.h \
    mainwindow.h \
    form.h \
    databasehandler.h \
    memory.h \
    existingstudent.h \
    showstudent.h \
    logindialog.h \
    adminform.h \
    studentresult.h

SOURCES += \
    dialog.cpp \
    main.cpp \
    mainwindow.cpp \
    form.cpp \
    databasehandler.cpp \
    existingstudent.cpp \
    showstudent.cpp \
    logindialog.cpp \
    adminform.cpp \
    studentresult.cpp

FORMS += \
    form.ui \
    showstudent.ui \
    studentresult.ui

OTHER_FILES += comsci \
    fstdep \
    sltdep \
    stadep \
    result.html \
    print.html

RESOURCES += images.qrc
