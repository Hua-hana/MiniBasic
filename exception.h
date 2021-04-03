#ifndef EXCEPTION_H
#define EXCEPTION_H
#include <string>
class Parse_Exception{
public:
    std::string str;
    Parse_Exception(std::string s):str(s){}
};

class Exec_Exception{
public:
    std::string str;
    Exec_Exception(std::string s):str(s){}
};

class Input_Exception{
public:
    std::string str;
    Input_Exception(std::string s):str(s){}
};


#endif // EXCEPTION_H
