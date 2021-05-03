#include"statement.h"
#include"program.h"

extern string var_input(Ui::MainWindow*ui);
extern int input_ret;

string ErrorStatement::to_ast() const{
    return to_string(line)+" "+message+"\n";
}

string RemStatement::to_ast()const{
    return to_string(line)+" REM\n  "+comment+"\n";
}

string LetStatement::to_ast() const{
    string tab="";
    return to_string(line)+" LET "+exp->to_ast(tab)+"\n";
}

Statement* LetStatement::eval(EvalContext &state) const{
    if(exp->type()==CompoundStr)exp->eval_str(state);
    else if(exp->type()==Compound)exp->eval_int(state);
    return next;
}

Statement* PrintStatement::eval(EvalContext &state) const{
    auto exp_type=exp->type();
    if(exp_type==ConstantStr||exp_type==IdentifierStr||exp_type==CompoundStr){
        throw Exec_Exception("Runtime Error: Print a string value! Use PRINTF?");
    }
    int ans=exp->eval_int(state);
    res_output.append(to_string(ans)+"\n");
    return next;
}

string PrintStatement::to_ast() const{
    string tab="";
    return to_string(line)+" PRINT "+exp->to_ast(tab)+"\n";
}

Statement* PrintFStatement::eval(EvalContext &state) const{
    if(exp->type()!=ConstantStr)throw Exec_Exception("Runtime Error: PrintF a non string value! Use PRINT?");
    string format=exp->eval_str(state);
    string ans="";
    int off=0;
    unsigned int args_p=0;
    int size=format.size();
    for(int i=0;i<size;++i){
        if(format[i]=='{'){
            if(i+1>=size||format[i+1]!='}')throw Exec_Exception("Runtime Error: Invalid single { !");
            ans.append(format.substr(off,i));
            off=i+2;
            if(args_p>=args.size())throw Exec_Exception("Runtime Error: PrintF doesn't have enough arguments!");
            auto exp_arg=args[args_p++];
            auto exp_type=exp_arg->type();
            if(exp_type==ConstantStr||exp_type==IdentifierStr||exp_type==CompoundStr){
                ans.append(exp->eval_str(state));
            }
            else {
                ans.append(to_string(exp->eval_int(state)));
            }
            i+=2;
            continue;
        }
    }
    res_output.append(ans+"\n");
    return next;
}

string PrintFStatement::to_ast() const{
    string tab="";
    string args_str="";
    for(auto &arg:args){
        args_str.append(arg->to_ast(tab));
    }
    return to_string(line)+" PRINTF "+exp->to_ast(tab)+" "+args_str+"\n";
}


string InputStatement::to_ast() const{
    return to_string(line)+" INPUT "+id+"\n";
}

Statement* InputStatement::eval(EvalContext&state) const{
    string inp=var_input(program.get_ui());
    state.add(id,atoi((inp.c_str())));
    return next;
}

string InputStrStatement::to_ast() const{
    return to_string(line)+" INPUTS "+id+"\n";
}

Statement* InputStrStatement::eval(EvalContext&state) const{
    string inp=var_input(program.get_ui());
    state.add(id,inp);
    return next;
}

string GotoStatement::to_ast() const{
    return to_string(line)+" GOTO "+to_string(goto_line)+"\n";
}

Statement* GotoStatement::eval(EvalContext &) const{
    return program.get(goto_line);
}

string IFStatement::to_ast() const{
    string tab="    ";
    return to_string(line)+" IF THEN\n"+tab+op+"\n"+exp1->to_ast(tab)+"\n"
    +exp2->to_ast(tab)+"\n"
    +tab+to_string(goto_line)+"\n";
}

Statement* IFStatement::eval(EvalContext &state) const{
    auto exp_type=exp1->type();
    if(exp_type==ConstantStr||exp_type==IdentifierStr||exp_type==CompoundStr){
        throw Exec_Exception("Runtime Error: Type error in If Statement!");
    }
    exp_type=exp2->type();
    if(exp_type==ConstantStr||exp_type==IdentifierStr||exp_type==CompoundStr){
        throw Exec_Exception("Runtime Error: Type error in If Statement!");
    }
    int left=exp1->eval_int(state);
    int right=exp2->eval_int(state);

    bool comp=false;
    if(op=="<")comp=left<right;
    else if(op==">")comp=left>right;
    else if(op=="=")comp=left==right;

    if(comp)return next;
    else return program.get(goto_line);
}
