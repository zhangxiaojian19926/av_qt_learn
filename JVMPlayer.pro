QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = JCMPlayer

TEMPLATE = app

# libx264
# ffmpeg
INCLUDEPATH += $$PWD/3rdparty/linux/libffmpeg/include
INCLUDEPATH += $$PWD/3rdparty/linux/libaac/include

LIBS += -L$$PWD/3rdparty/linux/libffmpeg/lib -lavformat  -lavdevice -lavcodec  -lavutil -lswresample -lswscale -pthread  -ldl -lbz2 -lz -lm
LIBS += -L$$PWD/3rdparty/linux/libx264 -lx264
LIBS += -L$$PWD/3rdparty/linux/libaac -laac

LIBS += -lm -lX11 -lvdpau

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CCOpenGLWidget.cpp \
    jcavcoderhandler.cpp \
    main.cpp \
    jcmplayer.cpp

HEADERS += \
    CCOpenGLWidget.h \
    CCYUVDataDefine.h \
    JCDataDefine.h \
    jcavcoderhandler.h \
    jcmplayer.h

FORMS += \
    jcmplayer.ui
