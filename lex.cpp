#include "lex.h"

Lex::Lex()
{
    cur = 0;

    letters << 'q' << 'w' << 'e' << 'r' << 't' << 'y';
    letters << 'u' << 'i' << 'o' << 'p' << 'a' << 's';
    letters << 'd' << 'f' << 'g' << 'h' << 'j' << 'k';
    letters << 'z' << 'x' << 'c' << 'v' << 'b' << 'n';
    letters << 'm' << 'l';

    letters << 'Q' << 'W' << 'E' << 'R' << 'T' << 'Y';
    letters << 'U' << 'I' << 'O' << 'P' << 'A' << 'S';
    letters << 'D' << 'F' << 'G' << 'H' << 'J' << 'K';
    letters << 'Z' << 'X' << 'C' << 'V' << 'B' << 'N';
    letters << 'M' << 'L' << '_';

    numerals << '1' << '2' << '3' << '4' << '5' << '6' << '7' << '8' << '9';

    signs << '+' << '-' << '/' << '*' << '&' << '|' << '=' << '<' << '%';
    signs << ';' << ',' << '}' << '{' << '(' << ')'  << '>' << '!' << '\"';

    null << ' ' << '\n' <<'\t' << '\0';

    dsigns << "&&" << "||" << "<=" << ">=" << "==" << "!=";

    keywords << "if" << "else" << "while" << "return" << "int" << "char";

    Output out[COUNT_STATE][COUNT_SYMBOL] =
    {
        //Statement - S1 Space
        {Output(O1,S1),     Output(O1,S2),      Output(O1,S3),      Output(O1,S301),
         Output(O1,S4),     Output(O1,S4),      Output(O1,S4),      Output(O1,S4),
         Output(O1,S401),   Output(O1,S402),    Output(O1,S403),    Output(O1,S404),
         Output(O1,S405),   Output(O1,S406),    Output(O1,S407),    Output(O1,S408),
         Output(O1,S409),   Output(O1,S410),    Output(O1,S411),    Output(O1,S412),
         Output(O1,S413),   Output(O1,S6)
        },
        //Statement - S2 'a-Z'
        {Output(O2,S1),     Output(O1,S2),      Output(O1,S2),      Output(O1,S2),
         Output(O2,S4),     Output(O2,S4),      Output(O2,S4),      Output(O2,S4),
         Output(O2,S401),   Output(O2,S402),    Output(O2,S403),    Output(O2,S404),
         Output(O2,S405),   Output(O2,S406),    Output(O2,S407),    Output(O2,S408),
         Output(O,S409),    Output(O,S410),     Output(O2,S411),    Output(O2,S412),
         Output(O2,S413),   Output(O,S6)
        },
        //Statement - S3 '1-9'
        {Output(O3,S1),     Output(O,S2),       Output(O1,S3),      Output(O1,S3),
         Output(O3,S4),     Output(O3,S4),      Output(O3,S4),      Output(O3,S4),
         Output(O3,S401),   Output(O3,S402),    Output(O3,S403),    Output(O3,S404),
         Output(O3,S405),   Output(O3,S406),    Output(O3,S407),    Output(O3,S408),
         Output(O,S409),    Output(O,S410),     Output(O3,S411),    Output(O,S412),
         Output(O3,S413),   Output(O,S6)
        },
        //Statement - S301 '0'
        {Output(O3,S1),     Output(O,S2),       Output(O,S2),       Output(O,S2),
         Output(O3,S4),     Output(O3,S4),      Output(O3,S4),      Output(O3,S4),
         Output(O3,S401),   Output(O3,S402),    Output(O3,S403),    Output(O3,S404),
         Output(O3,S405),   Output(O,S406),     Output(O3,S407),    Output(O3,S408),
         Output(O,S409),    Output(O,S410),     Output(O3,S411),    Output(O2,S412),
         Output(O3,S413),   Output(O,S6)
        },
        //Statement - S4 '+,-,*,/'
        {Output(O4,S1),     Output(O4,S2),      Output(O4,S3),      Output(O4,S301),
         Output(O,S4),      Output(O4,S4),      Output(O,S4),       Output(O,S4),
         Output(O,S401),    Output(O,S402),     Output(O,S403),     Output(O,S404),
         Output(O,S405),    Output(O,S406),     Output(O,S407),     Output(O,S408),
         Output(O,S409),    Output(O,S410),     Output(O,S411),     Output(O4,S412),
         Output(O,S413),    Output(O,S6)
        },
        //Statement - S401 '&'
        {Output(O4,S1),     Output(O4,S2),      Output(O4,S3),      Output(O4,S301),
         Output(O,S4),      Output(O,S4),       Output(O,S4),       Output(O,S4),
         Output(O1,S5),     Output(O,S402),     Output(O,S403),     Output(O,S404),
         Output(O,S405),    Output(O4,S406),    Output(O,S407),     Output(O,S408),
         Output(O,S409),    Output(O,S410),     Output(O,S411),     Output(O,S412),
         Output(O,S413),    Output(O,S6)
        },
        //Statement - S402 '|'
        {Output(O4,S1),     Output(O4,S2),      Output(O4,S3),      Output(O4,S301),
         Output(O,S4),      Output(O,S4),       Output(O,S4),       Output(O,S4),
         Output(O,S401),    Output(O1,S5),      Output(O,S403),     Output(O,S404),
         Output(O,S405),    Output(O4,S406),    Output(O,S407),     Output(O,S408),
         Output(O,S409),    Output(O,S410),     Output(O,S411),     Output(O4,S412),
         Output(O,S413),    Output(O,S6)
        },
        //Statement - S4O3 '='
        {Output(O4,S1),     Output(O4,S2),      Output(O4,S3),      Output(O4,S301),
         Output(O,S4),      Output(O4,S4),      Output(O,S4),       Output(O,S4),
         Output(O,S401),    Output(O,S402),     Output(O1,S5),      Output(O,S404),
         Output(O,S405),    Output(O,S406),     Output(O,S407),     Output(O,S408),
         Output(O,S409),    Output(O,S410),     Output(O,S411),     Output(O4,S412),
         Output(O,S413),    Output(O,S6)
        },
        //Statement - S404 '<'
        {Output(O4,S1),     Output(O4,S2),      Output(O4,S3),      Output(O4,S301),
         Output(O,S4),      Output(O4,S4),      Output(O,S4),       Output(O,S4),
         Output(O,S401),    Output(O,S402),     Output(O1,S5),      Output(O,S404),
         Output(O,S405),    Output(O,S406),     Output(O,S407),     Output(O,S408),
         Output(O,S409),    Output(O,S410),     Output(O,S411),     Output(O4,S412),
         Output(O,S413),    Output(O,S6)
        },
        //Statement - S405 '>'
        {Output(O4,S1),     Output(O4,S2),      Output(O4,S3),      Output(O4,S301),
         Output(O,S4),      Output(O4,S4),      Output(O,S4),       Output(O,S4),
         Output(O,S401),    Output(O,S402),     Output(O1,S5),      Output(O,S404),
         Output(O,S405),    Output(O,S406),     Output(O,S407),     Output(O,S408),
         Output(O,S409),    Output(O,S410),     Output(O,S411),     Output(O4,S412),
         Output(O,S413),    Output(O,S6)
        },
        //Statement - S406 '!'
        {Output(O4,S1),     Output(O4,S2),      Output(O4,S3),      Output(O4,S301),
         Output(O,S4),      Output(O,S4),       Output(O,S4),       Output(O,S4),
         Output(O,S401),    Output(O,S402),     Output(O1,S5),      Output(O,S404),
         Output(O,S405),    Output(O,S406),     Output(O,S407),     Output(O,S408),
         Output(O,S409),    Output(O,S410),     Output(O,S411),     Output(O4,S412),
         Output(O,S413),    Output(O,S6)
        },
        //Statement - S407 ';'
        {Output(O4,S1),     Output(O4,S2),      Output(O4,S3),      Output(O4,S301),
         Output(O,S4),      Output(O,S4),       Output(O,S4),       Output(O,S4),
         Output(O,S401),    Output(O,S402),     Output(O,S403),     Output(O,S404),
         Output(O,S405),    Output(O,S406),     Output(O,S407),     Output(O,S408),
         Output(O,S409),    Output(O,S410),     Output(O,S411),     Output(O,S412),
         Output(O,S413),    Output(O,S6)
        },
        //Statement - S408 ','
        {Output(O4,S1),     Output(O4,S2),      Output(O4,S3),      Output(O4,S301),
         Output(O,S4),      Output(O,S4),       Output(O,S4),       Output(O,S4),
         Output(O4,S401),    Output(O,S402),     Output(O,S403),     Output(O,S404),
         Output(O,S405),    Output(O,S406),     Output(O,S407),     Output(O,S408),
         Output(O,S409),    Output(O,S410),     Output(O,S411),     Output(O,S412),
         Output(O,S413),    Output(O4,S6)
        },
        //Statement - S409 '}'
        {Output(O4,S1),     Output(O4,S2),      Output(O4,S3),      Output(O4,S301),
         Output(O,S4),      Output(O,S4),       Output(O,S4),       Output(O,S4),
         Output(O,S401),    Output(O,S402),     Output(O,S403),     Output(O,S404),
         Output(O,S405),    Output(O,S406),     Output(O,S407),     Output(O,S408),
         Output(O,S409),    Output(O,S410),     Output(O,S411),     Output(O,S412),
         Output(O,S413),    Output(O,S6)
        },
        //Statement - S410 '{'
        {Output(O4,S1),     Output(O4,S2),      Output(O4,S3),      Output(O4,S301),
         Output(O,S4),      Output(O,S4),       Output(O,S4),       Output(O,S4),
         Output(O,S401),    Output(O,S402),     Output(O,S403),     Output(O,S404),
         Output(O,S405),    Output(O,S406),     Output(O,S407),     Output(O,S408),
         Output(O,S409),    Output(O,S410),     Output(O,S411),     Output(O,S412),
         Output(O,S413),    Output(O,S6)
        },
        //Statement - S411 ')'
        {Output(O4,S1),     Output(O4,S2),      Output(O4,S3),      Output(O4,S301),
         Output(O4,S4),     Output(O4,S4),      Output(O4,S4),      Output(O4,S4),
         Output(O4,S401),   Output(O4,S402),    Output(O4,S403),    Output(O4,S404),
         Output(O4,S405),   Output(O4,S406),    Output(O4,S407),    Output(O4,S408),
         Output(O,S409),    Output(O,S410),     Output(O4,S411),    Output(O,S412),
         Output(O4,S413),   Output(O,S6)
        },
        //Statement - S412 '('
        {Output(O4,S1),     Output(O4,S2),      Output(O4,S3),      Output(O4,S301),
         Output(O,S4),      Output(O4,S4),      Output(O,S4),       Output(O,S4),
         Output(O,S401),    Output(O,S402),     Output(O,S403),     Output(O,S404),
         Output(O,S405),    Output(O4,S406),    Output(O,S407),     Output(O,S408),
         Output(O,S409),    Output(O,S410),     Output(O4,S411),    Output(O4,S412),
         Output(O,S413),    Output(O4,S6)
        },
        //Statement - S413 '%'
        {Output(O4,S1),     Output(O4,S2),      Output(O4,S3),      Output(O4,S301),
         Output(O,S4),      Output(O4,S4),      Output(O,S4),       Output(O,S4),
         Output(O,S401),    Output(O,S402),     Output(O,S403),     Output(O,S404),
         Output(O,S405),    Output(O,S406),     Output(O,S407),     Output(O,S408),
         Output(O,S409),    Output(O,S410),     Output(O,S411),     Output(O4,S412),
         Output(O,S413),    Output(O,S6)
        },
        //Statement - S5 Dsign
        {Output(O5,S1),     Output(O5,S2),      Output(O5,S3),      Output(O5,S301),
         Output(O,S4),      Output(O5,S4),      Output(O,S4),       Output(O,S4),
         Output(O,S401),    Output(O,S402),     Output(O,S403),     Output(O,S404),
         Output(O,S405),    Output(O5,S406),    Output(O,S407),     Output(O,S408),
         Output(O,S409),    Output(O,S410),     Output(O,S411),     Output(O5,S412),
         Output(O,S413),    Output(O,S6)
        },
        //Statement - S6 Literal
        {Output(O1,S6),     Output(O1,S6),      Output(O1,S6),      Output(O1,S6),
         Output(O1,S6),     Output(O1,S6),      Output(O1,S6),      Output(O1,S6),
         Output(O1,S6),     Output(O1,S6),      Output(O1,S6),      Output(O1,S6),
         Output(O1,S6),     Output(O1,S6),      Output(O1,S6),      Output(O1,S6),
         Output(O1,S6),     Output(O1,S6),      Output(O1,S6),      Output(O1,S6),
         Output(O1,S6),     Output(O6,S1)
        },
    };
    for(int i=0;i<COUNT_STATE;i++)
    {
        foreach(QChar nul,null)
            tableState.insert(Input(nul,Statement(i)),out[i][0]);
        foreach(QChar sym,letters)
            tableState.insert(Input(sym,Statement(i)),out[i][1]);
        foreach(QChar num,numerals)
            tableState.insert(Input(num,Statement(i)),out[i][2]);
        tableState.insert(Input('0',Statement(i)),out[i][3]);
        tableState.insert(Input('+',Statement(i)),out[i][4]);
        tableState.insert(Input('-',Statement(i)),out[i][5]);
        tableState.insert(Input('*',Statement(i)),out[i][6]);
        tableState.insert(Input('/',Statement(i)),out[i][7]);
        tableState.insert(Input('&',Statement(i)),out[i][8]);
        tableState.insert(Input('|',Statement(i)),out[i][9]);
        tableState.insert(Input('=',Statement(i)),out[i][10]);
        tableState.insert(Input('<',Statement(i)),out[i][11]);
        tableState.insert(Input('>',Statement(i)),out[i][12]);
        tableState.insert(Input('!',Statement(i)),out[i][13]);
        tableState.insert(Input(';',Statement(i)),out[i][14]);
        tableState.insert(Input(',',Statement(i)),out[i][15]);
        tableState.insert(Input('}',Statement(i)),out[i][16]);
        tableState.insert(Input('{',Statement(i)),out[i][17]);
        tableState.insert(Input(')',Statement(i)),out[i][18]);
        tableState.insert(Input('(',Statement(i)),out[i][19]);
        tableState.insert(Input('%',Statement(i)),out[i][20]);
        tableState.insert(Input('\"',Statement(i)),out[i][21]);
    }
}

