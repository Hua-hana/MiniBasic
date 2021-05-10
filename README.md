# A MiniBasic Interpreter
**(Course project for SE2322-Advanced Data Structure)** 

<img src=./pic/demo.png width=60%>

You can write some "Ancient Language--Basic" code to feed the "Interpreter", which possibly give you a right answer :)

The feature of the Interpreter are:
* Error Syntax Highlight
* Single Step debug
* Support two types (int and string, wow!)
* AST Printer
* A Trivial Type Check (there are only two type :)
* TODO...

**Error Highlight**

<img src=./pic/demo1.jpg width=60%>

**Single Step Debug**

<img src=./pic/single_step.jpg width=60%>

## How to run
You should use QT to open the project. I will update it when I am idle...

## File Structure

* ```mainwindow.cpp``` The UI and slot functions for executing the code
* ```tokenizer.cpp``` A simple lexical analyzer
* ```parse.cpp``` A simpl top-down parser, which can do trivial type-checking and variable defined checking.
* ```exp.cpp``` The ```Expression``` Class represing the arithmetic expression
* ```statement.cpp``` The Basic language statement, such as ```LET```, ```IF```, ```GOTO``` statements
* ```ExecThread.h``` Use multithread programming. It generate a new thread to execute the code, avoiding blocking the UI
* ```program.cpp``` Store the program(code), and provide the interface for UI to execute the code
* ```evalstate.h``` The runtime environment class 
* ```test/*``` Some classified test files
* ```mainwindow.ui``` The QT generated file, displaying the UI

