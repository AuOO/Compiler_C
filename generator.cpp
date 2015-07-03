#include "generator.h"

Generator::Generator()
{
    countLit = 0;
}

void Generator::setProgram(Program prog, TablesId tableId, TableFunc tableF)
{
    program = prog;
    tablesId = tableId;
    tableFunc = tableF;
}


Generator::~Generator()
{

}

void Generator::buildProgram()
{
    assembler.append(".386\n");
    assembler.append(".model flat, stdcall\n");
    assembler.append("option casemap :none\n");
    assembler.append("include \\masm32\\include\\windows.inc\n");
    assembler.append("include \\masm32\\include\\masm32.inc\n");
    assembler.append("include \\masm32\\include\\masm32rt.inc\n");
    assembler.append("include \\masm32\\include\\msvcrt.inc\n");
    assembler.append("include \\masm32\\macros\\macros.asm\n");
    assembler.append("includelib \\masm32\\lib\\masm32.lib\n");
    assembler.append("includelib \\masm32\\lib\\msvcrt.lib\n");

    assembler.append(".data\n");
    assembler.append("typeD DB \"%d\",0\n");
    assembler.append("endStr DB 13,10,0\n");

    buffer.append(".code\n");

    buffer.append("start:\n");

    buffer.append("call main_f\n");
    buffer.append("inkey\n");
    buffer.append("exit\n");

    foreach(TreeElm *root,program)
    {
        imax = 1;
        imetka = 1;
        buffer.append("\n;------------------FUNCTION : "+program.key(root)+"\n");
        buffer.append(program.key(root) + "_f proc\n");

        buffer.append("push EBP\n");
        buffer.append("mov EBP, ESP\n\n");

        int countLocal = tablesId.value(program.key(root)).count();

        buffer.append("sub ESP, "+QString::number(countLocal*4)+"\n");

        int countParam = tableFunc.value(program.key(root)).param.count();
        int num;
        for(int i=0;i<countParam;i++)
        {
            num = countParam-i;
            num = num*4+4;
            buffer.append("mov EAX, [EBP+"+QString::number(num)+"]\n");
            QString name = tableFunc.value(program.key(root)).param.at(i);
            int shift = tablesId.value(program.key(root)).value(name).shift;
            buffer.append("mov [EBP-"+QString::number(shift)+"], EAX\n\n");
        }

        TreeElm *stmt = root;
        while(stmt!=NULL)
        {
            buildExpr(stmt->right,1,program.key(root),0);
            stmt = stmt->left;
        }

        buffer.append("exit_"+program.key(root)+":\n");
        buffer.append("add ESP, "+QString::number(countLocal*4)+"\n");
        buffer.append("\npop EBP\n");
        buffer.append("ret "+QString::number(4*tableFunc.value(program.key(root)).param.count())+"\n");

        for(int i=1;i<=imax;i++)
            assembler.append("t_"+program.key(root)+"_"+QString::number(i)+" DD ?\n");
        assembler.append("\n");
        buffer.append(program.key(root) + "_f endp\n\n");
    }

    buffer.append("end start\n");
    assembler.append(buffer);
}

