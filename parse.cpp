#include"parse.h"
#include<stack>
#include<exception.h>

#define EXP_ERROR "Parse Error: LINE "+to_string(cur_line)+", expression is illegal!"


static Statement*pre=NULL;

void parse_statement(int);
void parse_exp(bool minus_is_valid=true);
void parse_exp1(bool minus_is_valid=true);
Expression* get_parse_exp();
Expression* parse_assign();
string optoken_to_string(int t);
//for parse expression
stack<int> op_stack;
stack<Expression*> exp_stack;
class Parse_Exception;

int cur_line;//for error handling

bool parse_error_flag=false;

//last time error should be handled
void init_for_parse(){
    pre=NULL;
    while(!op_stack.empty())op_stack.pop();
    while(!exp_stack.empty())exp_stack.pop();
}


void parse(){
    init_for_parse();
    while(true){
        int token_t=code_scanner();
        if(token_t==0)break;
        if(token_t==NUM){
            cur_line=token_attr.num;
            parse_statement(cur_line);
        }
    }
    if(parse_error_flag)throw Parse_Exception("Parse Error");
}

void parse_statement(int line){
    int token_t=code_scanner();
    string error_str="";
    try{
    if(token_t==REM){
        Statement* stmt=new RemStatement(token_attr.comment,line);
        if(pre)pre->set_next(stmt);
        program.insert(line,stmt);
        pre=stmt;
    }
    else if(token_t==LET){
        Expression* ret=parse_assign();
        Statement* stmt=new LetStatement(ret,line);
        if(pre)pre->set_next(stmt);
        program.insert(line,stmt);
        pre=stmt;
    }
    else if(token_t==PRINT){
        parse_exp();
        Expression*ret =get_parse_exp();
        Statement* stmt=new PrintStatement(ret,line);
        if(pre)pre->set_next(stmt);
        program.insert(line,stmt);
        pre=stmt;
    }
    else if(token_t==INPUT){
        token_t=code_scanner();
        if(token_t!=ID)throw Parse_Exception("Parse Error: LINE "+to_string(line)+", ID should follow INPUT!");
        Statement* stmt=new InputStatement(token_attr.id,line);
        if(pre)pre->set_next(stmt);
        program.insert(line,stmt);
        pre=stmt;
    }
    else if(token_t==GOTO){
        token_t=code_scanner();
        if(token_t!=NUM)throw Parse_Exception("Parse Error: LINE "+to_string(line)+", NUM should follow INPUT!");
        Statement* stmt=new GotoStatement(token_attr.num,line);
        if(pre)pre->set_next(stmt);
        program.insert(line,stmt);
        pre=stmt;
    }
    else if(token_t==IF){
        parse_exp();
        Expression* rhs=get_parse_exp();
        token_t=code_scanner();
        if(!(token_t=='<'||token_t=='='||token_t=='>'))
            throw Parse_Exception("Parse Error: LINE "+to_string(line)+", operator is illegal!");
        string op=optoken_to_string(token_t);
        parse_exp();
        Expression* lhs=get_parse_exp();
        token_t=code_scanner();
        if(token_t!=THEN)
            throw Parse_Exception("Parse Error: LINE "+to_string(line)+", expecting the 'THEN'!");
        token_t=code_scanner();
        if(token_t!=NUM)
            throw Parse_Exception("Parse Error: LINE "+to_string(line)+", expecting the 'NUM'!");
        Statement* stmt=new IFStatement(lhs,rhs,op,token_attr.num,line);
        if(pre)pre->set_next(stmt);
        program.insert(line,stmt);
        pre=stmt;
    }
    else if(token_t==END){
        Statement *stmt=new EndStatement(line);
        if(pre)pre->set_next(stmt);
        program.insert(line,stmt);
        pre=stmt;
    }
    else if(token_t==PRINTF){

    }
    else throw Parse_Exception("Parse Error: LINE "+to_string(line)+", unrecognized command!");
    }
    catch (Parse_Exception e){
        error_str=e.str;
        goto parse_error;
    }
    //end of line is \n
    token_t=lookahead1();
    if(token_t!='\n'){error_str="something unexpected end of line";goto parse_error;}
    else code_scanner();

    return;//if no error
parse_error:
        skip_to_new_line();
        Statement *stmt=new ErrorStatement(line,error_str);
        if(pre)pre->set_next(stmt);
        program.insert(line,stmt);
        pre=stmt;
        parse_error_flag=true;
}

