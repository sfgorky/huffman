#-------------------------------------------------
#
# Project created by QtCreator 2015-10-18T20:41:39
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = huffman
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += \
    ./FrequencyTable.cpp \
    ./Huffman.cpp \
    ./HuffmanCode.cpp \
    ./HuffmanNode.cpp \
    ./HuffmanTest.cpp \
    ./MemBuffer.cpp \
    ./Stream.cpp \
    ./SymbolEncode.cpp \
    ./Util.cpp \
    ./main.cpp

HEADERS += \
    ./FrequencyTable.h \
    ./Huffman.h \
    ./HuffmanCode.h \
    ./HuffmanNode.h \
    ./MemBuffer.h \
    ./Stream.h \
    ./Symbol.h \
    ./SymbolEncode.h \
    ./Util.h



CONFIG(debug, debug|release) {
    DESTDIR = ../build/debug
} else {
    DESTDIR = ../build/release
}

OBJECTS_DIR = $$DESTDIR/.obj 
MOC_DIR = $$DESTDIR/.moc 
RCC_DIR = $$DESTDIR/.qrc 
UI_DIR = $$DESTDIR/.ui


