#-------------------------------------------------
#
# Project created by QtCreator 2013-02-28T15:01:06
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = InfMedica
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    capturethread.cpp

HEADERS  += mainwindow.h \
    capturethread.h

FORMS    += mainwindow.ui

INCLUDEPATH += /usr/include/opencv
LIBS += -L/usr/local/lib
LIBS += -lopencv_core
#LIBS += -lopencv_imgproc
LIBS += -lopencv_highgui
LIBS += -ldecodeqr
LIBS += -lcurl
LIBS += -ljsoncpp
#LIBS += -lopencv_ml
#LIBS += -lopencv_video
#LIBS += -lopencv_features2d
#LIBS += -lopencv_calib3d
#LIBS += -lopencv_objdetect
#LIBS += -lopencv_contrib
#LIBS += -lopencv_legacy
#LIBS += -lopencv_flann

OTHER_FILES += \
    user.jpg \
    icon.jpg \
    dalmata.png

RESOURCES += \
    images.qrc
