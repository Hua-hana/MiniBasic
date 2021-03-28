#ifndef EXECTHREAD_H
#define EXECTHREAD_H

#include<QThread>
#include<program.h>
#include<QDebug>


class ExecThread:public QThread{
    Q_OBJECT
signals:
    void send_res_output(string);
    void send_ast(string);
public:
    ExecThread(QObject* par):QThread(par){}
    ~ExecThread(){qDebug() << "ExecThread::~ExecThread()";}
    void set_ui(Ui::MainWindow* u){
        ui=u;
    }
    void run();

private:
    Ui::MainWindow* ui;
};


#endif // EXECTHREAD_H
