#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextBlock>
#include<QString>
#include<QFileDialog>
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
    int low=0;
    int high=ui->codeDisplay->document()->blockCount()-1;
    bool replace=false;//bring out of while loop
    auto Displaydoc=ui->codeDisplay->document();
    while(low<high){
        int mid=(low+high)>>1;
        QTextBlock midblock=Displaydoc->findBlockByNumber(mid);
        QString str_mid=midblock.text();
        int mid_line=getLineNum(str_mid);
        //if(mid_line==-1);//TODO convert fail
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
    //get the updated line
    int curBlockCount=newBlockCount-1;
    QTextDocument* doc=ui->cmdLineEdit->document();
    QTextBlock blocktext=doc->findBlockByNumber(curBlockCount-1);
    QString str=blocktext.text()+"\n";

    //insert cmd
    insert_cmd(ui,str);
}



//clear the code
void MainWindow::on_btnClearCode_clicked()
{
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
