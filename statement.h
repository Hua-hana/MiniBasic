#ifndef STATEMENT_H
#define STATEMENT_H

#include"exp.h"
#include"program.h"
enum StatementType{Rem,Let,Print,Input,Goto,If,End};

template<class K,class V>
class EvalContext;
class Program;
extern string res_output;
extern Program program;


class Statement{
protected:
    Statement *next;
    int line;
public:
    Statement(int line=0):next(NULL),line(line){}
    virtual ~Statement(){}
    void set_next(Statement*n){next=n;}
    Statement* get_next(){return next;}
    virtual string to_ast()const=0;
    virtual Statement* eval(EvalContext<string,int> &state)const=0;
    virtual StatementType type()const=0;
};

class RemStatement:public Statement{
private:
    string comment;
public:
    RemStatement(string str="",int line=0):Statement(line),comment(str){}
    virtual ~RemStatement(){}
    virtual string to_ast()const;
    virtual Statement* eval(EvalContext<string,int> &)const{return next;}
    virtual StatementType type()const{return Rem;}
};

class LetStatement:public Statement{
private:
    Expression *exp;
public:
    LetStatement(Expression *e=NULL,int line=0):Statement(line),exp(e){}
    virtual ~LetStatement(){}
    virtual string to_ast()const;
    virtual Statement* eval(EvalContext<string,int> &)const;
    virtual StatementType type()const{return Let;}
};

class PrintStatement:public Statement{
private:
    Expression *exp;
public:
    PrintStatement(Expression *e=NULL,int line=0)
        :Statement(line),exp(e){}
    virtual ~PrintStatement(){}
    virtual string to_ast()const;
    virtual Statement* eval(EvalContext<string,int> &)const;
    virtual StatementType type()const{return Print;}
};

class InputStatement:public Statement{
private:
    string id;
public:
    InputStatement( string str="",int line=0):Statement(line),id(str){}
    ~InputStatement(){}
    virtual string to_ast()const;
    virtual Statement* eval(EvalContext<string,int> &state)const;
    virtual StatementType type()const{return Input;}
};

class GotoStatement:public Statement{
private:
    int goto_line;
public:
    GotoStatement(int nl=0,int line=0):Statement(line),goto_line(nl){}
    ~GotoStatement(){}
    virtual string to_ast()const;
    virtual Statement* eval(EvalContext<string,int> &)const;
    virtual StatementType type()const{return Goto;}
};

class IFStatement:public Statement{
private:
    Expression*exp1,*exp2;
    string op;
    int goto_line;
public:
    IFStatement(Expression* e1=NULL,Expression*e2=NULL,string op="",int nl=0,int line=0)
        :Statement(line),exp1(e1),exp2(e2),op(op),goto_line(nl){}
    ~IFStatement(){}
    virtual string to_ast()const;
    virtual Statement* eval(EvalContext<string,int> &)const;
    virtual StatementType type()const{return If;}
};

class EndStatement:public Statement{
public:
    EndStatement(int line=0)
        :Statement(line){}
    virtual ~EndStatement(){}
    virtual string to_ast()const{return to_string(line)+" END\n";}
    virtual Statement* eval(EvalContext<string,int> &)const{return NULL;}
    virtual StatementType type()const{return End;}
};


#endif // STATEMENT_H
