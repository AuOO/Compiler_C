#include "tablesymbol.h"

TableSymbol::TableSymbol()
{
    TableFuncElm temp;
    temp.type = "int";
    temp.cparam = -1;
    tableFunc.insert("printf",temp);

    temp.type = "int";
    temp.cparam = -1;
    tableFunc.insert("scanf",temp);
}

bool TableSymbol::addVar(QString name, QString func)
{
    TableIdElm var;
    TableId temp;
    var.type = "int";
    var.shift = 4*(tablesId.value(func).count()+1);
    if(tablesId.value(func).contains(name))
    {
        errors.append("Переменная '"+name+"' уже существует");
        return false;
    }

    temp = tablesId.take(func);
    temp.insert(name,var);
    tablesId.insert(func,temp);

    return true;
}

bool TableSymbol::addFunc(QString name)
{
    TableFuncElm temp;
    TableId tempTable;
    temp.type = "int";
    temp.cparam = 0;
    if(tableFunc.contains(name))
    {
        errors.append("Функция '"+name+"' уже существует");
        return false;
    }
    tableFunc.insert(name,temp);
    tablesId.insert(name,tempTable);
    return true;
}

bool TableSymbol::addParamFunc(QString name, QString varname)
{
    TableFuncElm temp;
    if(tableFunc.contains(name))
    {
        temp = tableFunc.take(name);
        temp.cparam++;
        temp.param.append(varname);
        tableFunc.insert(name,temp);
        return true;
    }
    else
        return false;
}

bool TableSymbol::chechVar(QString name, QString func)
{
    if(tablesId.value(func).contains(name))
        return true;
    else
    {
        errors.append("Переменная '"+name+"' неопределенна");
        return false;
    }
}

bool TableSymbol::checkFunc(QString name)
{
    if(tableFunc.contains(name))
        return true;
    else
    {
        errors.append("Функция '"+name+"' неопределенна");
        return false;
    }
}

bool TableSymbol::checkParams(int count, QString name)
{
    if(tableFunc.contains(name))
    {
        if(tableFunc.value(name).cparam==-1)
            return true;
        if(tableFunc.value(name).cparam==count)
            return true;
    }
    errors.append("Параметры функции '"+name+"' с аргументами  не совпадают");
    return false;
}

QList<QString> TableSymbol::getErrors()
{
    return errors;
}

TablesId TableSymbol::getTablesId()
{
    return tablesId;
}

TableFunc TableSymbol::getTableFunc()
{
    return tableFunc;
}

TableSymbol::~TableSymbol()
{

}

