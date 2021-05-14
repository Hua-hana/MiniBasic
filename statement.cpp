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

int LetStatement::type_check(map<string, int> &infer) const{
    auto type=exp->type_check(infer);
    if(type==-1)return -1;
    return type;
}

Statement* PrintStatement::eval(EvalContext &state) const{
    int ans=exp->eval_int(state);
    res_output.append(to_string(ans)+"\n");
    return next;
}

int PrintStatement::type_check(map<string, int> &infer) const{
    if(exp->type_check(infer)!=INT_TYPE)return -1;
    return INT_TYPE;
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
    int i=0;
    while(i<size){
        if(format[i]=='{'){
            if(i+1>=size||format[i+1]!='}')throw Exec_Exception("Runtime Error: Invalid single { !");
            ans.append(format.substr(off,i-off));
            off=i+2;
            if(args_p>=args.size())throw Exec_Exception("Runtime Error: PrintF doesn't have enough arguments!");
            auto exp_arg=args[args_p++];
            auto exp_type=exp_arg->type();
            auto id=exp_arg->getIdentifierName();
            int id_type=-1;
            if(exp_arg->type()==Identifier&&state.is_defined(id)){
                id_type=state.type(id);
            }
            if(exp_type==ConstantStr||exp_type==CompoundStr||id_type==STR_TYPE){
                ans.append(exp_arg->eval_str(state));
            }
            else if(exp_type==Constant||exp_type==Compound||id_type==INT_TYPE){
                ans.append(to_string(exp_arg->eval_int(state)));
            }
            else throw Exec_Exception("Runtime Error: undefined id or type error in printf!");
            i+=2;
            continue;
        }
        ++i;
    }
    if(args_p!=args.size())throw Exec_Exception("Runtime Error: mismatch for {} and arguments!");
    ans.append(format.substr(off,size-off));
    res_output.append(ans+"\n");
    return next;
}

string PrintFStatement::to_ast() const{
    string tab="";
    string args_str="";
    for(auto &arg:args){
        args_str.append(arg->to_ast(tab)+"\n");
    }
    return to_string(line)+" PRINTF "+exp->to_ast(tab)+"\n"+args_str;
}

int PrintFStatement::type_check(map<string, int> &infer) const{
    for(auto &exp:args){
        if(exp->type_check(infer)==-1)return -1;
    }
    return 0;
}


string InputStatement::to_ast() const{
    return to_string(line)+" INPUT "+id+"\n";
}

Statement* InputStatement::eval(EvalContext&state) const{
    string inp=var_input(program.get_ui());
    int res=atoi((inp.c_str()));
    if(res==0){
        if(inp.length()!=1||inp[0]!='0')
            throw Exec_Exception("Runtime Error: input invalid");
    }
    state.add(id,res);
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
    int left=exp1->eval_int(state);
    int right=exp2->eval_int(state);

    bool comp=false;
    if(op=="<")comp=(left<right);
    else if(op==">")comp=(left>right);
    else if(op=="=")comp=(left==right);

    if(!comp)return next;
    else return program.get(goto_line);
}

int IFStatement::type_check(map<string, int> &infer) const{
    if(exp1->type_check(infer)!=INT_TYPE||exp2->type_check(infer)!=INT_TYPE)
        return -1;
    return 0;
}
