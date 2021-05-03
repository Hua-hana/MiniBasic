#ifndef EVALSTATE_H
#define EVALSTATE_H

#include<map>
#include<string>
using namespace std;


class EvalContext{
private:
    map<string,int> state_int;
    map<string,string> state_str;
public:
    EvalContext(){}
    ~EvalContext(){}
    void add(const string &k,int v){
        state_int.insert_or_assign(k,v);
    }
    void add(const string &k,string& v){
        state_str.insert_or_assign(k,v);
    }
    int get_int(const string& k) const{
        return state_int.at(k);
    }
    string get_str(const string& k) const{
        return state_str.at(k);
    }

    bool is_defined_int(const string&k)const{
        return state_int.find(k)!=state_int.end();
    }
    bool is_defined_str(const string&k)const{
        return state_str.find(k)!=state_str.end();
    }
};


#endif // EVALSTATE_H
