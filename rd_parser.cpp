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
#define PAREN 4 // 소괄호
#define BRACE 5 // 중괄호
#define ALPHA 6 // 알파벳 소문자
#define COLON 7
#define UNKNOWN 99
// 어휘 1개의 의미
#define INT_LIT 10
#define CHAR_LIT 11
#define TYPE_LIT 12
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
#define LEFT_BRACE 33
#define RIGHT_BRACE 34
#define SEMI_COLON 35
#define ASSIGN_OP 36
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
string printStr;
int inputIdx;
int value[26];

int number()
{
    // cout<<"enter number\n";
    if (lexemes[idx].type == INT_LIT)
        return stoi(lexemes[idx++].data);
    else
        throw runtime_error("syntax error!!");
}

int var() // 변수명이 "int"일때 예외처리 해야될듯
{
    if (lexemes[idx].type == CHAR_LIT && lexemes[idx].data.size() == 1 && islower(lexemes[idx].data[0]))
        return value[lexemes[idx++].data[0] - 'a'];
    else
        throw runtime_error("syntax error!!");
}

int term() // number 또는 변수를 return하게 고쳐야 함
{
    // cout<<"enter number\n";
    if (lexemes[idx].type == INT_LIT)
        return number();
    return var();
}

int aexpr()
{
    // cout<<"enter aexpr\n";
    int l = term();
    while (1)
    {
        if (lexemes[idx].type == ADD_OP || lexemes[idx].type == SUB_OP)
        {
            int tmp = lexemes[idx].type;
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
    // cout<<"enter bexpr\n";
    int l = aexpr();
    if (lexemes[idx].type >= EQUAL_OP && lexemes[idx].type <= LESS_EQUAL_OP)
    {
        int tmp = lexemes[idx].type;
        idx++;
        isBinary = true;
        int r = aexpr();
        switch (tmp)
        {
        case EQUAL_OP:
            return l == r;
        case NOT_EQUAL_OP:
            return l != r;
        case GREATER_OP:
            return l > r;
        case LESS_OP:
            return l < r;
        case GREATER_EQUAL_OP:
            return l >= r;
        case LESS_EQUAL_OP:
            return l <= r;
        }
    }
    throw runtime_error("syntax error!!");
}

void input()
{
    int i = 0;
    cout << ">> ";
    getline(cin, inputStr);
    inputStr += '\n';
    inputIdx = 0;
    getChar();
    do
    {
        lex();
        lexemes[i].type = nextToken;
        lexemes[i].data = lexeme;
        i++;
    } while (nextToken != EOF && nextToken != UNKNOWN_OP);
}

int statement()
{
    if (lexemes[idx].type != CHAR_LIT)
    {
        throw runtime_error("syntax error!!");
    }

    if (lexemes[idx].data == "print")
    {
        idx++;
        int ret = aexpr();
        printStr += ">> " + to_string(ret)+ "\n";
        //cout << ">> " << ret << '\n';
        return 0;
    }
    else if (lexemes[idx].data == "while")
    {
        idx++;
        if (lexemes[idx++].type != LEFT_PAREN)
            throw runtime_error("syntax error!!");
        int tmp = idx;
        bool whilevalid = false;
        int valTmp[26];
        string strTmp;
        for (int i = 0; i < 26; i++)
            valTmp[i] = value[i];
        strTmp = printStr;
        while (bexpr()||!whilevalid)
        {
            //cout<<"while start\n";
            if (lexemes[idx++].type != RIGHT_PAREN)
                throw runtime_error("syntax error!!");
            if (lexemes[idx++].type != LEFT_BRACE)
                throw runtime_error("syntax error!!");
            while (lexemes[idx].type != RIGHT_BRACE)
            {
                int stype = statement();
                //cout<<"in "<<idx<<":"<<lexemes[idx].data<<'\n';
                if ((!stype && lexemes[idx++].type != SEMI_COLON) || (stype && lexemes[idx++].type != RIGHT_BRACE))
                    throw runtime_error("syntax error!!");
            }
            idx = tmp;
            //cout<<"while end\n";
            if (!whilevalid)
            {
                bool isInfLoop = true;
                for (int i = 0; i < 26; i++)
                {
                    if (value[i] != valTmp[i])
                        isInfLoop = false;
                    value[i] = valTmp[i];
                }
                if (isInfLoop)
                    throw runtime_error("this is infinity loop!!");
                printStr = strTmp;
                whilevalid = true;
            }
        }
        int chk = 0;
        //cout<<"chk "<<idx<<":"<<lexemes[idx].data<<'\n';
        if (lexemes[idx++].type != RIGHT_PAREN)
            throw runtime_error("syntax error!!");
        if (lexemes[idx++].type != LEFT_BRACE)
            throw runtime_error("syntax error!!");
        while (chk != 1)
        {
            //cout<<"chk "<<idx<<":"<<lexemes[idx].data<<'\n';
            if (lexemes[idx].type == EOF)
                throw runtime_error("syntax error!!");
            if (lexemes[idx].type == RIGHT_BRACE)
                chk++;
            if (lexemes[idx].type == LEFT_BRACE)
                chk--;
            idx++;
        }
        idx--;
        return 1;
    }
    else if (lexemes[idx].data.size() == 1 && islower(lexemes[idx].data[0]))
    {
        int tmp = lexemes[idx++].data[0];
        if (lexemes[idx++].type != ASSIGN_OP)
            throw runtime_error("syntax error!!");
        value[tmp - 'a'] = aexpr();
        return 0;
    }
    else
        throw runtime_error("syntax error!!");
}

bool endEnter = false;

int main()
{
    while (1)
    {
        input();
        printStr = "";
        idx = 0;
        memset(value, 0, sizeof(value));
        if (lexemes[0].type == EOF)
        {
            if (!endEnter)
            {
                endEnter = true;
                continue;
            }
            else
                break;
        }
        endEnter = false;
        try
        {
            while (lexemes[idx].type == TYPE_LIT)
            {
                idx++;
                var();//딱히 해줄거 없음 인덱스 증가만 시켜주고 예외처리만 해주면됨.
                if (lexemes[idx++].type != SEMI_COLON)
                    throw runtime_error("syntax error!!");
                //예외처리 말고 딱히 해줄거 없는듯?
            }
            while (lexemes[idx].type != EOF)
            {
                int stype = statement();
                //cout<<idx<<":"<<lexemes[idx].data<<'\n';
                if ((!stype && lexemes[idx++].type != SEMI_COLON) || (stype && lexemes[idx++].type != RIGHT_BRACE))
                    throw runtime_error("syntax error!!");
            }
            cout << printStr;
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
        else if (islower(nextChar))
            charClass = ALPHA;
        else if (nextChar == '=')
            charClass = EQUAL;
        else if (nextChar == '!' || nextChar == '>' || nextChar == '<')
            charClass = LOGIC;
        else if (nextChar == '*' || nextChar == '/' || nextChar == '+' || nextChar == '-')
            charClass = ARITH;
        else if (nextChar == '(' || nextChar == ')')
            charClass = PAREN;
        else if (nextChar == '{' || nextChar == '}')
            charClass = BRACE;
        else if (nextChar == ';')
            charClass = COLON;
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
    if (!strcmp(lexeme, "="))
        return ASSIGN_OP;
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
    if (!strcmp(lexeme, "{"))
        return LEFT_BRACE;
    if (!strcmp(lexeme, "}"))
        return RIGHT_BRACE;
    if (!strcmp(lexeme, ";"))
        return SEMI_COLON;
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
    case ALPHA:
        addChar();
        getChar();
        while (charClass == ALPHA)
        {
            addChar();
            getChar();
        }
        if (!strcmp(lexeme, "int"))
            nextToken = TYPE_LIT;
        else
            nextToken = CHAR_LIT;
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
    case BRACE:
    case COLON:
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