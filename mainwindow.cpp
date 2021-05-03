#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextBlock>
#include<QString>
#include<QFileDialog>
#include<string>
#include<QTimer>
#include"parse.h"
#include"program.h"
#include<ExecThread.h>
#include <QWaitCondition>
#include <QMutex>
#include"exception.h"
std::string code_text;

Program program;
extern string res_output;
extern string ast;
extern unsigned int pcur;
enum MachineState{WAIT_INPUT,CMDING};
static MachineState st=CMDING;
bool Exec_Immediate=false;
QString cmd_for_immediate_exec;
//used for input blocking
QWaitCondition cond;
QMutex mut;
//QMutex cmd_mutex;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//input a string, get the line number
int getLineNum(QString &str){
    QString num_str=str.section(" ",0,0);
    if(num_str=="")num_str=str;
    int ret=num_str.toInt();
    if(ret)return ret;//success
    else return -1;//convert fail
}

//the empty command, e.g. 100
bool emptycmd(QString &str){
    QString cmd_str=str.section(" ",1,1);
    if(cmd_str=="")return true;
    else return false;
}

//insert a cmd in codeDisplay
//error do nothing
void insert_cmd(Ui::MainWindow* ui,QString& str){
    bool empty_cmd=false;
    empty_cmd=emptycmd(str); //empty command
    int line=getLineNum(str);
    if(line==-1)throw Input_Exception("Input Error: no line number!");
    if(line>1000000)throw Input_Exception("Input Error: line number is too large!");

    int low=0;
    int high=ui->codeDisplay->document()->blockCount()-1;
    bool replace=false;//bring out of while loop
    auto Displaydoc=ui->codeDisplay->document();
    while(low<high){
        int mid=(low+high)>>1;
        QTextBlock midblock=Displaydoc->findBlockByNumber(mid);
        QString str_mid=midblock.text();
        int mid_line=getLineNum(str_mid);
        if(line<mid_line)high=mid;
        else if(line>mid_line)low=mid+1;
        else {replace=true;high=mid;break;}
    }
    //init the cursor
    auto cursor=ui->codeDisplay->textCursor();
    cursor.movePosition(QTextCursor::Start);

    //replace or delete
    if(replace){
        //remove the current line
        cursor.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor,high);
        cursor.select(QTextCursor::LineUnderCursor);
        cursor.removeSelectedText();
        cursor.deleteChar();
        ui->codeDisplay->setTextCursor(cursor);
        if(!empty_cmd)ui->codeDisplay->insertPlainText(str);
    }
    else if(!empty_cmd){
        cursor.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor,high);
        ui->codeDisplay->setTextCursor(cursor);
        ui->codeDisplay->insertPlainText(str);
    }
}


//enter a command the "enter key"
//codeDisplay will be updated
void MainWindow::on_cmdLineEdit_blockCountChanged(int newBlockCount)
{
    ui->resDisplay->clear();//clear the error message
    static int prev_block_count=0;
    if(st==WAIT_INPUT){
        cond.wakeAll();
        prev_block_count++;//usually forget
        return;
    }


    //get the updated line
    while(prev_block_count<newBlockCount-1){
        QTextDocument* doc=ui->cmdLineEdit->document();
        QTextBlock blocktext=doc->findBlockByNumber(prev_block_count);
        QString str=blocktext.text();

        QString cmd_str=str.section(" ",0,0);
        //extra command
        if(cmd_str=="LIST"){prev_block_count++;return;}
        if(cmd_str=="RUN"){
            on_btnRunCode_clicked();
            prev_block_count++;
            return;
        }
        if(cmd_str=="LOAD"){
            on_btnLoadCode_clicked();
            prev_block_count++;
            return;
        }
        if(cmd_str=="CLEAR"){
            on_btnClearCode_clicked();
            prev_block_count++;
            return;
        }
        if(cmd_str=="QUIT"){
            exit(0);
        }
        if(cmd_str=="HELP"){
            ui->resDisplay->insertPlainText("HELP ME!!");
            prev_block_count++;
            return;
        }

        str+="\n";
        //LET PRINT INPUT without line number
        if(cmd_str=="PRINT"||cmd_str=="LET"||cmd_str=="INPUT"){
            cmd_for_immediate_exec=str;
            Exec_Immediate=true;
            on_btnRunCode_clicked();
            //two thread run if not return
            prev_block_count++;//!!
            return;
        }

        //insert cmd
        try {insert_cmd(ui,str);}
        catch(Input_Exception e){
            ui->resDisplay->insertPlainText(QString::fromStdString(e.str));
            prev_block_count++;
            return;
        }
        prev_block_count++;
    }
}



