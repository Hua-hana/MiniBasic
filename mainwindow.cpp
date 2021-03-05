#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextBlock>
#include<QString>
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
    int ret=num_str.toInt();
    if(ret)return ret;//success
    else return -1;//convert fail
}

//the empty command, e.g. 100
bool emptycmd(QString &str){
    QStringList strlist=str.split(" ");
    if(strlist.length()<2)return true;
    else return false;
}

//enter a command the "enter key"
//codeDisplay will be updated
void MainWindow::on_cmdLineEdit_blockCountChanged(int newBlockCount)
{
    //get the updated line
    int curBlockCount=newBlockCount-1;
    QTextDocument* doc=ui->cmdLineEdit->document();
    QTextBlock blocktext=doc->findBlockByNumber(curBlockCount-1);
    bool empty_cmd=false;
    QString str=blocktext.text()+"\n";
    if(emptycmd(str))empty_cmd=true; //empty command

    //insert in codeDisplay
    //binary search
    int line=getLineNum(str);
    int low=0;
    int high=ui->codeDisplay->document()->blockCount()-1;
    bool replace=false;//bring out of while loop
    while(low<high){
        int mid=(low+high)>>1;
        QTextBlock midblock=doc->findBlockByNumber(mid);
        QString str_mid=midblock.text();
        int mid_line=getLineNum(str_mid);//wrong! cannot get 200 line number,no " "
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
    else{
        cursor.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor,high);
        ui->codeDisplay->setTextCursor(cursor);
        ui->codeDisplay->insertPlainText(str);
    }
}



