#include"program.h"

string res_output;
string ast;
EvalContext state;
extern bool Exec_Immediate;
void Program::exec(){
    res_output="";


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
        if(!Exec_Immediate)ast+=cur->to_ast();
        cur=cur->get_next();
    }
}

Program::~Program(){
    for(auto &iter:bitmap){
        Statement* del=iter.second;
        delete del;
    }
}

void Program::clear(){
    for(auto &iter:bitmap){
        Statement* del=iter.second;
        delete del;
    }
    bitmap.clear();
}
