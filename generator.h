#ifndef GENERATOR_H
#define GENERATOR_H

#include <syn.h>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

class Generator
{
public:
    Generator();
    void setProgram(Program,TablesId,TableFunc);
    void buildProgram();
    int buildExpr(TreeElm *,int,QString,int);
    QString autoTab(int);
    QString getAssembler();
    ~Generator();
private:
    QString assembler;
    QString buffer;
    Program program;
    TablesId tablesId;
    TableFunc tableFunc;
    int imax;
    int imetka;
    int countLit;
};

#endif // GENERATOR_H
