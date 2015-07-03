#include "syntaxhighlighter.h"

SyntaxHighlighter::SyntaxHighlighter(QObject *parent) : QSyntaxHighlighter(parent)
{

}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    QTextCharFormat format;
    QRegExp rx;
    int index;
    int length;
    rx.setPattern("^int\\s|[\\W]+int\\s|^while|[\\W]+while|^return|[\\W]+return|"
                  "^if|[\\W]+if|^else|[\\W]+else|^char|[\\W]+char");
    format.setForeground(QColor(180,180,20));
    index = rx.indexIn(text);
    while(index >= 0)
    {
        length = rx.matchedLength();
        setFormat(index,length,format);
        index = rx.indexIn(text,index+length);
    }

    rx.setPattern("[\\W][0-9]+");
    format.setForeground(QColor(50,50,200));
    index = rx.indexIn(text);
    while(index >= 0)
    {
        length = rx.matchedLength();
        setFormat(index,length,format);
        index = rx.indexIn(text,index+length);
    }
    rx.setPattern("[+|\\-|*|/|!|\(|\)|<|>|=|{|}|,|&|%]");
    format.setForeground(QColor(0,0,0));
    index = rx.indexIn(text);
    while(index >= 0)
    {
        length = rx.matchedLength();
        setFormat(index,length,format);
        index = rx.indexIn(text,index+length);
    }
    rx.setPattern("\".*\"");
    format.setForeground(QColor(0,180,0));
    index = rx.indexIn(text);
    while(index >= 0)
    {
        length = rx.matchedLength();
        setFormat(index,length,format);
        index = rx.indexIn(text,index+length);
    }
}
