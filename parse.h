#ifndef PARSE_H
#define PARSE_H

#include<string>
#include"program.h"
#include"statement.h"
#include"tokenizer.h"
using namespace std;
enum tokentype{
    REM=258,
    LET=259,
    PRINT=260,
    INPUT=261,
    GOTO=262,
    IF=263,
    THEN=264,
    END=265,
    NUM=266,
    ID=267,
    ERROR=268,
    EXPO=269
};

#define REM 258
#define LET 259
#define PRINT 260
#define INPUT 261
#define GOTO 262
#define IF 263
#define THEN 264
#define END 265
#define NUM 266
#define ID 267
#define ERROR 268
#define EXPO 269
typedef struct Token_attribute{
    int num;
    string id;
    string error_msg;
    string comment;
} Token_attribute;

extern Token_attribute token_attr;
#ifndef CODE_TEXT
#define CODE_TEXT
extern string code_text;
#endif
void parse();

#endif // PARSE_H
