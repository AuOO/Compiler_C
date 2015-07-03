#-------------------------------------------------
#
# Project created by QtCreator 2015-03-07T14:56:31
#
#-------------------------------------------------

QT       += core gui

win32 {
    RC_FILE += app.rc
    OTHER_FILES += app.rc
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Compiler_C
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    lex.cpp \
    syn.cpp \
    tablesymbol.cpp \
    generator.cpp \
    syntaxhighlighter.cpp

HEADERS  += widget.h \
    lex.h \
    syn.h \
    tablesymbol.h \
    generator.h \
    syntaxhighlighter.h