//clear the code
void MainWindow::on_btnClearCode_clicked()
{
    ui->resDisplay->clear();
    ui->treeDisplay->clear();
    ui->codeDisplay->clear();
}

// load the code file
void MainWindow::on_btnLoadCode_clicked()
{
    QFileDialog *f=new QFileDialog(this);
    f->setWindowTitle("select the code");
    f->setViewMode(QFileDialog::Detail);

    //select file
    QString filepath;
    if(f->exec()){
        auto filelist=f->selectedFiles();
        filepath=filelist[0];
    }
    else return;

    QFile codefile(filepath);
    if(!codefile.open(QIODevice::ReadOnly|QIODevice::Text))return;

    QTextStream textstream(&codefile);

    while(!textstream.atEnd()){
        QString str=textstream.readLine()+'\n';
        insert_cmd(ui,str);
    }
}

//execute the code
void MainWindow::on_btnRunCode_clicked()
{

    ExecThread *thread=new ExecThread(NULL);
    connect(thread,&QThread::finished
                ,thread,&QObject::deleteLater);
    connect(thread,&ExecThread::send_res_output,
            this,&MainWindow::set_res_output);
    connect(thread,&ExecThread::send_ast,
            this,&MainWindow::set_ast);
    thread->set_ui(ui);
    thread->start();

    //parse
    //exec
    //set output
}

//wait for the input
//return a string, can be transform to int or string
string var_input(Ui::MainWindow*ui){
    st=WAIT_INPUT;
    //QEventLoop loop;

    ui->cmdLineEdit->insertPlainText("? ");

    mut.lock();
    cond.wait(&mut);
    mut.unlock();

    int curBlockCount=ui->cmdLineEdit->blockCount()-1;
    QTextDocument* doc=ui->cmdLineEdit->document();
    QTextBlock blocktext=doc->findBlockByNumber(curBlockCount-1);
    QString str=blocktext.text();
    int low=0;
    while(low<str.length()&&(str[low]=='?'||str[low]==' '||str[low]=='\t'))++low;
    if(low==str.length())throw Exec_Exception("Runtime Error: input invalid");

    int high=low;
    while(high<str.length()&&str[high]!=' '&&str[high]!='\t')++high;

    st=CMDING;
    return str.toStdString().substr(low,high-low+1);
}

void ExecThread::run(){
    /*begin of ui operation*/
    program.clear();
    //LET PRINT INPUT
    if(Exec_Immediate)code_text="1 "+cmd_for_immediate_exec.toStdString();
    else code_text=ui->codeDisplay->document()->toPlainText().toStdString();
    Exec_Immediate=false;//click the run button and not exec the cmd_for_imm..._exec
    //initial the scanner;
    pcur=0;
    //set the ui in program
    program.set_ui(ui);
    st=CMDING;

    /*end of ui operation */


    try {
        parse();

    }
    catch(Parse_Exception e){
        //need to show the ast
        program.generate_ast();
        emit send_res_output(e.str);
        emit send_ast(ast);
        return;
    }

    if(program.is_empty())return;
    program.generate_ast();
    emit send_ast(ast);
    try {
        program.exec();
    }
    catch(Exec_Exception e){
        emit send_res_output(e.str);
        return;
    }

    emit send_res_output(res_output);


}

void MainWindow::set_res_output(string res){
    /*begin of ui operation*/
    ui->resDisplay->clear();
    QString str=QString::fromStdString(res);
    ui->resDisplay->insertPlainText(str);
}

void MainWindow::set_ast(string ast_res){
    QString str=QString::fromStdString(ast_res);
    ui->treeDisplay->clear();
    ui->treeDisplay->insertPlainText(str);
}


void MainWindow::on_btnDebugStep_clicked()
{

}
