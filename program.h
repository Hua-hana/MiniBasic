#ifndef PROGRAM_H
#define PROGRAM_H

#include<map>
#include"statement.h"
#include"mainwindow.h"
using namespace std;


class Statement;

class Program{
private:
    map<int,Statement*>bitmap;
    Ui::MainWindow* ui;
public:
    Program(){}
    ~Program(){}
    void insert(int line,Statement* sta){
        bitmap.insert(pair<int,Statement*>(line,sta));
    }
    Statement* get(int line){
        auto ret=bitmap.find(line);
        assert(ret!=bitmap.end());
        return ret->second;
    }
    void set_ui(Ui::MainWindow*u){ui=u;}
    Ui::MainWindow* get_ui(){return ui;}
    void exec();
    void generate_ast();
    void clear(){
        bitmap.clear();
    }

};


#endif // PROGRAM_H
