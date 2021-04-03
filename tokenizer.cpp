#include<stdlib.h>
#include<string>
#include"parse.h"
#include"exception.h"
using namespace std;

//curent pointer
unsigned int pcur=0;

//code text
extern string code_text;

//token attribute
Token_attribute token_attr;
unsigned int len;
tokentype number_scanner();

void skip_blank();

string word_scanner();

class Parse_Exception;

bool is_blank(const char & c){
    return  c==' '||c=='\n'||c=='\t';
}
bool is_letter(char c){
    return (c>='a'&&c<='z')||(c>='A'&&c<'Z')||c=='_';
}
bool is_digit(char c){
    return (c>='0'&&c<='9');
}


int code_scanner(){
    len=code_text.length();
    skip_blank();
    if(pcur>=len){
        //end of code
        return 0;
    }
    char c=code_text[pcur];

    if(c>='0'&&c<='9')return number_scanner();
    if(c=='+'||c=='*'||c=='-'||c=='/'||c=='('||c==')'||c=='='||c=='>'||c=='<'){
        if(c=='*'&&pcur+1<len&&code_text[pcur+1]=='*'){
            pcur+=2;
            return EXPO;
        }
        pcur++;
        return (int)c;
    }

    string word=word_scanner();
    if(word=="REM"){
        int low=pcur;
        while(pcur<len&&code_text[pcur]!='\n')++pcur;
        //careful for the deconstruct!
        token_attr.comment=code_text.substr(low,pcur-low);
        return REM;
    }
    if(word=="LET")return LET;
    if(word=="PRINT")return PRINT;
    if(word=="INPUT")return INPUT;
    if(word=="GOTO")return GOTO;
    if(word=="IF")return IF;
    if(word=="THEN")return THEN;
    if(word=="END")return END;
    else {
        //FIXME identified must have rule!
        token_attr.id=word;
        return ID;
    }

    token_attr.error_msg="unknown error";
    return ERROR;

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
    while(pcur<len&&(is_letter(code_text[pcur])||is_digit(code_text[pcur])))++pcur;
    return code_text.substr(low,pcur-low);
}

int lookahead1(){
    int pcur_tmp=pcur;
    if(!(pcur<len))throw Parse_Exception("Parse Error: lookhead error!");
    int ret=code_scanner();
    pcur=pcur_tmp;
    return ret;
}


