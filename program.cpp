#include"program.h"

string res_output;
string ast;

void Program::exec(){
    res_output="";
    ast="";

    EvalContext<string,int> state;
    //int size=bitmap.size();
    auto cur=bitmap.begin()->second;

    while(cur){
        ast+=cur->to_ast();
        auto next=cur->eval(state);
        cur=next;
    }

}
