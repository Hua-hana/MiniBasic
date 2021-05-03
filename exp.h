#ifndef EXP_H
#define EXP_H

#include"evalstate.h"
#include<cassert>
#include"exception.h"

enum ExpType{Constant,Identifier,Compound,ConstantStr,CompoundStr};


class EvalContext;
class Exec_Exception;

class Expression{
public:
    Expression(){}
    virtual ~Expression(){}
    virtual int eval_int(EvalContext& state)const=0;
    virtual string eval_str(EvalContext& state)const=0;
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
    virtual int eval_int(EvalContext&)const{
        return value;
    }
    virtual string eval_str(EvalContext&)const{
        throw Exec_Exception("Runtime Error: type error!");
        return "";
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

class ConstantStrExp: public Expression{
private:
    string value;

public:
    ConstantStrExp(string val=""):value(val){}
    virtual ~ConstantStrExp(){}
    virtual int eval_int(EvalContext&)const{
        throw Exec_Exception("Runtime Error: type error!");
        return 0;
    }
    virtual string eval_str(EvalContext&)const{
        return value;
    }
    virtual string to_ast(string & tab)const{
        return tab + "\""+value+"\"";
    }
    virtual ExpType type()const{
        return ConstantStr;
    }
    virtual Expression* getLHS()const{return NULL;}
    virtual Expression* getRHS()const{return NULL;}
    virtual string getIdentifierName()const{return "";}
};

class IdentifierExp:public Expression{
private:
    string id;
    int id_type;
public:
    IdentifierExp(string str=""):id(str){}
    virtual ~IdentifierExp(){}
    virtual int eval_int(EvalContext&st)const{
        //FIXME
        if(!st.is_defined(id))throw Exec_Exception("Runtime Error: use undefined variable");
        if(st.type(id)!=INT_TYPE)throw Exec_Exception("Runtime Error: type error!");
        return st.get_int(id);
    }
    virtual string eval_str(EvalContext&st)const{
        if(!st.is_defined(id))throw Exec_Exception("Runtime Error: use undefined variable");
        if(st.type(id)!=STR_TYPE)throw Exec_Exception("Runtime Error: type error!");
        return st.get_str(id);
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
    virtual int eval_int(EvalContext& state)const;
    virtual string eval_str(EvalContext&)const{
        throw Exec_Exception("Runtime Error: type error!");
        return "";
    }
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


class CompoundStrExp:public Expression{
private:
    Expression* lhs,*rhs;
    string  op;
public:
    CompoundStrExp(Expression* lhs=NULL,Expression*rhs=NULL,string op=""):
        lhs(lhs),rhs(rhs),op(op){}
    virtual ~CompoundStrExp(){}
    virtual int eval_int(EvalContext&)const{
        throw Exec_Exception("Runtime Error: type error!");
        return 0;
    }
    virtual string eval_str(EvalContext& state)const;
    virtual string to_ast(string & tab)const;
    virtual ExpType type()const{
        return CompoundStr;
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
