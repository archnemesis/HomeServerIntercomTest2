#-------------------------------------------------
#
# Project created by QtCreator 2018-03-24T20:06:53
#
#-------------------------------------------------

QT       += core gui multimedia network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HomeServerIntercomTest2
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    messageparser.cpp \
    command.c \
    configuration_payload.c \
    intercom_channel_accept.c \
    intercom_channel_create.c \
    intercom_channel_request.c \
    ping.c \
    request_error.c \
    intercom_directory_listing.c

HEADERS += \
        mainwindow.h \
    messageparser.h \
    command.h \
    configuration_payload.h \
    intercom_channel_accept.h \
    intercom_channel_create.h \
    intercom_channel_request.h \
    message.h \
    ping.h \
    request_error.h \
    intercom_directory_listing.h

FORMS += \
        mainwindow.ui
