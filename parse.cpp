#include"parse.h"
#include<stack>
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

void parse(){
    while(true){
        int token_t=code_scanner();
        if(token_t==0)break;
        if(token_t==NUM){
            parse_statement(token_attr.num);
        }
    }
}

void parse_statement(int line){
    int token_t=code_scanner();
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
        assert(token_t==ID);
        Statement* stmt=new InputStatement(token_attr.id,line);
        if(pre)pre->set_next(stmt);
        program.insert(line,stmt);
        pre=stmt;
    }
    else if(token_t==GOTO){
        token_t=code_scanner();
        assert(token_t==NUM);
        Statement* stmt=new GotoStatement(token_attr.num,line);
        if(pre)pre->set_next(stmt);
        program.insert(line,stmt);
        pre=stmt;
    }
    else if(token_t==IF){
        parse_exp();
        Expression* rhs=get_parse_exp();
        token_t=code_scanner();
        assert(token_t=='<'||token_t=='='||token_t=='>');
        string op=optoken_to_string(token_t);
        parse_exp();
        Expression* lhs=get_parse_exp();
        token_t=code_scanner();
        assert(token_t==THEN);
        token_t=code_scanner();
        assert(token_t==NUM);
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
    else assert(false);
}

Expression* get_parse_exp(){
    assert(exp_stack.size()==1);
    assert(op_stack.empty());
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
        else assert(false);
    }
    else assert(false);
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


void comsume_the_stack(){
    while(!op_stack.empty()){
        int top_op=op_stack.top();
        assert(isop(top_op));
        op_stack.pop();
        assert(!exp_stack.empty());
        Expression* rhs=exp_stack.top();
        exp_stack.pop();
        assert(!exp_stack.empty());
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
        comsume_the_stack();
        assert(exp_stack.size()==1);
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
                assert(!exp_stack.empty());
                Expression* rhs=exp_stack.top();
                exp_stack.pop();
                assert(!exp_stack.empty());
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
                assert(!exp_stack.empty());
                Expression* rhs=exp_stack.top();
                exp_stack.pop();
                assert(!exp_stack.empty());
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
                assert(!exp_stack.empty());
                Expression* rhs=exp_stack.top();
                exp_stack.pop();
                assert(!exp_stack.empty());
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
                assert(!exp_stack.empty());
                Expression* rhs=exp_stack.top();
                exp_stack.pop();
                assert(!exp_stack.empty());
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
    else assert(false);

    parse_exp(false);

}

void parse_exp1(bool minus_is_valid){
    int token_t=code_scanner();
    if(token_t=='('){
        op_stack.emplace(token_t);
        parse_exp(true);
        token_t=code_scanner();
        if(token_t==')'){
            assert(!op_stack.empty());
            int top_op=op_stack.top();
            while(top_op!='('){
                op_stack.pop();
                assert(!exp_stack.empty());
                Expression* rhs=exp_stack.top();
                exp_stack.pop();
                assert(!exp_stack.empty());
                Expression*lhs=exp_stack.top();
                exp_stack.pop();
                Expression* compound=new CompoundExp(lhs,rhs,optoken_to_string(top_op));
                exp_stack.emplace(compound);
                top_op=op_stack.top();
            }
            op_stack.pop();
            return;
        }
        else assert(false);
    }
    if(token_t==NUM){
        Expression* num=new ConstantExp(token_attr.num);
        exp_stack.emplace(num);
        return;
    }
    if(token_t==ID){
        Expression* id=new IdentifierExp(token_attr.id);
        exp_stack.emplace(id);
        return;
    }
    //negative number
    if(token_t=='-'){
        if(!minus_is_valid)assert(false);//minus position error
        token_t=lookahead1();
        if(token_t==NUM){
            code_scanner();
            Expression* num=new ConstantExp(-token_attr.num);
            exp_stack.emplace(num);
            return;
        }
        //not a negative number, error parsing
        else assert(false);
    }
    assert(false);
}

/*
Expression* parse_exp2(){
    int token_t=code_scanner();
    if(token_t==ID){
        string c=lookahead2();
        if(c=="**"){
            Expression*lhs=new IdentifierExp(token_attr.id);
            code_scanner();
            Expression*rhs=parse_exp();
            Expression*ret=new CompoundExp(lhs,rhs,c);
            return ret;
        }
        else {
            Expression *ret=new IdentifierExp(token_attr.id);
            return ret;
        }
    }
    if(token_t==NUM){
        string c=lookahead2();
        if(c=="**"){
            Expression*lhs=new ConstantExp(token_attr.num);
            code_scanner();
            Expression*rhs=parse_exp();
            Expression*ret=new CompoundExp(lhs,rhs,c);
            return ret;
        }
        else {
            Expression *ret=new ConstantExp(token_attr.num);
            return ret;
        }
    }
    if(token_t=='('){
        Expression*lhs=parse_exp();
        token_t=code_scanner();
        if(token_t==')'){
            string c=lookahead2();
            if(c=="**"){
                code_scanner();
                Expression*rhs=parse_exp2();
                Expression*ret=new CompoundExp(lhs,rhs,c);
                return ret;
            }
            else return lhs;
        }
        else assert(false);
    }
    assert(false);
    return NULL;
}
*/
/*
Expression* parse_exp1(){
    Expression* lhs=parse_exp2();
    string c=lookahead1();
    if(c=="*"||c=="/"){
        code_scanner();
        Expression*rhs=parse_exp1();
        Expression*ret=new CompoundExp(lhs,rhs,c);
        return ret;
    }
    else return lhs;
}
*/
/*
Expression* parse_exp(){
    Expression* lhs=parse_exp1();
    string c=lookahead1();
    if(c=="+"||c=="-"){
        code_scanner();
        Expression*rhs=parse_exp1();
        Expression*ret=new CompoundExp(lhs,rhs,c);
        return ret;
    }
    else return lhs;
}
*/