Expression* get_parse_exp(){
    if(exp_stack.size()!=1)
        throw Parse_Exception(EXP_ERROR);
    if(!op_stack.empty())
        throw Parse_Exception(EXP_ERROR);
    Expression *exp=exp_stack.top();
    exp_stack.pop();
    return exp;
}

Expression* parse_assign(){
    Expression *lhs=NULL,*rhs=NULL;
    string op="";
    int token_t=code_scanner();
    if(token_t==ID){
        lhs=new IdentifierExp(token_attr.id);
        token_t=code_scanner();
        if(token_t=='='){
            op="=";
            parse_exp();
            //error handle
            rhs=get_parse_exp();
            Expression* ret=new CompoundExp(lhs,rhs,op);
            return ret;
        }
        else throw Parse_Exception(EXP_ERROR);
    }
    else throw Parse_Exception(EXP_ERROR);
    return NULL;
}
bool isexp_token(int t){
    return t==NUM||t==ID||t==EXPO||t=='('||t==')'||t=='+'||
            t=='-'||t=='*'||t=='/';
}

bool isop(int t){
    return t==EXPO||t=='+'||t=='-'||t=='*'||t=='/';
}

string optoken_to_string(int t){
    switch (t) {
        case EXPO:return "**";
    case '+':return "+";
    case '-':return "-";
    case '*':return "*";
    case '/':return "/";
    case '<':return "<";
    case '>':return ">";
    case '=':return "=";
    default:return "";
    }
}

bool precedence_less(int l,int r){
    if(r=='+'||r=='-')return false;
    if((r=='*'||r=='/')&&l!='+'&&l!='-')return false;
    return true;
}


void consume_the_stack(){
    while(!op_stack.empty()){
        int top_op=op_stack.top();
        if(!isop(top_op))throw Parse_Exception(EXP_ERROR);
        op_stack.pop();
        if(exp_stack.empty())throw Parse_Exception(EXP_ERROR);
        Expression* rhs=exp_stack.top();
        exp_stack.pop();
        if(exp_stack.empty())throw Parse_Exception(EXP_ERROR);
        Expression*lhs=exp_stack.top();
        exp_stack.pop();
        Expression* compound=new CompoundExp(lhs,rhs,optoken_to_string(top_op));
        exp_stack.emplace(compound);
    }
}

