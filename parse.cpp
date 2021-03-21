#include"parse.h"

static Statement*pre=NULL;

void parse_statement(int);
Expression* parse_exp();
Expression* parse_exp1();
Expression* parse_exp2();
Expression* parse_assign();
void parse(){
    int token_t=code_scanner();
    if(token_t==NUM){
        parse_statement(token_attr.num);
    }

}

void parse_statement(int line){
    int token_t=code_scanner();
    if(token_t==REM){
        Statement* stmt=new RemStatement(token_attr.comment,line);
        if(pre)pre->set_next(stmt);
        program.insert(line,stmt);
        pre=stmt;
        return;
    }
    if(token_t==LET){

    }
}

Expression* parse_assign(){
    Expression *lhs=NULL,*rhs=NULL;
    string op="";
    int token_t=code_scanner();
    if(token_t==ID){
        lhs=new IdentifierExp(token_attr.id);
        token_t=code_scanner();
        if(token_t=='='){
            op="=";
            rhs=parse_exp();
        }
        else assert(false);
    }
    else assert(false);
    Expression* ret=new CompoundExp(lhs,rhs,op);
    return ret;
}

Expression* parse_exp2(){
    int token_t=code_scanner();
    if(token_t==ID){
        string c=lookahead2();
        if(c=="**"){
            Expression*lhs=new IdentifierExp(token_attr.id);
            code_scanner();
            Expression*rhs=parse_exp();
            Expression*ret=new CompoundExp(lhs,rhs,c);
            return ret;
        }
        else {
            Expression *ret=new IdentifierExp(token_attr.id);
            return ret;
        }
    }
    if(token_t==NUM){
        string c=lookahead2();
        if(c=="**"){
            Expression*lhs=new ConstantExp(token_attr.num);
            code_scanner();
            Expression*rhs=parse_exp();
            Expression*ret=new CompoundExp(lhs,rhs,c);
            return ret;
        }
        else {
            Expression *ret=new ConstantExp(token_attr.num);
            return ret;
        }
    }
    if(token_t=='('){
        Expression*lhs=parse_exp();
        token_t=code_scanner();
        if(token_t==')'){
            string c=lookahead2();
            if(c=="**"){
                code_scanner();
                Expression*rhs=parse_exp2();
                Expression*ret=new CompoundExp(lhs,rhs,c);
                return ret;
            }
            else return lhs;
        }
        else assert(false);
    }
    assert(false);
    return NULL;
}
Expression* parse_exp1(){
    Expression* lhs=parse_exp2();
    string c=lookahead1();
    if(c=="*"||c=="/"){
        code_scanner();
        Expression*rhs=parse_exp1();
        Expression*ret=new CompoundExp(lhs,rhs,c);
        return ret;
    }
    else return lhs;
}

Expression* parse_exp(){
    Expression* lhs=parse_exp1();
    string c=lookahead1();
    if(c=="+"||c=="-"){
        code_scanner();
        Expression*rhs=parse_exp1();
        Expression*ret=new CompoundExp(lhs,rhs,c);
        return ret;
    }
    else return lhs;
}
