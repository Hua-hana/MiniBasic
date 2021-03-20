#include<stdlib.h>
#include<string>
#include"parse.h"

using namespace std;

//curent pointer
static unsigned int pcur=0;

//code text
extern string code_text;

//token attribute
extern Token_attribute token_attr;
static unsigned int len=code_text.length();
tokentype number_scanner();

void skip_blank();

string word_scanner();

bool is_blank(const char & c){
    return  c=='\n'||c=='\t'||c=='\b'||c=='\f';
}



int code_scanner(){
    skip_blank();
    if(pcur>=len){
        token_attr.error_msg="reaching the end of code";
        return ERROR;
    }
    char c=code_text[pcur];

    if(c>='0'&&c<='9')return number_scanner();
    if(c=='+'||'*'||'-'||'/'||'('||')'||'='){
        if(c=='*'&&pcur+1<len&&code_text[pcur+1]=='*'){
            pcur+=2;
            return EXPO;
        }
        pcur++;
        return (int)c;
    }

    string word=word_scanner();
    if(word=="REM")return REM;
    if(word=="LET")return LET;
    if(word=="PRINT")return PRINT;
    if(word=="INPUT")return INPUT;
    if(word=="GOTO")return GOTO;
    if(word=="IF")return IF;
    if(word=="THEN")return THEN;
    if(word=="END")return END;

    //FIXME identified must have rule!
    token_attr.id=word;
    return ID;

}

tokentype number_scanner(){
    int low=pcur;
    while(code_text[pcur]<='9' && code_text[pcur]>='0')++pcur;
    token_attr.num=atoi(code_text.substr(low,pcur-low).c_str());
    return (tokentype)NUM;
}

void skip_blank(){
    while(pcur<len&&is_blank(code_text[pcur]))
        ++pcur;
}

string word_scanner(){
    int low=pcur;
    //FIXME it allow any variable!
    while(pcur<len&&!is_blank(code_text[pcur]))++pcur;
    return code_text.substr(low,pcur);
}
