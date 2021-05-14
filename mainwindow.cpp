#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextBlock>
#include<QString>
#include<QFileDialog>
#include<string>
#include"parse.h"
#include"program.h"
#include<ExecThread.h>
#include <QWaitCondition>
#include <QMutex>
#include<QMessageBox>
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

//for syntax highlight
QList<QTextEdit::ExtraSelection> extras;
QList<QPair<unsigned int,QColor>> highlights;

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

void syntax_highlights_remove_cursor(Ui::MainWindow* ui,QTextCursor cursor){
    auto code=ui->codeDisplay;
    cursor.movePosition(QTextCursor::EndOfLine);
    int loc=0;
    for(auto &format:extras){
        if(format.cursor==cursor)format.format.clearBackground();
        loc++;
    }
    code->setExtraSelections(extras);
}


//insert a cmd in codeDisplay
//error do nothing
//0:correct -1:error
int insert_cmd(Ui::MainWindow* ui,QString& str){
    if(str==""||str=="\n")return 0;
    bool empty_cmd=false;
    empty_cmd=emptycmd(str); //empty command
    int line=getLineNum(str);
    if(line==-1)return -1;
    if(line>1000000)return -1;

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
    //move to the inserted line
    cursor.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor,high);

    //replace or delete
    if(replace){
        //remove the current line
        syntax_highlights_remove_cursor(ui,cursor);
        cursor.select(QTextCursor::LineUnderCursor);
        cursor.removeSelectedText();
        cursor.deleteChar();
        //auto code_str_debug=ui->codeDisplay->toPlainText().toStdString();
        ui->codeDisplay->setTextCursor(cursor);
        if(!empty_cmd){
            ui->codeDisplay->insertPlainText(str);
        }
    }   
    else if(!empty_cmd){
        ui->codeDisplay->setTextCursor(cursor);
        ui->codeDisplay->insertPlainText(str);
    }
    return 0;
}

QTextCursor search_line_cursor(Ui::MainWindow* ui,int line){
    int low=0;
    int high=ui->codeDisplay->document()->blockCount()-1;
    auto Displaydoc=ui->codeDisplay->document();
    while(low<high){
        int mid=(low+high)>>1;
        QTextBlock midblock=Displaydoc->findBlockByNumber(mid);
        QString str_mid=midblock.text();
        int mid_line=getLineNum(str_mid);
        if(line<mid_line)high=mid;
        else if(line>mid_line)low=mid+1;
        else {high=mid;break;}
    }
    //init the cursor
    auto cursor=ui->codeDisplay->textCursor();
    cursor.movePosition(QTextCursor::Start);
    //move to the inserted line
    cursor.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor,high);
    return cursor;
}

//enter a command the "enter key"
//codeDisplay will be updated
void MainWindow::on_cmdLineEdit_blockCountChanged(int newBlockCount)
{
    //ui->resDisplay->clear();//clear the error message ???
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
        if(insert_cmd(ui,str)==-1){
            ui->resDisplay->insertPlainText("Input Error!\n");
            prev_block_count=newBlockCount;
            return;
        }
//        try {insert_cmd(ui,str);}
//        catch(Input_Exception e){
//            ui->resDisplay->insertPlainText(QString::fromStdString(e.str));
//            prev_block_count=newBlockCount;
//            return;
//        }
        prev_block_count++;
    }
}



