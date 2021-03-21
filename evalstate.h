#ifndef EVALSTATE_H
#define EVALSTATE_H

#include<map>
#include<string>
using namespace std;

template<class K,class V>
class EvalContext{
private:
    map<K,V> state;

public:
    EvalContext(){}
    ~EvalContext(){}
    void add(const K &k,V v){
        state.insert(pair<K,V>(k,v));
    }

    V get(const K& k) const{
        return state.at(k);
    }

    bool is_defined(const K&k)const{
        return state.find(k)!=state.end();
    }
};


#endif // EVALSTATE_H
