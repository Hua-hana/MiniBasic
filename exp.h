#ifndef EXP_H
#define EXP_H

#include"evalstate.h"
#include<cassert>
#include"exception.h"

enum ExpType{Constant,Identifier,Compound};

template<class K,class V>
class EvalContext;
class Exec_Exception;

class Expression{
public:
    Expression(){}
    virtual ~Expression(){}
    virtual int eval(EvalContext<string,int>& state)const=0;
    virtual string to_ast(string & tab)const=0;
    virtual ExpType type()const=0;
    virtual string getIdentifierName()const=0;
    virtual Expression* getLHS()const=0;
    virtual Expression* getRHS()const=0;

};

class ConstantExp: public Expression{
private:
    int value;

public:
    ConstantExp(int val=0):value(val){}
    virtual ~ConstantExp(){}
    virtual int eval(EvalContext<string,int>&)const{
        return value;
    }
    virtual string to_ast(string & tab)const{
        return tab + std::to_string(value);
    }
    virtual ExpType type()const{
        return Constant;
    }
    virtual Expression* getLHS()const{return NULL;}
    virtual Expression* getRHS()const{return NULL;}
    virtual string getIdentifierName()const{return "";}
};

class IdentifierExp:public Expression{
private:
    string id;
public:
    IdentifierExp(string str=""):id(str){}
    virtual ~IdentifierExp(){}
    virtual int eval(EvalContext<string,int>&st)const{
        //FIXME
        if(!st.is_defined(id))throw Exec_Exception("Runtime Error: use undefined variable");
        return st.get(id);
    }
    virtual string to_ast(string & tab)const{
        return tab + id;
    }
    virtual ExpType type()const{
        return Identifier;
    }
    virtual Expression* getLHS()const{return NULL;}
    virtual Expression* getRHS()const{return NULL;}
    virtual string getIdentifierName()const{return id;}
};

class CompoundExp:public Expression{
private:
    Expression* lhs,*rhs;
    string  op;
public:
    CompoundExp(Expression* lhs=NULL,Expression*rhs=NULL,string op=""):
        lhs(lhs),rhs(rhs),op(op){}
    virtual ~CompoundExp(){}
    virtual int eval(EvalContext<string,int>& state)const;
    virtual string to_ast(string & tab)const;
    virtual ExpType type()const{
        return Compound;
    }

    virtual Expression* getLHS()const{
        return lhs;
    }
    virtual Expression* getRHS()const{
        return rhs;
    }
    virtual string getIdentifierName()const{return "";}
};


#endif // EXP_H
