#include"program.h"

string res_output;
string ast;
EvalContext state;
extern bool Exec_Immediate;

//-1:error 0:end 1:not end
int Program::exec(){
    res_output="";

    if(!debug){
        Statement*cur;
        if(!debug_cur)cur=bitmap.begin()->second;
        else {
            cur=debug_cur;//continue the debug
            syntax_highlight_remove_line(ui,debug_cur->get_line());
        }
        debug_cur=nullptr;

        while(cur){
            auto next=cur->eval(state);
            cur=next;
        }
        return 0;
    }
    //the debug ast act as run time building, so the ast
    //generation is in the exec
    else{
        if(!debug_cur){
            debug_cur=bitmap.begin()->second;
            syntax_highlight_line(ui,debug_cur->get_line());
            return 1;
        }
        else{
            syntax_highlight_remove_line(ui,debug_cur->get_line());
            ast=debug_cur->to_ast();
            auto next=debug_cur->eval(state);
            if(next){
                debug_cur=next;
                syntax_highlight_line(ui,debug_cur->get_line());
                return 1;
            }
            //end of program
            else {
                debug_cur=nullptr;
                debug=false;
                return 0;
            }
        }
    }
    return 0;
}

void Program::generate_ast(){
    ast="";

    auto cur=bitmap.begin()->second;
    while(cur){
        if(!Exec_Immediate)ast+=cur->to_ast();
        cur=cur->get_next();
    }
}

//the state should be the member of the program
string Program::generate_curvar(){
    return state.show_variable();
}

Program::~Program(){
    for(auto &iter:bitmap){
        Statement* del=iter.second;
        delete del;
    }
}

void Program::clear(){
    debug_cur=nullptr;
    for(auto &iter:bitmap){
        Statement* del=iter.second;
        delete del;
    }
    bitmap.clear();
}
