#include"program.h"

string res_output;
string ast;

void Program::exec(){
    res_output="";

    EvalContext<string,int> state;
    //int size=bitmap.size();
    auto cur=bitmap.begin()->second;

    while(cur){
        auto next=cur->eval(state);
        cur=next;
    }

}

void Program::generate_ast(){
    ast="";
    auto cur=bitmap.begin()->second;
    while(cur){
        ast+=cur->to_ast();
        cur=cur->get_next();
    }
}