int Generator::buildExpr(TreeElm *root, int i, QString func, int level)
{
    int l,r,shift;
    TreeElm *stmt;
    if(root!=NULL)
    {
        if(root->token.terminal=="int")
        {
            buildExpr(root->right,i,func,level);
        }
        if(root->token.terminal=="=")
        {
            buildExpr(root->right,i,func,level);
            buffer.append(autoTab(level)+"mov EAX, t_"+func+"_1\n");
            shift = tablesId.value(func).value(root->left->token.attribute).shift;
            buffer.append(autoTab(level)+"mov [EBP-"+QString::number(shift)+"], EAX\n");
            buffer.append("\n");
        }
        if(root->token.terminal=="+")
        {
            l = buildExpr(root->left,i+1,func,level+1);
            r = buildExpr(root->right,l+1,func,level+1);
            buffer.append(autoTab(level)+"mov EAX, t_"+func+"_"+QString::number(l)+"\n");
            buffer.append(autoTab(level)+"add EAX, t_"+func+"_"+QString::number(r)+"\n");
            buffer.append(autoTab(level)+"mov t_"+func+"_"+QString::number(i)+", EAX\n");
            buffer.append("\n");
        }
        if(root->token.terminal=="!")
        {
            r = buildExpr(root->right,i+1,func,level+1);
            buffer.append(autoTab(level)+"mov EAX, t_"+func+"_"+QString::number(r)+"\n");
            buffer.append(autoTab(level)+"cmp EAX, 0\n");
            buffer.append(autoTab(level)+"je me_"+func+"_"+QString::number(imetka)+"\n");
            buffer.append(autoTab(level)+"jne mne_"+func+"_"+QString::number(imetka)+"\n\n");

            buffer.append(autoTab(level)+"me_"+func+"_"+QString::number(imetka)+":\n");
            buffer.append(autoTab(level)+"mov t_"+func+"_"+QString::number(i)+", 1\n");
            buffer.append(autoTab(level)+"jmp exitCmp_"+func+"_"+QString::number(imetka)+"\n\n");

            buffer.append(autoTab(level)+"mne_"+func+"_"+QString::number(imetka)+":\n");
            buffer.append(autoTab(level)+"mov t_"+func+"_"+QString::number(i)+", 0\n");

            buffer.append(autoTab(level)+"exitCmp_"+func+"_"+QString::number(imetka)+":\n\n");
            imetka++;
        }
        if(root->token.terminal=="=="||root->token.terminal=="!=")
        {
            l = buildExpr(root->left,i+1,func,level+1);
            r = buildExpr(root->right,l+1,func,level+1);
            buffer.append(autoTab(level)+"mov EAX, t_"+func+"_"+QString::number(r)+"\n");
            buffer.append(autoTab(level)+"cmp EAX, t_"+func+"_"+QString::number(l)+"\n");
            buffer.append(autoTab(level)+"je me_"+func+"_"+QString::number(imetka)+"\n");
            buffer.append(autoTab(level)+"jne mne_"+func+"_"+QString::number(imetka)+"\n\n");

            buffer.append(autoTab(level)+"me_"+func+"_"+QString::number(imetka)+":\n");
            if(root->token.terminal=="==")
                buffer.append(autoTab(level)+"mov t_"+func+"_"+QString::number(i)+", 1\n");
            else
                buffer.append(autoTab(level)+"mov t_"+func+"_"+QString::number(i)+", 0\n");

            buffer.append(autoTab(level)+"jmp exitCmp_"+func+"_"+QString::number(imetka)+"\n\n");

            buffer.append(autoTab(level)+"mne_"+func+"_"+QString::number(imetka)+":\n");
            if(root->token.terminal=="==")
                buffer.append(autoTab(level)+"mov t_"+func+"_"+QString::number(i)+", 0\n");
            else
                buffer.append(autoTab(level)+"mov t_"+func+"_"+QString::number(i)+", 1\n");

            buffer.append(autoTab(level)+"exitCmp_"+func+"_"+QString::number(imetka)+":\n\n");
            imetka++;
        }
        if(root->token.terminal=="<"||root->token.terminal=="<="
                ||root->token.terminal==">"||root->token.terminal==">=")
        {
            QString jmp;
            QString jmpn;
            if(root->token.terminal=="<")
            {
                jmp = "jl";
                jmpn = "jnl";
            }
            if(root->token.terminal=="<=")
            {
                jmp = "jle";
                jmpn = "jnle";
            }
            if(root->token.terminal==">")
            {
                jmp = "jg";
                jmpn = "jng";
            }
            if(root->token.terminal==">=")
            {
                jmp = "jge";
                jmpn = "jnge";
            }
            l = buildExpr(root->left,i+1,func,level+1);
            r = buildExpr(root->right,l+1,func,level+1);
            buffer.append(autoTab(level)+"mov EAX, t_"+func+"_"+QString::number(l)+"\n");
            buffer.append(autoTab(level)+"cmp EAX, t_"+func+"_"+QString::number(r)+"\n");
            buffer.append(autoTab(level)+jmp+" me_"+func+"_"+QString::number(imetka)+"\n");
            buffer.append(autoTab(level)+jmpn+" mne_"+func+"_"+QString::number(imetka)+"\n\n");

            buffer.append(autoTab(level)+"me_"+func+"_"+QString::number(imetka)+":\n");
            buffer.append(autoTab(level)+"mov t_"+func+"_"+QString::number(i)+", 1\n");
            buffer.append(autoTab(level)+"jmp exitCmp_"+func+"_"+QString::number(imetka)+"\n\n");

            buffer.append(autoTab(level)+"mne_"+func+"_"+QString::number(imetka)+":\n");
            buffer.append(autoTab(level)+"mov t_"+func+"_"+QString::number(i)+", 0\n");

            buffer.append(autoTab(level)+"exitCmp_"+func+"_"+QString::number(imetka)+":\n\n");
            imetka++;
        }
        if(root->token.terminal=="&&")
        {
            l = buildExpr(root->left,i+1,func,level+1);
            r = buildExpr(root->right,l+1,func,level+1);
            buffer.append(autoTab(level)+"mov EAX, t_"+func+"_"+QString::number(l)+"\n");
            buffer.append(autoTab(level)+"cmp EAX, 0\n");
            buffer.append(autoTab(level)+"je me_"+func+"_"+QString::number(imetka)+"\n\n");

            buffer.append(autoTab(level)+"mov EAX, t_"+func+"_"+QString::number(r)+"\n");
            buffer.append(autoTab(level)+"cmp EAX, 0\n");
            buffer.append(autoTab(level)+"je me_"+func+"_"+QString::number(imetka)+"\n\n");

            buffer.append(autoTab(level)+"mov t_"+func+"_"+QString::number(i)+", 1\n");
            buffer.append(autoTab(level)+"jmp exitCmp_"+func+"_"+QString::number(imetka)+"\n\n");

            buffer.append(autoTab(level)+"me_"+func+"_"+QString::number(imetka)+":\n");
            buffer.append(autoTab(level)+"mov t_"+func+"_"+QString::number(i)+", 0\n");

            buffer.append(autoTab(level)+"exitCmp_"+func+"_"+QString::number(imetka)+":\n\n");
            imetka++;
        }
        if(root->token.terminal=="||")
        {
            l = buildExpr(root->left,i+1,func,level+1);
            r = buildExpr(root->right,l+1,func,level+1);
            buffer.append(autoTab(level)+"mov EAX, t_"+func+"_"+QString::number(l)+"\n");
            buffer.append(autoTab(level)+"cmp EAX, 0\n");
            buffer.append(autoTab(level)+"jne mne_"+func+"_"+QString::number(imetka)+"\n\n");

            buffer.append(autoTab(level)+"mov EAX, t_"+func+"_"+QString::number(r)+"\n");
            buffer.append(autoTab(level)+"cmp EAX, 0\n");
            buffer.append(autoTab(level)+"jne mne_"+func+"_"+QString::number(imetka)+"\n\n");

            buffer.append(autoTab(level)+"mov t_"+func+"_"+QString::number(i)+", 0\n");
            buffer.append(autoTab(level)+"jmp exitCmp_"+func+"_"+QString::number(imetka)+"\n\n");

            buffer.append(autoTab(level)+"mne_"+func+"_"+QString::number(imetka)+":\n");
            buffer.append(autoTab(level)+"mov t_"+func+"_"+QString::number(i)+", 1\n");

            buffer.append(autoTab(level)+"exitCmp_"+func+"_"+QString::number(imetka)+":\n\n");
            imetka++;
        }
        if(root->token.terminal=="-")
        {
            if(root->left!=NULL)
            {
                l = buildExpr(root->left,i+1,func,level+1);
                r = buildExpr(root->right,l+1,func,level+1);
                buffer.append(autoTab(level)+"mov EAX, t_"+func+"_"+QString::number(l)+"\n");
                buffer.append(autoTab(level)+"sub EAX, t_"+func+"_"+QString::number(r)+"\n");
                buffer.append(autoTab(level)+"mov t_"+func+"_"+QString::number(i)+", EAX\n");
                buffer.append("\n");
            }
            else
            {
                r = buildExpr(root->right,i+1,func,level+1);
                buffer.append(autoTab(level)+"mov EAX, t_"+func+"_"+QString::number(r)+"\n");
                buffer.append(autoTab(level)+"neg EAX\n");
                buffer.append(autoTab(level)+"mov t_"+func+"_"+QString::number(i)+", EAX\n");
                buffer.append("\n");
            }
        }
        if(root->token.terminal=="*")
        {
            l = buildExpr(root->left,i+1,func,level+1);
            r = buildExpr(root->right,l+1,func,level+1);
            buffer.append(autoTab(level)+"mov EAX, t_"+func+"_"+QString::number(l)+"\n");
            buffer.append(autoTab(level)+"imul EAX, t_"+func+"_"+QString::number(r)+"\n");
            buffer.append(autoTab(level)+"mov t_"+func+"_"+QString::number(i)+", EAX\n");
            buffer.append("\n");
        }
        if(root->token.terminal=="/")
        {
            l = buildExpr(root->left,i+1,func,level+1);
            r = buildExpr(root->right,l+1,func,level+1);
            buffer.append(autoTab(level)+"mov EAX, t_"+func+"_"+QString::number(l)+"\n");
            buffer.append(autoTab(level)+"mov EDX, 0\n");
            buffer.append(autoTab(level)+"idiv t_"+func+"_"+QString::number(r)+"\n");
            buffer.append(autoTab(level)+"mov t_"+func+"_"+QString::number(i)+", EAX\n");
            buffer.append("\n");
        }
        if(root->token.terminal=="%")
        {
            l = buildExpr(root->left,i+1,func,level+1);
            r = buildExpr(root->right,l+1,func,level+1);
            buffer.append(autoTab(level)+"mov EAX, t_"+func+"_"+QString::number(l)+"\n");
            buffer.append(autoTab(level)+"mov EDX, 0\n");
            buffer.append(autoTab(level)+"idiv t_"+func+"_"+QString::number(r)+"\n");
            buffer.append(autoTab(level)+"mov t_"+func+"_"+QString::number(i)+", EDX\n");
            buffer.append("\n");
        }
        if(root->token.terminal=="&")
        {
            shift = tablesId.value(func).value(root->right->token.attribute).shift;
            buffer.append(autoTab(level)+"lea EAX, [EBP-"+QString::number(shift)+"]\n");
            buffer.append(autoTab(level)+"mov t_"+func+"_"+QString::number(i)+", EAX\n");
            buffer.append("\n");
        }
        if(root->token.terminal=="if")
        {
            buildExpr(root->right,i,func,level+1);
            int curMetka = imetka;
            imetka++;
            buffer.append(autoTab(level)+"mov EAX, t_"+func+"_"+QString::number(i)+"\n");
            buffer.append(autoTab(level)+"cmp EAX, 0\n");
            buffer.append(autoTab(level)+"je me_"+func+"_"+QString::number(curMetka)+"\n");
            buffer.append(autoTab(level)+"jne mne_"+func+"_"+QString::number(curMetka)+"\n\n");

            buffer.append(autoTab(level)+"me_"+func+"_"+QString::number(curMetka)+":\n");
            stmt = root->left->left;
            while(stmt!=NULL)
            {
                buildExpr(stmt->right,i,func,level+1);
                stmt = stmt->left;
            }
            buffer.append(autoTab(level)+"jmp exitCmp_"+func+"_"+QString::number(curMetka)+"\n\n");

            buffer.append(autoTab(level)+"mne_"+func+"_"+QString::number(curMetka)+":\n");
            stmt = root->left->right;
            while(stmt!=NULL)
            {
                buildExpr(stmt->right,i,func,level+1);
                stmt = stmt->left;
            }

            buffer.append(autoTab(level)+"exitCmp_"+func+"_"+QString::number(curMetka)+":\n");
        }
        if(root->token.terminal=="while")
        {
            int curMetka = imetka;
            imetka++;
            buffer.append(autoTab(level)+"while_"+func+"_"+QString::number(curMetka)+":\n");
            buildExpr(root->right,i,func,level+1);
            buffer.append(autoTab(level)+"mov EAX, t_"+func+"_"+QString::number(i)+"\n");
            buffer.append(autoTab(level)+"cmp EAX, 0\n");
            buffer.append(autoTab(level)+"je me_"+func+"_"+QString::number(curMetka)+"\n");
            buffer.append(autoTab(level)+"jne mne_"+func+"_"+QString::number(curMetka)+"\n\n");

            buffer.append(autoTab(level)+"me_"+func+"_"+QString::number(curMetka)+":\n");
            buffer.append(autoTab(level)+"jmp exitCmp_"+func+"_"+QString::number(curMetka)+"\n\n");

            buffer.append(autoTab(level)+"mne_"+func+"_"+QString::number(curMetka)+":\n");
            stmt = root->left;
            while(stmt!=NULL)
            {
                buildExpr(stmt->right,i,func,level+1);
                stmt = stmt->left;
            }
            buffer.append(autoTab(level)+"jmp while_"+func+"_"+QString::number(curMetka)+"\n\n");

            buffer.append(autoTab(level)+"exitCmp_"+func+"_"+QString::number(curMetka)+":\n");
            imetka++;
        }
        if(root->token.terminal=="call")
        {
            if(root->token.attribute=="printf")
            {
                TreeElm *arg = root->right;
                QString temp;
                temp.append(autoTab(level)+"invoke crt_printf, ADDR ");
                while(arg!=NULL)
                {
                    if(arg->right!=NULL)
                    {
                        if(arg->right->token.terminal=="lit")
                        {
                            buildExpr(arg->right,i,func,level);
                            temp.append("Literal_"+QString::number(countLit));
                        }
                        else
                        {
                            buildExpr(arg->right,i,func,level);
                            temp.append(", t_"+func+"_"+QString::number(i));
                            i++;
                        }
                    }
                    arg = arg->left;
                }
                temp.append("\n");
                buffer.append(temp);
                buffer.append(autoTab(level)+"invoke crt_printf, ADDR endStr\n\n");
            }
            else
            if(root->token.attribute=="scanf")
            {
                TreeElm *arg = root->right;
                QString temp;
                temp.append(autoTab(level)+"invoke crt_scanf, ADDR ");
                while(arg!=NULL)
                {
                    if(arg->right->token.terminal=="lit")
                    {
                        buildExpr(arg->right,i,func,level);
                        temp.append("Literal_"+QString::number(countLit));
                    }
                    else
                    {
                        shift = tablesId.value(func).value(arg->right->right->token.attribute).shift;
                        temp.append(", ADDR [EBP-"+QString::number(shift)+"]");
                    }
                    arg = arg->left;
                }
                temp.append("\n");
                buffer.append(temp);
            }
            else
            {
                buildExpr(root->right,i+1,func,level+1);
                buffer.append(autoTab(level)+"call "+root->token.attribute+"_f\n");
                buffer.append(autoTab(level)+"mov t_"+func+"_"+QString::number(i)+", EAX\n\n");
            }
        }
        if(root->token.terminal=="arg")
        {
            if(root->right!=NULL)
            {
                buildExpr(root->right,i,func,level+1);
                buffer.append(autoTab(level)+"push t_"+func+"_"+QString::number(i)+"\n\n");
                buildExpr(root->left,i+1,func,level);
            }
        }
        if(root->token.terminal=="return")
        {
            buildExpr(root->right,i,func,level+1);
            buffer.append(autoTab(level)+"mov EAX, t_"+func+"_"+QString::number(i)+"\n");
            buffer.append(autoTab(level)+"jmp exit_"+func+"\n\n");
        }
        if(root->token.terminal=="id")
        {
            shift = tablesId.value(func).value(root->token.attribute).shift;
            buffer.append(autoTab(level)+"mov EAX, [EBP-"+QString::number(shift)+"]\n");
            buffer.append(autoTab(level)+"mov t_"+func+"_"+QString::number(i)+", EAX\n");
            buffer.append("\n");
        }
        if(root->token.terminal=="num")
        {
            buffer.append(autoTab(level)+"mov t_"+func+"_"+QString::number(i)+", "+root->token.attribute+"\n");
            buffer.append("\n");
        }
        if(root->token.terminal=="lit")
        {
            countLit++;
            assembler.append("Literal_"+QString::number(countLit)+" DB "+root->token.attribute+"\",0\n");
        }
    }
    imax = (i>imax)?i:imax;
    return i;
}

QString Generator::autoTab(int count)
{
    QString result;
    result.fill('\t',count);
    return result;
}

QString Generator::getAssembler()
{
    return assembler;
}
