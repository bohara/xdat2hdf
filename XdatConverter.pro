TEMPLATE = app

QT += qml quick widgets concurrent

SOURCES += main.cpp \
    converter.cpp \
    xdathdf.cpp \
    speciesinfo.cpp \
    atomicsystem.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    converter.h \
    xdathdf.h \
    speciesinfo.h \
    atomicsystem.h

macx: LIBS += -L$$PWD/../../../../../usr/local/hdf5/lib/ -lhdf5

INCLUDEPATH += $$PWD/../../../../../usr/local/hdf5/include/
DEPENDPATH += $$PWD/../../../../../usr/local/hdf5/include/

macx: PRE_TARGETDEPS += $$PWD/../../../../../usr/local/hdf5/lib/libhdf5.a
