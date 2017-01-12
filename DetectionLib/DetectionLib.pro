QT += core
QT -= gui

CONFIG += c++11

#QMAKE_LFLAGS += /LARGEADDRESSAWARE

TARGET = DetectionLib
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = lib

SOURCES += \
    worker.cpp \
    threadpool.cpp \
    locker.cpp

HEADERS += \
    worker.h \
    threadpool.h \
    locker.h \
    sharedlibrary.h \
    types.h

DEFINES += SHAREDLIB_LIBRARY_DEF

OPENCV_PATH = $$PWD/../../../OpenCV-3.2.0

INCLUDEPATH += $${OPENCV_PATH}/include

Debug: OPENCV_POSTFIX = d
Release: OPENCV_POSTFIX = \


LIBS += -L$${OPENCV_PATH}/$${QT_ARCH}/lib \
        -lopencv_highgui320$$OPENCV_POSTFIX \
        -lopencv_core320$$OPENCV_POSTFIX \
        -lopencv_imgproc320$$OPENCV_POSTFIX \
        -lopencv_imgcodecs320$$OPENCV_POSTFIX \
        -lopencv_objdetect320$$OPENCV_POSTFIX

Debug: BUILD_PATH = $$PWD/../$${QT_ARCH}/Debug
Release: BUILD_PATH = $$PWD/../$${QT_ARCH}/Release


DESTDIR = $${BUILD_PATH}
