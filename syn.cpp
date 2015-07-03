#include "syn.h"


Syn::Syn(Lex *aLex)
{
    lex = aLex;
}

bool Syn::parsing()
{
    return on_Program();
}

bool Syn::checkSemantic()
{
    QString type;
    foreach(CallFuncElm elm,callFuncs)
    {
        if(elm.name=="printf")
        {
            for(int i=0;i<elm.arg.count();i++)
            {
                type = elm.arg.at(i);
                if(i==0&&type!="lit") return false;
                if(i>0&&type!="int"&&type!="pint") return false;
            }
        }
        else if(elm.name=="scanf")
        {
            for(int i=0;i<elm.arg.count();i++)
            {
                type = elm.arg.at(i);
                if(i==0&&type!="lit") return false;
                if(i>0&&type!="pint") return false;
            }
        }
        else
        {
            foreach(QString type,elm.arg)
            {
                if(type!="int") return false;
            }
        }
    }
    return true;
}

bool Syn::on_Program()
{
    Token t;
    if(!on_Funcs()) return false;
    if(lex->getToken(t)) return false;
    if(!tableSymbol.checkFunc("main")) return false;
    if(!tableSymbol.checkParams(0,"main")) return false;
    return true;
}

bool Syn::on_Funcs()
{
    if(!on_Func()) return false;
    if(!lex->isEmpty())
    {
        if(!on_Funcs()) return false;
    }
    return true;
}

bool Syn::on_Func()
{
    Token t;
    QString name;
    if(!lex->getToken(t)) return false;
    if(t.terminal!="int") return false;
    if(!lex->getToken(t)) return false;
    if(t.terminal!="id") return false;
    name = t.attribute;
    if(!tableSymbol.addFunc(t.attribute)) return false;
    curFunc = t.attribute;
    if(!lex->getToken(t)) return false;
    if(t.terminal!="(") return false;
    if(!on_Params()) return false;
    if(!lex->getToken(t)) return false;
    if(t.terminal!=")") return false;

    TreeElm *root = addTreeElm({"stmt",""});
    program.insert(name,root);
    if(!on_Block(root)) return false;
    return true;
}

bool Syn::on_Params()
{
    Token t;
    if(!on_Param()) return false;

    lex->saveStatement();
    if(!lex->getToken(t)) return false;
    if(t.terminal==",")
    {
        if(!on_Params()) return false;
    }
    else
    {
        lex->rollBack();
    }
    return true;
}

bool Syn::on_Param()
{
    Token t;
    lex->saveStatement();
    if(!lex->getToken(t)) return false;
    if(t.terminal=="int")
    {
        if(!lex->getToken(t)) return false;
        if(t.terminal!="id") return false;
        if(!tableSymbol.addVar(t.attribute,curFunc)) return false;
        tableSymbol.addParamFunc(curFunc,t.attribute);
    }
    else
        lex->rollBack();
    return true;
}

bool Syn::on_Block(TreeElm *root)
{
    Token t;
    if(!lex->getToken(t)) return false;
    if(t.terminal!="{") return false;

    if(!on_Stmts(root)) return false;

    if(!lex->getToken(t)) return false;
    if(t.terminal!="}") return false;
    return true;
}

bool Syn::on_Stmts(TreeElm *root)
{
    Token t;
    if(!on_Stmt(root)) return false;

    lex->saveStatement();
    if(!lex->getToken(t)) return false;
    lex->rollBack();

    if(t.terminal!="}")
    {
        root->left = addTreeElm({"stmt",""});
        if(!on_Stmts(root->left))return false;
    }
    return true;
}

bool Syn::on_Stmt(TreeElm *root)
{
    Token t;
    lex->saveStatement();
    if(!lex->getToken(t)) return false;
    if(t.terminal=="int")
    {
        lex->rollBack();
        if(!on_Decl(root)) return false;
        if(!lex->getToken(t)) return false;
        if(t.terminal!=";") return false;

    }
    else if(t.terminal=="id")
    {
        lex->rollBack();
        if(!on_Instr(root)) return false;
        if(!lex->getToken(t)) return false;
        if(t.terminal!=";") return false;
    }
    else if(t.terminal=="return")
    {
        lex->rollBack();
        if(!on_Return(root)) return false;
        if(!lex->getToken(t)) return false;
        if(t.terminal!=";") return false;
    }
    else if(t.terminal=="if")
    {
        root->right = addTreeElm({"if",""});

        if(!lex->getToken(t)) return false;
        if(t.terminal!="(") return false;

        if(!on_Bool(root->right)) return false;

        if(!lex->getToken(t)) return false;
        if(t.terminal!=")") return false;

        root->right->left = addTreeElm({"else",""});
        root->right->left->right = addTreeElm({"stmt",""});
        if(!on_Block(root->right->left->right)) return false;

        lex->saveStatement();
        if(!lex->getToken(t)) return false;
        if(t.terminal=="else")
        {
            root->right->left->left = addTreeElm({"stmt",""});
            if(!on_Block(root->right->left->left)) return false;
        }
        else
            lex->rollBack();
    }
    else if(t.terminal=="while")
    {
        root->right = addTreeElm({"while",""});

        if(!lex->getToken(t)) return false;
        if(t.terminal!="(") return false;

        if(!on_Bool(root->right)) return false;

        if(!lex->getToken(t)) return false;
        if(t.terminal!=")") return false;

        root->right->left = addTreeElm({"stmt",""});
        if(!on_Block(root->right->left)) return false;
    }
    else if(t.terminal=="}")
        lex->rollBack();
    else
    {
        lex->rollBack();
        return false;
    }
    return true;
}

