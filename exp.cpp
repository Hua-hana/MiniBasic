#ifndef EXP_CPP
#define EXP_CPP

#include"exp.h"
#include<cmath>
int CompoundExp::eval(EvalContext<string,int>& state)const{
    int right=rhs->eval(state);
    if(op=="="){
        state.add(lhs->getIdentifierName(),right);
        return right;
    }
    int left=lhs->eval(state);
    if(op=="+")return left+right;
    if(op=="-")return left-right;
    if(op=="*")return left*right;
    if(op=="/"){
        assert(right!=0);
        return left/right;
    }
    if(op=="**")return pow(left,right);
    //FIXME
    return 0;
}

string CompoundExp::to_ast(string & tab)const{
    string new_tab=tab+"    ";
    return tab+op+"\n"+lhs->to_ast(new_tab)+"\n"+rhs->to_ast(new_tab);
}





#endif // EXP_CPP
