TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    core_api.cpp \
    main.cpp \
    sim_mem.c

HEADERS += \
    core_api.h \
    sim_api.h
