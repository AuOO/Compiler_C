#ifndef TABLESYMBOL_H
#define TABLESYMBOL_H

#include <QMap>
#include <QVector>
#include <QString>
#include <QDebug>

struct TableIdElm
{
    QString type;
    int shift;
};

struct TableFuncElm
{
    QString type;
    int cparam;
    QList<QString> param;
};

typedef QMap<QString,TableIdElm> TableId;
typedef QMap<QString,TableId> TablesId;
typedef QMap<QString,TableFuncElm> TableFunc;

class TableSymbol
{
public:
    TableSymbol();
    bool addVar(QString,QString);
    bool addFunc(QString);
    bool addParamFunc(QString,QString);
    bool chechVar(QString,QString);
    bool checkFunc(QString);
    bool checkParams(int,QString);
    QList<QString> getErrors();
    TablesId getTablesId();
    TableFunc getTableFunc();
    ~TableSymbol();
private:
    TablesId tablesId;
    TableFunc tableFunc;
    QList<QString> errors;
};

#endif // TABLESYMBOL_H
