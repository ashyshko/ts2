QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = desktop-test
TEMPLATE = app

INCLUDEPATH += ../../backend
INCLUDEPATH += ../../backend/hub
INCLUDEPATH += ../../backend/tl

SOURCES += main.cpp

HEADERS += ../../backend/hub/Defs.h
SOURCES += ../../backend/hub/Defs.cpp

HEADERS += ../../backend/hub/CHub.h

HEADERS += ../../backend/hub/CJunction.h
SOURCES += ../../backend/hub/CJunction.cpp
HEADERS += ../../backend/hub/CPath.h
SOURCES += ../../backend/hub/CPath.cpp
HEADERS += ../../backend/hub/CVehicle.h
SOURCES += ../../backend/hub/CVehicle.cpp
HEADERS += ../../backend/hub/CWayPoint.h
SOURCES += ../../backend/hub/CWayPoint.cpp

HEADERS += ../../backend/tl/CMap.h
SOURCES += ../../backend/tl/CMap.cpp



