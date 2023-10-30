TEMPLATE = app
CONFIG += warn_on \
	  thread \
          qt
TARGET = ../bin/ikafe

SOURCES += anaPencere.cpp \
main.cpp \
server.cpp
HEADERS += anaPencere.h \
server.h
FORMS += anaPencere.ui

QT += network
QT += gui

