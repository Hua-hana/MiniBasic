#ifndef EVALSTATE_H
#define EVALSTATE_H

#include<map>
#include<string>
using namespace std;

#define INT_TYPE 0
#define STR_TYPE 1
class EvalContext{
private:
    map<string,int> state_int;
    map<string,string> state_str;
    map<string,int> state_type;
public:
    EvalContext(){}
    ~EvalContext(){}
    void add(const string &k,int v){
        state_type.insert_or_assign(k,INT_TYPE);
        state_int.insert_or_assign(k,v);
    }
    void add(const string &k,string& v){
        state_type.insert_or_assign(k,STR_TYPE);
        state_str.insert_or_assign(k,v);
    }
    int get_int(const string& k) const{
        return state_int.at(k);
    }
    string get_str(const string& k) const{
        return state_str.at(k);
    }

    bool is_defined(const string&k)const{
        return state_type.find(k)!=state_type.end();
    }
    int type(const string &k){
        return state_type.at(k);
    }
};


#endif // EVALSTATE_H
