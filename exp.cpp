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

int CompoundExp::type_check(map<string, int> &infer) const{
    auto type2=rhs->type_check(infer);
    auto type1=lhs->type_check(infer);
    if(op=="=")return type2;
    if(type1==-1||type2==-1)return -1;
    if(type1!=type2)return -1;
    if(type1==STR_TYPE||type2==STR_TYPE)
        throw Parse_Exception("Parse Error: Unsupported operator for string!");

    return type1;
}

string CompoundStrExp::eval_str(EvalContext& state)const{
    string right=rhs->eval_str(state);
    if(op=="="){
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

int CompoundStrExp::type_check(map<string, int> &infer) const{
    auto type2=rhs->type_check(infer);

    if(op=="=")return type2;
    //unsupported operator
    else {
        throw Parse_Exception("Parse Error: Unsupported operator for string!");
    }
    return -1;
//    auto type1=lhs->type_check(infer);
//    if(type1==-1||type2==-1)return -1;
//    if(type1!=type2)return -1;
//    return type1;
}






#endif // EXP_CPP
