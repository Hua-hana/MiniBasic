#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_cmdLineEdit_blockCountChanged(int newBlockCount);

    void on_btnClearCode_clicked();

    void on_btnLoadCode_clicked();

    void on_btnRunCode_clicked();

    void on_btnDebugStep_clicked();

public slots:
    void set_res_output(std::string res);

    void set_ast(std::string ast);

    void set_curvar(std::string curvar);

    void debug_message(std::string message);

private:
    Ui::MainWindow *ui;
};

void syntax_highlight_remove_line(Ui::MainWindow*ui,int line);
void syntax_highlight_line(Ui::MainWindow*ui,int line);



#endif // MAINWINDOW_H