void parse_exp(bool minus_is_valid){
    parse_exp1(minus_is_valid);
    int token_t=lookahead1();
    if(!isop(token_t)){
        //Careful!!
        // not the end of expression
        if(token_t==')')return;
        consume_the_stack();
        if(exp_stack.size()!=1)throw Parse_Exception(EXP_ERROR);
        return;
    }
    code_scanner();
    if(token_t=='+'){
        if(op_stack.empty())op_stack.emplace(token_t);
        else {
            while(!op_stack.empty()){
                int top_op=op_stack.top();
                if(!isop(top_op))break;
                op_stack.pop();
                if(exp_stack.empty())throw Parse_Exception(EXP_ERROR);
                Expression* rhs=exp_stack.top();
                exp_stack.pop();
                if(exp_stack.empty())throw Parse_Exception(EXP_ERROR);
                Expression*lhs=exp_stack.top();
                exp_stack.pop();
                Expression* compound=new CompoundExp(lhs,rhs,optoken_to_string(top_op));
                exp_stack.emplace(compound);
                //if(!op_stack.empty())top_op=op_stack.top();
            }
            op_stack.emplace(token_t);
        }
    }
    // the same as +
    else if(token_t=='-'){
        if(op_stack.empty())op_stack.emplace(token_t);
        else {
            while(!op_stack.empty()){
                int top_op=op_stack.top();
                if(!isop(top_op))break;
                op_stack.pop();
                if(exp_stack.empty())throw Parse_Exception(EXP_ERROR);
                Expression* rhs=exp_stack.top();
                exp_stack.pop();
                if(exp_stack.empty())throw Parse_Exception(EXP_ERROR);
                Expression*lhs=exp_stack.top();
                exp_stack.pop();
                Expression* compound=new CompoundExp(lhs,rhs,optoken_to_string(top_op));
                exp_stack.emplace(compound);
                //top_op=op_stack.top();
            }
            op_stack.emplace(token_t);
        }
    }
    else if(token_t=='*'){
        if(op_stack.empty())op_stack.emplace(token_t);
        else {
            while(!op_stack.empty()){
                int top_op=op_stack.top();
                if(!isop(top_op))break;
                if(precedence_less(top_op,token_t))break;
                op_stack.pop();
                if(exp_stack.empty())throw Parse_Exception(EXP_ERROR);
                Expression* rhs=exp_stack.top();
                exp_stack.pop();
                if(exp_stack.empty())throw Parse_Exception(EXP_ERROR);
                Expression*lhs=exp_stack.top();
                exp_stack.pop();
                Expression* compound=new CompoundExp(lhs,rhs,optoken_to_string(top_op));
                exp_stack.emplace(compound);
                //top_op=op_stack.top();
            }
            op_stack.emplace(token_t);
        }
    }
    else if(token_t=='/'){
        if(op_stack.empty())op_stack.emplace(token_t);
        else {
            while(!op_stack.empty()){
                int top_op=op_stack.top();
                if(!isop(top_op))break;
                if(precedence_less(top_op,token_t))break;
                op_stack.pop();
                if(exp_stack.empty())throw Parse_Exception(EXP_ERROR);
                Expression* rhs=exp_stack.top();
                exp_stack.pop();
                if(exp_stack.empty())throw Parse_Exception(EXP_ERROR);
                Expression*lhs=exp_stack.top();
                exp_stack.pop();
                Expression* compound=new CompoundExp(lhs,rhs,optoken_to_string(top_op));
                exp_stack.emplace(compound);
                //top_op=op_stack.top();
            }
            op_stack.emplace(token_t);
        }
    }
    else if(token_t==EXPO)op_stack.emplace(token_t);
    else throw Parse_Exception(EXP_ERROR);

    parse_exp(false);

}

void parse_exp1(bool minus_is_valid){
    int token_t=lookahead1();
    if(token_t=='('){
        code_scanner();
        op_stack.emplace(token_t);
        parse_exp(true);
        token_t=code_scanner();
        if(token_t==')'){
            if(op_stack.empty())throw Parse_Exception(EXP_ERROR);
            int top_op=op_stack.top();
            while(top_op!='('){
                op_stack.pop();
                if(exp_stack.empty())throw Parse_Exception(EXP_ERROR);
                Expression* rhs=exp_stack.top();
                exp_stack.pop();
                if(exp_stack.empty())throw Parse_Exception(EXP_ERROR);
                Expression*lhs=exp_stack.top();
                exp_stack.pop();
                Expression* compound=new CompoundExp(lhs,rhs,optoken_to_string(top_op));
                exp_stack.emplace(compound);
                top_op=op_stack.top();
            }
            op_stack.pop();
            return;
        }
        else throw Parse_Exception(EXP_ERROR);
    }
    if(token_t==NUM){
        code_scanner();
        Expression* num=new ConstantExp(token_attr.num);
        exp_stack.emplace(num);
        return;
    }
    if(token_t==ID){
        code_scanner();
        Expression* id=new IdentifierExp(token_attr.id);
        exp_stack.emplace(id);
        return;
    }
    //negative number
    if(token_t=='-'){
        code_scanner();
        if(!minus_is_valid)throw Parse_Exception(EXP_ERROR);//minus position error
        token_t=lookahead1();
        if(token_t==NUM){
            code_scanner();
            Expression* num=new ConstantExp(-token_attr.num);
            exp_stack.emplace(num);
            return;
        }
        //not a negative number, error parsing
        else throw Parse_Exception(EXP_ERROR);
    }
    throw Parse_Exception(EXP_ERROR);
}