//clear the code
void MainWindow::on_btnClearCode_clicked()
{
    ui->resDisplay->clear();
    ui->treeDisplay->clear();
    ui->codeDisplay->clear();
    ui->debugDisplay->clear();
    highlights.clear();
    extras.clear();
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
{   if(!program.is_debug()){
        //clear the last result
        ui->resDisplay->clear();
        ui->treeDisplay->clear();
        ui->debugDisplay->clear();
        /*begin of ui operation*/
    //        program.clear();
//        extras.clear();
//        highlights.clear();
        ui->btnLoadCode->setEnabled(true);
        ui->btnClearCode->setEnabled(true);
    }
    else{
        ui->treeDisplay->clear();
    }
    //if debug mode click the run, do not clear!
    ExecThread *thread=new ExecThread(NULL);
    program.set_run_thread(thread);
    connect(thread,&QThread::finished
                ,thread,&QObject::deleteLater);
    connect(thread,&ExecThread::send_res_output,
            this,&MainWindow::set_res_output);
    connect(thread,&ExecThread::send_ast,
            this,&MainWindow::set_ast);
    connect(thread,&ExecThread::send_curvar,
            this,&MainWindow::set_curvar);
    connect(thread,&ExecThread::send_debug_message,
            this,&MainWindow::debug_message);
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

//    ui->btnClearCode->setEnabled(false);
//    ui->btnDebugStep->setEnabled(false);
//    ui->btnLoadCode->setEnabled(false);
//    ui->btnRunCode->setEnabled(false);

    mut.lock();
    cond.wait(&mut);
    mut.unlock();

//    ui->btnClearCode->setEnabled(true);
//    ui->btnDebugStep->setEnabled(true);
//    ui->btnLoadCode->setEnabled(true);
//    ui->btnRunCode->setEnabled(true);

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

void syntax_highlight(Ui::MainWindow*ui){
    extras.clear();
    QTextBrowser *code=ui->codeDisplay;
    QTextCursor cursor(code->document());
    for(auto &line:highlights){
        QTextEdit::ExtraSelection h;
        h.cursor=cursor;
        h.cursor.setPosition(line.first);
        h.cursor.movePosition(QTextCursor::StartOfLine);
        h.cursor.movePosition(QTextCursor::EndOfLine);
        h.format.setProperty(QTextFormat::FullWidthSelection,true);
        h.format.setBackground(line.second);
        extras.append(h);
    }
    code->setExtraSelections(extras);
}

void syntax_highlight_remove_line(Ui::MainWindow*ui,int line){
    auto code=ui->codeDisplay;
    auto cursor=search_line_cursor(ui,line);
    QTextEdit::ExtraSelection h;
    h.cursor=cursor;
    h.cursor.movePosition(QTextCursor::StartOfLine);
    h.cursor.movePosition(QTextCursor::EndOfLine);
    h.format.setProperty(QTextFormat::FullWidthSelection,true);
    h.format.clearBackground();
    int loc=0;
    for(auto &extra:extras){
        if(extra.cursor==h.cursor){
            extras.remove(loc);
            break;
        }
        ++loc;
    }
    code->setExtraSelections(extras);
}

void syntax_highlight_line(Ui::MainWindow*ui,int line){
    auto code=ui->codeDisplay;
    auto cursor=search_line_cursor(ui,line);
    QTextEdit::ExtraSelection h;
    h.cursor=cursor;
    h.cursor.movePosition(QTextCursor::StartOfLine);
    h.cursor.movePosition(QTextCursor::EndOfLine);
    h.format.setProperty(QTextFormat::FullWidthSelection,true);
    h.format.setBackground(QColor(100,255,100));
    extras.append(h);
    code->setExtraSelections(extras);
}

void ExecThread::run(){
    if(program.is_debug()){
        program.set_debug(false);
        goto parse_finished;
    }

    //LET PRINT INPUT
    if(Exec_Immediate)code_text="1 "+cmd_for_immediate_exec.toStdString();
    else code_text=ui->codeDisplay->document()->toPlainText().toStdString();

    //initial the scanner;
    pcur=0;
    //set the ui in program
    program.set_ui(ui);
    //set the debug flag
    program.set_debug(false);
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
        if(!Exec_Immediate)syntax_highlight(ui);
        //clear program
        program.clear();
        extras.clear();
        highlights.clear();
        return;
    }

parse_finished:
    if(program.is_empty())return;
    program.generate_ast();
    emit send_ast(ast);
    try {
        int status=program.exec();
        if(status==0){
            //may exit from debug mode
            ui->btnLoadCode->setEnabled(true);
            ui->btnClearCode->setEnabled(true);
        }
    }
    catch(Exec_Exception e){
        emit send_debug_message(e.str);
        emit send_res_output(e.str);
        ui->btnLoadCode->setEnabled(true);
        ui->btnClearCode->setEnabled(true);
    }
    //emit send_curvar(program.generate_curvar());
    //emit send_res_output(res_output);
    //clear program
    program.clear();
    extras.clear();
    highlights.clear();
}

//clear operation can be done in the runcode btn slot
void MainWindow::set_res_output(string res){
    /*begin of ui operation*/
    QString str=QString::fromStdString(res);
    ui->resDisplay->insertPlainText(str);
}

void MainWindow::set_ast(string ast_res){
    QString str=QString::fromStdString(ast_res);
    ui->treeDisplay->insertPlainText(str);
}

void MainWindow::set_curvar(string curvar){
    //because it regenerate every time, so it need to clear
    ui->debugDisplay->clear();
    QString str=QString::fromStdString(curvar);
    ui->debugDisplay->insertPlainText(str);
}

//similar to on_btnRunCode_clicked
void MainWindow::on_btnDebugStep_clicked()
{
    ui->btnLoadCode->setEnabled(false);
    ui->btnClearCode->setEnabled(false);
    if(!program.is_debug()){
        ui->resDisplay->clear();
        ui->debugDisplay->clear();
    }
    ui->treeDisplay->clear();

    DebugThread *thread=new DebugThread(NULL);
    program.set_debug_thread(thread);
    connect(thread,&QThread::finished
                ,thread,&QObject::deleteLater);
    connect(thread,&DebugThread::send_res_output,
            this,&MainWindow::set_res_output);
    connect(thread,&DebugThread::send_ast,
            this,&MainWindow::set_ast);
    connect(thread,&DebugThread::send_curvar,
            this,&MainWindow::set_curvar);
    connect(thread,&DebugThread::send_debug_message,
            this,&MainWindow::debug_message);
    thread->set_ui(ui);
    thread->start();

}

//mostly the same as ExecThread::run, do one time parsing
//TODO:show the runtime variable
void DebugThread::run(){
    if(!program.is_debug()){
        //firstly enter debug mode


        code_text=ui->codeDisplay->document()->toPlainText().toStdString();
        //initial the scanner;
        pcur=0;
        //set the ui in program
        program.set_ui(ui);
        //set the debug flag
        program.set_debug(true);
        st=CMDING;

        try{
            parse();
        }
        catch(Parse_Exception e){
            //need to show the ast
            program.generate_ast();
            //display the error message in resDisplay
//            emit send_res_output(e.str);
//            emit send_ast(ast);
            syntax_highlight(ui);
//            program.set_debug(false);
//            ui->btnLoadCode->setEnabled(true);
//            ui->btnClearCode->setEnabled(true);
//            return;
//            unable to clear
        }
    }


    if(program.is_empty()){
        program.set_debug(false);
        ui->btnLoadCode->setEnabled(true);
        ui->btnClearCode->setEnabled(true);
        //clear program
        program.clear();
        extras.clear();
        highlights.clear();
        return;
    }
    //program.generate_ast(); no need to generate
    //emit send_ast(ast);

    try {
        int status=program.exec();
        if(status==0){
            //FIXME: may need to do more thing
            emit send_debug_message("End of Debug.");
            program.set_debug(false);
            ui->btnLoadCode->setEnabled(true);
            ui->btnClearCode->setEnabled(true);
            //clear program
            program.clear();
            extras.clear();
            highlights.clear();
        }
    }
    catch(Exec_Exception e){
        emit send_debug_message(e.str);
        emit send_res_output(e.str);
        program.set_debug(false);
        ui->btnLoadCode->setEnabled(true);
        ui->btnClearCode->setEnabled(true);
        //clear program
        program.clear();
        extras.clear();
        highlights.clear();
        return;
    }
    //exec also generate ast for current code
    emit send_ast(ast);
    //emit send_curvar(program.generate_curvar());


}

void MainWindow::debug_message(std::string message){
    QString str=QString::fromStdString(message);
    QMessageBox::information(this,"Information",str,QMessageBox::Ok | QMessageBox::Cancel);
}

