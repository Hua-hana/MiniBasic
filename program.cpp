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
            if(cur->type()==Err)throw Exec_Exception("There is syntax error in this line!");
            auto next=cur->eval(state);
            cur=next;
            emit run_thread->send_curvar(generate_curvar());
            emit run_thread->send_res_output(res_output);
            res_output="";
        }
        return 0;
    }
    //the debug ast act as run time building, so the ast
    //generation is in the exec
    else{
        if(!debug_cur){
            debug_cur=bitmap.begin()->second;
            if(debug_cur->type()==Err)throw Exec_Exception("There is syntax error in this line!");
            ast=debug_cur->to_ast();
            syntax_highlight_line(ui,debug_cur->get_line());
            return 1;
        }
        else{
            if(debug_cur->type()==Err)throw Exec_Exception("There is syntax error in this line!");
            auto next=debug_cur->eval(state);
            syntax_highlight_remove_line(ui,debug_cur->get_line());
            emit debug_thread->send_curvar(generate_curvar());
            emit debug_thread->send_res_output(res_output);
            res_output="";

            if(next){
                debug_cur=next;
                ast=debug_cur->to_ast();
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
        ast+=cur->to_ast();
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
    if(!Exec_Immediate)state.clear();
    Exec_Immediate=false;
    bitmap.clear();
}
