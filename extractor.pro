#-------------------------------------------------
#
# Project created by QtCreator 2012-09-29T10:46:05
#
#-------------------------------------------------

QT       += core gui

TARGET = extractor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    extractthread.cpp \
    framedialog.cpp \
    framesthread.cpp \
    QProgressIndicator.cpp \
    qvideofile.cpp

HEADERS  += mainwindow.h \
    extractthread.h \
    framedialog.h \
    framesthread.h \
    QProgressIndicator.h \
    qvideofile.h

FORMS    += mainwindow.ui \
    framedialog.ui

win32:INCLUDEPATH += C:/Qt/ffmpeg/include
win32:LIBS += -LC:/Qt/ffmpeg/lib
LIBS += -lavutil -lavcodec -lavformat -lswscale
#LIBS += C:/Qt/4.7.4/plugins/codecs/libqcncodecs4.a

win32:RC_FILE = extractor.rc

RESOURCES += \
    extractor.qrc

OTHER_FILES += \
    extractor.ts \
    ChangeLog.txt
