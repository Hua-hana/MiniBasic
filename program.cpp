#include"program.h"

string res_output;
string ast;
EvalContext state;
extern bool Exec_Immediate;
void Program::exec(){
    res_output="";

    if(!debug){
        auto cur=bitmap.begin()->second;

        while(cur){
            auto next=cur->eval(state);
            cur=next;
        }
    }
    //the debug ast act as run time building, so the ast
    //generation is in the exec
    else{
        if(!debug_cur)debug_cur=bitmap.begin()->second;
        else{
            ast=debug_cur->to_ast();
            auto next=debug_cur->eval(state);
            if(next)debug_cur=next;
            //end of program
            else {
                debug=false;
                //do some show
                res_output+="End of Debug\n";
            }
        }
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
