#ifndef PROGRAM_H
#define PROGRAM_H

#include<map>
#include"statement.h"
#include"mainwindow.h"
#include"exception.h"
#include"ExecThread.h"
using namespace std;

class Exec_Exception;
class Statement;
class ExecThread;
class DebugThread;
class Program{
private:
    map<int,Statement*>bitmap;
    Ui::MainWindow* ui;
    bool debug;
    Statement* debug_cur;
    ExecThread* run_thread;
    DebugThread* debug_thread;
public:
    Program(){debug=false;debug_cur=nullptr;}
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
    void set_debug(bool flag){debug=flag;}
    void set_run_thread(ExecThread*thre){run_thread=thre;}
    void set_debug_thread(DebugThread*thre){debug_thread=thre;}
    bool is_debug(){return debug;}
    Ui::MainWindow* get_ui(){return ui;}
    int exec();
    void generate_ast();
    string generate_curvar();
    void clear();
    bool is_empty(){
        return bitmap.empty();
    }

};


#endif // PROGRAM_H