Lex::~Lex()
{

}

void Lex::analisys(QString program)
{
    int line = 1;
    Token token;
    QString lexeme;
    Input in;
    Output out;
    Statement curState = S1;
    foreach(QChar sym,program)
    {
        if(sym=='\n')
            line++;

        if(!letters.contains(sym)&&!numerals.contains(sym)&&!signs.contains(sym)&&!null.contains(sym)&&sym!='0')
        {
            errors.append(Error(line,"неизвестный символ '"+QString(sym)+"'"));
            continue;
        }

        token.terminal.clear();
        token.attribute.clear();

        in.first = sym;
        in.second = curState;

        out = tableState.value(in);

        curState = out.second;
        switch(out.first)
        {
        case O2:
            if(keywords.contains(lexeme))
                token.terminal = lexeme;
            else
            {
                token.terminal = "id";
                token.attribute = lexeme;
            }
            tokens.append(token);
            lexeme.clear();
            break;
        case O3:
            token.terminal = "num";
            token.attribute = lexeme;
            tokens.append(token);
            lexeme.clear();
            break;
        case O4:
            token.terminal = lexeme;
            tokens.append(token);
            lexeme.clear();
            break;
        case O5:
            token.terminal = lexeme;
            tokens.append(token);
            lexeme.clear();
            break;
        case O6:
            token.attribute = lexeme;
            token.terminal = "lit";
            tokens.append(token);
            lexeme.clear();
            break;
        case O:
            errors.append(Error(line,"неизвестное выражение '"+lexeme+QString(sym)+"'"));
            break;
        }
        if(out.second!=S1&&out.first!=O)
            lexeme.append(sym);
    }
}

QList<Token> Lex::getTokens()
{
    return tokens;
}

QList<Error> Lex::getError()
{
    return errors;
}

bool Lex::getToken(Token &value)
{
    if(cur<tokens.length())
    {
        value = tokens.at(cur);
        cur++;
        return true;
    }
    else
        return false;
}

bool Lex::isEmpty()
{
    return cur==tokens.length();
}

void Lex::saveStatement()
{
    save = cur;
}

void Lex::rollBack()
{
    cur = save;
}

int Lex::getCurrent()
{
    return cur;
}
