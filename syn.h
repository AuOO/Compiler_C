#ifndef SYN_H
#define SYN_H

#include <lex.h>
#include <tablesymbol.h>
#include <QStack>
#include <QList>
#include <QMap>

struct TreeElm
{
    Token token;
    TreeElm *left;
    TreeElm *right;
};

struct CallFuncElm
{
    QString name;
    QList<QString> arg;
};

typedef QMap<QString,TreeElm*> Program;
typedef QList<CallFuncElm> TableCallFunc;

class Syn
{
public:
    Syn(Lex*);
    bool parsing();
    bool checkSemantic();
    bool on_Program();
    bool on_Funcs();
    bool on_Func();
    bool on_Params();
    bool on_Param();
    bool on_Block(TreeElm*);
    bool on_Stmts(TreeElm*);
    bool on_Stmt(TreeElm*);
    bool on_Decl(TreeElm*);
    bool on_Instr(TreeElm*);
    bool on_Return(TreeElm*);
    bool on_Literal(TreeElm*);
    bool on_Address(TreeElm*);
    bool on_Bool(TreeElm*);
    bool on_Join(TreeElm*);
    bool on_Equality(TreeElm*);
    bool on_Rel(TreeElm*);
    bool on_Expr(TreeElm*);
    bool on_Term(TreeElm*);
    bool on_Unary(TreeElm*);
    bool on_Cfunc(TreeElm*);
    bool on_Args(TreeElm*);
    bool on_Arg(TreeElm*);
    bool on_Factor(TreeElm*);

    QList<QString> getErrors();
    Program getProgram();
    TablesId getTablesId();
    TableFunc getTableFunc();
    ~Syn();
private:
    Lex *lex;
    Program program;
    TableSymbol tableSymbol;
    QString curFunc;
    QStack<int> countArg;
    TableCallFunc callFuncs;

    TreeElm* addTreeElm(Token);
    void deleteTree(TreeElm*);
};

#endif // SYN_H
