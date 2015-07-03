#ifndef LEX_H
#define LEX_H

#include <QList>
#include <QSet>
#include <QMap>
#include <QDebug>
#include <QStack>

#define COUNT_STATE 20
#define COUNT_SYMBOL 22

enum Statement{S1,S2,S3,S301,S4,S401,S402,S403,S404,S405,S406,S407,S408,S409,S410,S411,S412,S413,S5,S6};

enum Signal{O,O1,O2,O3,O4,O5,O6};

struct Token
{
    QString terminal;
    QString attribute;
};

typedef QPair<int,QString> Error;
typedef QPair<QChar,Statement> Input;
typedef QPair<Signal,Statement> Output;

class Lex
{
public:
    Lex();
    ~Lex();
    void analisys(QString);
    bool isEmpty();

    void saveStatement();
    void rollBack();

    int getCurrent();
    QList<Token> getTokens();
    QList<Error> getError();
    bool getToken(Token &value);

private:
    QSet<QChar> letters;
    QSet<QChar> numerals;
    QSet<QChar> signs;
    QSet<QChar> null;

    QSet<QString> dsigns;
    QSet<QString> keywords;

    QMap<Input,Output> tableState;

    QList<Error> errors;
    QList<Token> tokens;
    int cur;
    int save;
};

#endif // LEX_H
