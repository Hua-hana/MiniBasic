#include"statement.h"
#include"program.h"

extern int var_input(Ui::MainWindow*ui);
extern int input_ret;
string RemStatement::to_ast()const{
    return to_string(line)+" REM\n  "+comment+"\n";
}

string LetStatement::to_ast() const{
    string tab="";
    return to_string(line)+" LET "+exp->to_ast(tab)+"\n";
}

Statement* LetStatement::eval(EvalContext<string, int> &state) const{
    exp->eval(state);
    return next;
}

Statement* PrintStatement::eval(EvalContext<string, int> &state) const{
    int ans=exp->eval(state);
    res_output.append(to_string(ans)+"\n");
    return next;
}

string PrintStatement::to_ast() const{
    string tab="";
    return to_string(line)+" PRINT "+exp->to_ast(tab)+"\n";
}

string InputStatement::to_ast() const{
    return to_string(line)+" INPUT "+id+"\n";
}

Statement* InputStatement::eval(EvalContext<string, int> &state) const{
    int inp=var_input(program.get_ui());
    state.add(id,inp);
    return next;
}

string GotoStatement::to_ast() const{
    return to_string(line)+" GOTO "+to_string(next_line)+"\n";
}

Statement* GotoStatement::eval(EvalContext<string, int> &) const{
    return program.get(next_line);
}

string IFStatement::to_ast() const{
    string tab="    ";
    return to_string(line)+" IF THEN "+exp1->to_ast(tab)+"\n"
    +tab+op+exp2->to_ast(tab)+"\n"
    +tab+to_string(next_line)+"\n";
}

Statement* IFStatement::eval(EvalContext<string, int> &state) const{
    int left=exp1->eval(state);
    int right=exp2->eval(state);

    bool comp=false;
    if(op=="<")comp=left<right;
    else if(op==">")comp=left>right;
    else if(op=="=")comp=left==right;

    if(comp)return next;
    else return program.get(next_line);
}
