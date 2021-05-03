#ifndef STATEMENT_H
#define STATEMENT_H

#include"exp.h"
#include"program.h"
#include<vector>
enum StatementType{Rem,Let,Print,Input,Goto,If,End,Err};

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
    virtual Statement* eval(EvalContext &state)const=0;
    virtual StatementType type()const=0;
};

class ErrorStatement:public Statement{
private:
    string message;
public:
    ErrorStatement(int line=0,string mes=""):Statement(line),message(mes){}
    virtual string to_ast()const;
    virtual Statement* eval(EvalContext &)const{return next;}
    virtual StatementType type()const{return Err;};
};


class RemStatement:public Statement{
private:
    string comment;
public:
    RemStatement(string str="",int line=0):Statement(line),comment(str){}
    virtual ~RemStatement(){}
    virtual string to_ast()const;
    virtual Statement* eval(EvalContext &)const{return next;}
    virtual StatementType type()const{return Rem;}
};

class LetStatement:public Statement{
private:
    Expression *exp;
public:
    LetStatement(Expression *e=NULL,int line=0):Statement(line),exp(e){}
    virtual ~LetStatement(){delete exp;}
    virtual string to_ast()const;
    virtual Statement* eval(EvalContext &)const;
    virtual StatementType type()const{return Let;}
};

class PrintStatement:public Statement{
private:
    Expression *exp;
public:
    PrintStatement(Expression *e=NULL,int line=0)
        :Statement(line),exp(e){}
    virtual ~PrintStatement(){delete exp;}
    virtual string to_ast()const;
    virtual Statement* eval(EvalContext &)const;
    virtual StatementType type()const{return Print;}
};

class PrintFStatement:public Statement{
private:
    Expression *exp;
    vector<Expression*> args;
public:
    PrintFStatement(Expression *e,vector<Expression*>& args,int line)
        :Statement(line),exp(e),args(args){}
    virtual ~PrintFStatement(){delete exp;for(auto &arg:args)delete arg;}
    virtual string to_ast()const;
    virtual Statement* eval(EvalContext &)const;
    virtual StatementType type()const{return Print;}
};

class InputStatement:public Statement{
private:
    string id;
public:
    InputStatement( string str="",int line=0):Statement(line),id(str){}
    virtual ~InputStatement(){}
    virtual string to_ast()const;
    virtual Statement* eval(EvalContext &state)const;
    virtual StatementType type()const{return Input;}
};

class InputStrStatement:public Statement{
private:
    string id;
public:
    InputStrStatement( string str="",int line=0):Statement(line),id(str){}
    virtual ~InputStrStatement(){}
    virtual string to_ast()const;
    virtual Statement* eval(EvalContext &state)const;
    virtual StatementType type()const{return Input;}
};

class GotoStatement:public Statement{
private:
    int goto_line;
public:
    GotoStatement(int nl=0,int line=0):Statement(line),goto_line(nl){}
    virtual ~GotoStatement(){}
    virtual string to_ast()const;
    virtual Statement* eval(EvalContext &)const;
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
    virtual ~IFStatement(){delete exp1;delete exp2;}
    virtual string to_ast()const;
    virtual Statement* eval(EvalContext &)const;
    virtual StatementType type()const{return If;}
};

class EndStatement:public Statement{
public:
    EndStatement(int line=0)
        :Statement(line){}
    virtual ~EndStatement(){}
    virtual string to_ast()const{return to_string(line)+" END\n";}
    virtual Statement* eval(EvalContext&)const{return NULL;}
    virtual StatementType type()const{return End;}
};


#endif // STATEMENT_H
