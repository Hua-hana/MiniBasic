#ifndef EXP_CPP
#define EXP_CPP

#include"exp.h"
#include<cmath>
#include"exception.h"

class Exec_Exception;
int CompoundExp::eval_int(EvalContext& state)const{
    int right=rhs->eval_int(state);
    if(op=="="){
        assert(lhs->type()==Identifier);
        state.add(lhs->getIdentifierName(),right);
        return right;
    }
    int left=lhs->eval_int(state);
    if(op=="+")return left+right;
    if(op=="-")return left-right;
    if(op=="*")return left*right;
    if(op=="/"){
        if(right==0)throw Exec_Exception("Runtime Error: divide by zero!");
        return left/right;
    }
    if(op=="**")return pow(left,right);

    throw Exec_Exception("Runtime Error: unregonized operator!");
    return 0;
}

string CompoundExp::to_ast(string & tab)const{
    string new_tab=tab+"    ";
    return tab+op+"\n"+lhs->to_ast(new_tab)+"\n"+rhs->to_ast(new_tab);
}

string CompoundStrExp::eval_str(EvalContext& state)const{
    string right=rhs->eval_str(state);
    if(op=="="){
        assert(lhs->type()==IdentifierStr);
        state.add(lhs->getIdentifierName(),right);
        return right;
    }
    throw Exec_Exception("Runtime Error: unsurpport operator!");
    return "";
}

string CompoundStrExp::to_ast(string & tab)const{
    string new_tab=tab+"    ";
    return tab+op+"\n"+lhs->to_ast(new_tab)+"\n"+rhs->to_ast(new_tab);
}






#endif // EXP_CPP