bool Syn::on_Decl(TreeElm *root)
{
    Token t;
    Token left;
    if(!lex->getToken(t)) return false;
    if(t.terminal!="int") return false;

    root->right = addTreeElm(t);

    if(!lex->getToken(t)) return false;
    if(t.terminal!="id") return false;
    if(!tableSymbol.addVar(t.attribute,curFunc)) return false;
    left = t;
    root->right->left = addTreeElm(t);

    lex->saveStatement();
    if(!lex->getToken(t)) return false;
    if(t.terminal!="=")
        lex->rollBack();
    else
    {
        root->right->right = addTreeElm(t);
        root->right->right->left = addTreeElm(left);
        if(!on_Bool(root->right->right)) return false;
    }
    return true;
}

bool Syn::on_Instr(TreeElm *root)
{
    Token t;
    Token left;
    QString name;
    lex->saveStatement();
    if(!lex->getToken(t)) return false;
    if(t.terminal!="id") return false;
    name = t.attribute;
    if(!lex->getToken(t)) return false;
    lex->rollBack();
    if(t.terminal=="(")
    {
        root->right = addTreeElm({"call",name});
        if(!on_Cfunc(root->right)) return false;
        return true;
    }
    else
    {
        lex->getToken(t);
        if(!tableSymbol.chechVar(t.attribute,curFunc)) return false;
        left = t;
        if(!lex->getToken(t)) return false;
        if(t.terminal=="=")
        {
            root->right = addTreeElm(t);
            root->right->left = addTreeElm(left);
            if(!on_Bool(root->right)) return false;
        }
        else
            return false;
    }

    return true;
}

bool Syn::on_Return(TreeElm *root)
{
    Token t;
    if(!lex->getToken(t)) return false;
    if(t.terminal!="return") return false;
    root->right = addTreeElm(t);
    if(!on_Bool(root->right)) return false;
    return true;
}

bool Syn::on_Literal(TreeElm *root)
{
    Token t;
    if(!lex->getToken(t)) return false;
    if(t.terminal!="lit") return false;
    root->right = addTreeElm(t);
    return true;
}

bool Syn::on_Address(TreeElm *root)
{
    Token t;
    if(!lex->getToken(t)) return false;
    if(t.terminal!="&") return false;
    root->right = addTreeElm(t);
    if(!lex->getToken(t)) return false;
    if(t.terminal!="id") return false;
    root->right->right = addTreeElm(t);
    return true;
}

bool Syn::on_Bool(TreeElm *root)
{
    Token t;
    TreeElm *p = new TreeElm;
    if(!on_Join(p)) return false;
    lex->saveStatement();

    if(!lex->getToken(t)) return false;
    if(t.terminal=="||")
    {
        root->right = addTreeElm(t);
        root->right->left = p->right;
        if(!on_Bool(root->right)) return false;
        return true;
    }

    lex->rollBack();
    root->right = p->right;
    return true;
}

bool Syn::on_Join(TreeElm *root)
{
    Token t;
    TreeElm *p = new TreeElm;
    if(!on_Equality(p)) return false;
    lex->saveStatement();

    if(!lex->getToken(t)) return false;
    if(t.terminal=="&&")
    {
        root->right = addTreeElm(t);
        root->right->left = p->right;
        if(!on_Join(root->right)) return false;
        return true;
    }

    lex->rollBack();
    root->right = p->right;
    return true;
}

bool Syn::on_Equality(TreeElm *root)
{
    Token t;
    TreeElm *p = new TreeElm;
    if(!on_Rel(p)) return false;
    lex->saveStatement();

    if(!lex->getToken(t)) return false;
    if(t.terminal=="=="||t.terminal=="!=")
    {
        root->right = addTreeElm(t);
        root->right->left = p->right;
        if(!on_Equality(root->right)) return false;
        return true;
    }

    lex->rollBack();
    root->right = p->right;
    return true;
}

bool Syn::on_Rel(TreeElm *root)
{
    Token t;
    TreeElm *p = new TreeElm;
    if(!on_Expr(p)) return false;
    lex->saveStatement();

    if(!lex->getToken(t)) return false;
    if(t.terminal=="<"||t.terminal==">"||t.terminal==">="
            ||t.terminal=="<=")
    {
        root->right = addTreeElm(t);
        root->right->left = p->right;
        if(!on_Expr(root->right)) return false;
        return true;
    }

    lex->rollBack();
    root->right = p->right;
    return true;
}

