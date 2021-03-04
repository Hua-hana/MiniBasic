#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextBlock>
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



void MainWindow::on_cmdLineEdit_blockCountChanged(int newBlockCount)
{
    int curBlockCount=newBlockCount-1;
    QTextDocument* doc=ui->cmdLineEdit->document();
    QTextBlock blocktext=doc->findBlockByNumber(curBlockCount-1);
    QString str=blocktext.text()+"\n";
    ui->codeDisplay->insertPlainText(str);
}
