#include <cstdio>
#include <cctype>
#include <cstring>
#include <string>
#include <iostream>

using namespace std;

// lexeme analysis
int charClass;
int nextToken;
int lexLen;
char nextChar;
char lexeme[1000];
void lex();
void getNonBlank();
int lookup();
void addChar();
void getChar();
int term();
// 문자 1개의 의미
#define DIGIT 0
#define LOGIC 1
#define ARITH 2
#define EQUAL 3
#define PAREN 4
#define UNKNOWN 99
// 어휘 1개의 의미
#define INT_LIT 10
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define LEFT_PAREN 25
#define RIGHT_PAREN 26
#define EQUAL_OP 27
#define NOT_EQUAL_OP 28
#define GREATER_OP 29
#define LESS_OP 30
#define GREATER_EQUAL_OP 31
#define LESS_EQUAL_OP 32
#define UNKNOWN_OP 98

// syntx analysis
struct LEXEME
{
    int type;
    string data;
};
int factor();
int term();
int aexpr();
int bexpr();
int number();
string expr();
bool isBinary;
int idx;
LEXEME lexemes[1000];
string inputStr;
int inputIdx;

int number()
{
    //cout<<"enter number\n";
    if (lexemes[idx].type == INT_LIT)
        return stoi(lexemes[idx++].data);
    else
        throw runtime_error("syntax error!!");
}

int term()//number 또는 변수를 return하게 고쳐야 함
{
    //cout<<"enter number\n";
    return number();
}

int aexpr()
{
    //cout<<"enter aexpr\n";
    int l = term();
    while (1)
    {
        if (lexemes[idx].type == ADD_OP || lexemes[idx].type == SUB_OP)
        {
            int tmp=lexemes[idx].type;
            idx++;
            int r = term();
            if (tmp == ADD_OP)
                l += r;
            else
                l -= r;
        }
        else
            break;
    }
    return l;
}

int bexpr()
{
    //cout<<"enter bexpr\n";
    int l = aexpr();
    if (lexemes[idx].type>=EQUAL_OP&&lexemes[idx].type<=LESS_EQUAL_OP)
    {
        int tmp=lexemes[idx].type;
        idx++;
        isBinary=true;
        int r=aexpr();
        switch(tmp)
        {
            case EQUAL_OP:
                return l==r;
            case NOT_EQUAL_OP:
                return l!=r;
            case GREATER_OP:
                return l>r;
            case LESS_OP:
                return l<r;
            case GREATER_EQUAL_OP:
                return l>=r;
            case LESS_EQUAL_OP:
                return l<=r;
        }
    }
    return l;
}

string expr()
{
    //cout<<"enter expr\n";
    isBinary = false;
    idx = 0;
    int res = bexpr();
    if (isBinary)
    {
        if (res)
            return "true";
        else
            return "false";
    }
    else
        return to_string(res);
}

void input()
{
    int i = 0;
    cout << ">> ";
    getline(cin, inputStr);
    inputStr+='\n';
    inputIdx=0;
    getChar();
    do
    {
        lex();
        lexemes[i].type = nextToken;
        lexemes[i].data = lexeme;
        i++;
    } while (nextToken != EOF && nextToken != UNKNOWN_OP);
}

int main()
{
    while (1)
    {
        input();
        // int i=0;
        // while(lexemes[i].type!=EOF&&lexemes[i].type!=UNKNOWN_OP)
        // {
        //     cout<<"type: "<<lexemes[i].type<<"   data: "<<lexemes[i].data<<'\n';
        //     i++;
        // }
        if (lexemes[0].type == EOF)
            break;
        try
        {
            string res = expr();
            if(lexemes[idx].type!=EOF)
                throw runtime_error("syntax error!!");
            cout << ">> " << res << '\n';
        }
        catch (runtime_error e)
        {
            cout << ">> " << e.what() << '\n';
        }
    }
}

void getNonBlank()
{
    while (isspace(nextChar) && nextChar != '\n')
        getChar();
}

void addChar()
{
    lexeme[lexLen++] = nextChar;
    lexeme[lexLen] = 0;
}

void getChar()
{
    if ((nextChar = inputStr[inputIdx++]) != '\n')
    {
        if (isdigit(nextChar))
            charClass = DIGIT;
        else if (nextChar == '=')
            charClass = EQUAL;
        else if (nextChar == '!' || nextChar == '>' || nextChar == '<')
            charClass = LOGIC;
        else if (nextChar == '*' || nextChar == '/' || nextChar == '+' || nextChar == '-')
            charClass = ARITH;
        else if (nextChar == '(' || nextChar == ')')
            charClass = PAREN;
        else
            charClass = UNKNOWN;
    }
    else
        charClass = EOF;
}

int lookup()
{
    if (!strcmp(lexeme, "+"))
        return ADD_OP;
    if (!strcmp(lexeme, "-"))
        return SUB_OP;
    if (!strcmp(lexeme, "/"))
        return DIV_OP;
    if (!strcmp(lexeme, "*"))
        return MULT_OP;
    if (!strcmp(lexeme, "=="))
        return EQUAL_OP;
    if (!strcmp(lexeme, "!="))
        return NOT_EQUAL_OP;
    if (!strcmp(lexeme, ">"))
        return GREATER_OP;
    if (!strcmp(lexeme, ">="))
        return GREATER_EQUAL_OP;
    if (!strcmp(lexeme, "<"))
        return LESS_OP;
    if (!strcmp(lexeme, "<="))
        return LESS_EQUAL_OP;
    if (!strcmp(lexeme, "("))
        return LEFT_PAREN;
    if (!strcmp(lexeme, ")"))
        return RIGHT_PAREN;
    return UNKNOWN_OP;
}

void lex()
{
    lexLen = 0;
    getNonBlank();
    switch (charClass)
    {
    case DIGIT:
        addChar();
        getChar();
        while (charClass == DIGIT)
        {
            addChar();
            getChar();
        }
        nextToken = INT_LIT;
        break;
    case LOGIC:
    case EQUAL:
        addChar();
        getChar();
        while (charClass == EQUAL)
        {
            addChar();
            getChar();
        }
        nextToken = lookup();
        break;
    case ARITH:
    case PAREN:
        addChar();
        getChar();
        nextToken = lookup();
        break;
    case UNKNOWN:
        addChar();
        getChar();
        nextToken = UNKNOWN_OP;
        break;
    case EOF:
        lexeme[0] = 'E';
        lexeme[1] = 'O';
        lexeme[2] = 'F';
        lexeme[3] = 0;
        nextToken = EOF;
    }
}