bool Syn::on_Expr(TreeElm *root)
{
    Token t;
    TreeElm *p = new TreeElm;
    if(!on_Term(p)) return false;
    lex->saveStatement();

    if(!lex->getToken(t)) return false;
    if(t.terminal=="+"||t.terminal=="-")
    {
        root->right = addTreeElm(t);
        root->right->left = p->right;
        if(!on_Expr(root->right)) return false;
        return true;
    }

    lex->rollBack();
    root->right = p->right;
    return true;
}

bool Syn::on_Term(TreeElm *root)
{
    Token t;
    TreeElm *p = new TreeElm;
    if(!on_Unary(p)) return false;
    lex->saveStatement();

    if(!lex->getToken(t)) return false;
    if(t.terminal=="*"||t.terminal=="/"||t.terminal=="%")
    {
        root->right = addTreeElm(t);
        root->right->left = p->right;
        if(!on_Term(root->right)) return false;
        return true;
    }

    lex->rollBack();
    root->right = p->right;
    return true;
}

bool Syn::on_Unary(TreeElm *root)
{
    Token t;
    lex->saveStatement();
    if(!lex->getToken(t)) return false;
    if(t.terminal=="!"||t.terminal=="-")
    { 
        root->right = addTreeElm(t);
        if(!on_Factor(root->right)) return false;
    }
    else
    {
        lex->rollBack();
        if(!on_Factor(root)) return false;
    }

    return true;
}

bool Syn::on_Cfunc(TreeElm *root)
{
    Token t;
    CallFuncElm callFunc;
    QString name;
    if(!lex->getToken(t)) return false;
    if(t.terminal=="id")
    {
        name = t.attribute;
        if(!tableSymbol.checkFunc(t.attribute)) return false;
        if(!lex->getToken(t)) return false;
        if(t.terminal!="(") return false;
        countArg.push(0);
        root->right = addTreeElm({"arg",""});
        callFunc.name = name;
        callFuncs.append(callFunc);
        if(!on_Args(root->right)) return false;
        if(!tableSymbol.checkParams(countArg.pop(),name)) return false;
        if(!lex->getToken(t)) return false;
        if(t.terminal!=")") return false;
    }
    else
        return false;
    return true;
}

bool Syn::on_Args(TreeElm *root)
{
    Token t;
    if(!on_Arg(root)) return false;
    lex->saveStatement();

    if(!lex->getToken(t)) return false;
    if(t.terminal==",")
    {
        root->left = addTreeElm({"arg",""});
        if(!on_Args(root->left)) return false;
    }

    lex->rollBack();
    return true;
}

bool Syn::on_Arg(TreeElm *root)
{
    Token t;
    lex->saveStatement();
    if(!lex->getToken(t)) return false;
    lex->rollBack();
    if(t.terminal==")") return true;

    lex->saveStatement();

    if(!on_Literal(root))
    {
        lex->rollBack();
        if(!lex->getToken(t)) return false;
        if(t.terminal!="&")
        {
            lex->rollBack();
            callFuncs.last().arg.append("int");
            if(!on_Bool(root)) return false;
        }
        else
        {
            lex->rollBack();
            callFuncs.last().arg.append("pint");
            if(!on_Address(root)) return false;
        }
    }
    else
    {
        callFuncs.last().arg.append("lit");
    }
    countArg.last()++;

    return true;
}

bool Syn::on_Factor(TreeElm *root)
{
    Token t;
    QString name;
    lex->saveStatement();
    if(!lex->getToken(t)) return false;
    if(t.terminal=="id")
    {
        name = t.attribute;
        if(!lex->getToken(t)) return false;
        lex->rollBack();
        if(t.terminal=="(")
        {
            root->right = addTreeElm({"call",name});
            if(!on_Cfunc(root->right))return false;
            return true;
        }
        else
        {
            lex->getToken(t);
            if(!tableSymbol.chechVar(t.attribute,curFunc)) return false;
            root->right = addTreeElm(t);
            return true;
        }
    }
    if(t.terminal=="(")
    {
        if(!on_Bool(root)) return false;
        if(!lex->getToken(t)) return false;
        if(t.terminal!=")") return false;
        return true;
    }
    if(t.terminal=="&")
    {
        lex->rollBack();
        if(!on_Address(root)) return false;
        return true;
    }
    if(t.terminal=="num")
    {
        root->right = addTreeElm(t);
        return true;
    }
    else
        return false;
}

QList<QString> Syn::getErrors()
{
    return tableSymbol.getErrors();
}

Program Syn::getProgram()
{
    return program;
}

TablesId Syn::getTablesId()
{
    return tableSymbol.getTablesId();
}

TableFunc Syn::getTableFunc()
{
    return tableSymbol.getTableFunc();
}

Syn::~Syn()
{
    foreach(TreeElm *root,program)
        deleteTree(root);
}

TreeElm* Syn::addTreeElm(Token token)
{
    TreeElm *p = new TreeElm;
    p->left = NULL;
    p->right = NULL;
    p->token = token;
    return p;
}

void Syn::deleteTree(TreeElm *root)
{
    if(root!=NULL)
    {
        deleteTree(root->right);
        deleteTree(root->left);
        delete root;
    }
}

