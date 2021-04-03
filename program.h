#ifndef PROGRAM_H
#define PROGRAM_H

#include<map>
#include"statement.h"
#include"mainwindow.h"
#include"exception.h"
using namespace std;

class Exec_Exception;
class Statement;

class Program{
private:
    map<int,Statement*>bitmap;
    Ui::MainWindow* ui;
public:
    Program(){}
    ~Program();
    void insert(int line,Statement* sta){
        bitmap.insert(pair<int,Statement*>(line,sta));
    }
    Statement* get(int line){
        auto ret=bitmap.find(line);
        if(ret==bitmap.end())throw Exec_Exception("Runtime Error: access to a non-exist line");
        return ret->second;
    }
    void set_ui(Ui::MainWindow*u){ui=u;}
    Ui::MainWindow* get_ui(){return ui;}
    void exec();
    void generate_ast();
    void clear();

};


#endif // PROGRAM_H
