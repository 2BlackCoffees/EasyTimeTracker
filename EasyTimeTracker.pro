#-------------------------------------------------
#
# Project created by QtCreator 2013-01-22T18:47:11
#
#-------------------------------------------------

QT       += core xml  widgets  printsupport
#QT       += webkit webkitwidgets
# multimediawidgets multimedia

TARGET = EasyTimeTracker
TEMPLATE = app
RC_FILE = ett.rc

#Uncomment following 4 lines to activate unit tests
#CONFIG += qtestlib
#DEFINES += UNIT_TEST_MODE=1
#SOURCES +=     UnitTest/testeasytimetracker.cpp
#HEADERS +=     UnitTest/testeasytimetracker.h

DEFINES += PRINT_DEBUG=1

# Boost
INCLUDEPATH += C:\BackupHD\Development\Qt\Development\boost_1_55_0
LIBS += -LC:\BackupHD\Development\Qt\Development\boost_1_55_0\stage\lib

#OpenGL
#INCLUDEPATH += C:/Qt/Qt5.2-mingw/5.2.0/mingw48_32/mkspecs/common/ios

QMAKE_CXXFLAGS += -std=c++0x -pthread
# For static linking
QMAKE_LFLAGS = -static-libgcc -static-libstdc++ -static
CONFIG += static
LIBS += -pthread

SOURCES += main.cpp\
        mainwindow.cpp \
    timetrackingmodel.cpp \
    timetrackingsettings.cpp \
    debug.cpp \
    iofileaccesses.cpp \
    iiofileaccesses.cpp \
    misc.cpp \
    timetrackingwindow.cpp \
    itimetrackingwindow.cpp \
    observersubject.cpp \
    iobserver.cpp \
    Preferences/preferencesdialog.cpp \
    Preferences/tablemodeladapter.cpp \
    Preferences/combodelegate.cpp \
    Settings/iiosettings.cpp \
    Settings/iosettings.cpp \
    TimeTrackingAction/timetrackingaction.cpp \
    tableviewmodel.cpp \
    tableviewdelegate.cpp \
    tablemodel.cpp \
    Preferences/tablemodelpreferences.cpp \
    PDFCreator/pdfcreator.cpp \
    Pause/pausedialog.cpp \
    DialogTimeFollower/dialogtimefollower.cpp \
    DialogAbout/dialogabout.cpp \
    Promoted/clickablelabel.cpp \
    Misc/clickableimage.cpp \
    Misc/disconnecttemporarily.cpp

HEADERS  += mainwindow.h \
    timetrackingmodel.h \
    ttexceptions.h \
    debug.h \
    timetrackingsettings.h \
    misc.h \
    iofileaccesses.h \
    iiofileaccesses.h \
    iconnection.h \
    timetrackingwindow.h \
    itimetrackingwindow.h \
    observersubject.h \
    iobserver.h \
    Preferences/preferencesdialog.h \
    Preferences/tablemodeladapter.h \
    Preferences/combodelegate.h \
    Settings/iosettings.h \
    Settings/iiosettings.h \
    TimeTrackingAction/timetrackingaction.h \
    tableviewmodel.h \
    tableviewdelegate.h \
    tablemodel.h \
    Preferences/tablemodelpreferences.h \
    PDFCreator/pdfcreator.h \
    Pause/pausedialog.h \
    DialogTimeFollower/dialogtimefollower.h \
    DialogAbout/dialogabout.h \
    Promoted/clickablelabel.h \
    Misc/dynamicfields.h \
    Misc/clickableimage.h \
    Misc/disconnecttemporarily.h

FORMS    += mainwindow.ui \
    Preferences/preferencesdialog.ui \
    Pause/pausedialog.ui \
    DialogTimeFollower/dialogtimefollower.ui \
    DialogAbout/dialogabout.ui
RESOURCES = EasyTimeTracker.qrc
