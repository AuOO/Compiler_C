#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QIcon>
#include <QDebug>
#include <QFileDialog>
#include <QTextStream>
#include <QFile>
#include <QMessageBox>
#include <QMap>
#include <QSet>
#include <QProcess>
#include <QRegularExpression>
#include <QDir>
#include <lex.h>
#include <syn.h>
#include <tablesymbol.h>
#include <generator.h>
#include <syntaxhighlighter.h>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();
private:
    QTextEdit *textEdit;
    QTextEdit *textDebug;
    QVBoxLayout *mainLay;

    QMenuBar *mainMenu;
    QMenu *fileMenu;
    QMenu *correctMenu;
    QMenu *runMenu;
    QMenu *aboutMenu;

    SyntaxHighlighter *hlighter;
    QString fileName;
public slots:
    void NewFile();
    void OpenFile();
    void SaveFile();
    void SaveAsFile();
    void About();
    void Run();
};

#endif // WIDGET_H
