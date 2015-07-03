#include "widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    this->setWindowTitle("Compiler_C");
    this->setGeometry(300,300,1200,600);
    textEdit = new QTextEdit;
    textDebug = new QTextEdit;
    mainLay = new QVBoxLayout;

    mainMenu = new QMenuBar(this);
    fileMenu = new QMenu("Файл",this);
    correctMenu = new QMenu("Правка",this);
    runMenu = new QMenu("Приложение",this);
    aboutMenu = new QMenu("Справка",this);

    hlighter = new SyntaxHighlighter(this);
    hlighter->setDocument(textEdit->document());

    fileMenu->addAction(QIcon("icon\\new.ico"),"Новый",this,SLOT(NewFile()),Qt::CTRL+Qt::Key_N);
    fileMenu->addAction(QIcon("icon\\open.ico"),"Открыть",this,SLOT(OpenFile()),Qt::CTRL+Qt::Key_O);
    fileMenu->addAction(QIcon("icon\\save.ico"),"Сохранить",this,SLOT(SaveFile()),Qt::CTRL+Qt::Key_S);
    fileMenu->addAction("Сохранить как",this,SLOT(SaveAsFile()),Qt::CTRL+Qt::SHIFT+Qt::Key_S);
    fileMenu->addSeparator();
    fileMenu->addAction("Выход",this,SLOT(close()));
    mainMenu->addMenu(fileMenu);

    correctMenu->addAction(QIcon("icon\\undo.ico"),"Отменить ввод",textEdit,SLOT(undo()),Qt::CTRL+Qt::Key_Z);
    correctMenu->addAction(QIcon("icon\\redo.ico"),"Повторить ввод",textEdit,SLOT(redo()),Qt::CTRL+Qt::Key_Y);
    correctMenu->addSeparator();
    correctMenu->addAction("Вырезать",textEdit,SLOT(cut()),Qt::CTRL+Qt::Key_X);
    correctMenu->addAction(QIcon("icon\\copy.ico"),"Копировать",textEdit,SLOT(copy()),Qt::CTRL+Qt::Key_C);
    correctMenu->addAction("Вставить",textEdit,SLOT(paste()),Qt::CTRL+Qt::Key_V);
    correctMenu->addSeparator();
    correctMenu->addAction("Выделить все",textEdit,SLOT(selectAll()),Qt::CTRL+Qt::Key_A);
    mainMenu->addMenu(correctMenu);

    runMenu->addAction(QIcon("icon\\run.ico"),"Запуск",this,SLOT(Run()),Qt::CTRL+Qt::Key_R);
    mainMenu->addMenu(runMenu);

    aboutMenu->addAction(QIcon("icon\\about.ico"),"О программе",this,SLOT(About()),Qt::CTRL+Qt::Key_Q);
    mainMenu->addMenu(aboutMenu);
    mainLay->addWidget(mainMenu);
    mainLay->addWidget(textEdit);
    mainLay->addWidget(textDebug);
    mainLay->setStretch(1,10);
    mainLay->setMargin(0);
    mainLay->setSpacing(1);
    this->setLayout(mainLay);

    QFont bFont("Courier",14);
    bFont.setPointSize(14);
    textEdit->setFont(bFont);
    textEdit->setTabStopWidth(50);
    bFont.setPointSize(10);
    textDebug->setFont(bFont);
    QString start = "\nint main()\n{\n\n}";
    textEdit->setText(start);

    QString css = "QMenu,QMenuBar,QMenuBar::item{"
          "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
          "stop: 0 #E1E1E1, stop: 0.4 #DDDDDD,"
          "stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3);}";
    css += "QMenuBar::item:selected{"
           "background: gray;"
           "border: 1px inset gray;"
           "color:white}";
    css += "QMenu::item:selected{"
           "background: gray;"
           "color:white}";
    this->setStyleSheet(css);
}

Widget::~Widget()
{
    delete textEdit;
    delete textDebug;
    delete mainLay;

    delete mainMenu;

    delete fileMenu;
    delete correctMenu;
    delete runMenu;
    delete aboutMenu;
}

void Widget::NewFile()
{
    if(!fileName.isEmpty())
        this->SaveFile();
    this->setWindowTitle("Compiler_C");
    textEdit->clear();
    textDebug->clear();
    fileName.clear();
    QString start = "\nint main()\n{\n\n}";
    textEdit->setText(start);
}

void Widget::OpenFile()
{
    fileName = QFileDialog::getOpenFileName(this, "Открыть файл","",
            "Text Files (*.txt);;C Files (*.c)");

    if(!fileName.isEmpty())
    {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::critical(this, "Ошибка", "Невозможно открыть файл");
            return;
        }
        QTextStream in(&file);
        textEdit->setText(in.readAll());
        file.close();
        textDebug->clear();
        this->setWindowTitle("Compiler_C - "+fileName);
    }
}

void Widget::SaveFile()
{
    if(fileName.isEmpty())
        fileName = QFileDialog::getSaveFileName(this, "Сохранить файл","",
                "Text Files (*.txt);;C Files (*.c)");

    if(!fileName.isEmpty())
    {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::critical(this, "Ошибка", "Невозможно открыть файл");
            return;
        }
        QTextStream out(&file);
        out << textEdit->toPlainText();
        out.flush();
        file.close();

        this->setWindowTitle("Compiler_C - "+fileName);
    }
}

void Widget::SaveAsFile()
{
    fileName = QFileDialog::getSaveFileName(this, "Сохранить файл","",
                "Text Files (*.txt);;C Files (*.c)");

    if(!fileName.isEmpty())
    {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::critical(this, "Ошибка", "Невозможно открыть файл");
            return;
        }
        QTextStream out(&file);
        out << textEdit->toPlainText();
        out.flush();
        file.close();

        this->setWindowTitle("Compiler_C - "+fileName);
    }
}

void Widget::About()
{
    QMessageBox::about(this,"О программе","Автор:\nКутилкин Федор Валерьевич\nГруппа И332");
}

void Widget::Run()
{
    QString nameAsm;
    QString nameProg;
    if(fileName.isEmpty())
    {
        fileName = QDir::currentPath() + "/undefined.txt";
        nameProg = "undefined";
    }
    else
    {
        nameProg = fileName;
        QRegExp re(".:/.+/+");
        nameProg.remove(re);
        nameProg.remove(".txt");
        nameProg.remove(".c");
    }
    nameAsm = QDir::currentPath() + "/" + nameProg + ".asm";

    Lex lex;
    textDebug->clear();
    lex.analisys(textEdit->toPlainText()+" ");
    foreach(Error error,lex.getError())
        textDebug->append(QString::number(error.first)+" : Лексическая ошибка  " + error.second);
    if(!lex.getError().isEmpty())
        return;

    Syn syn(&lex);
    Generator gen;
    if(syn.parsing()&&syn.checkSemantic())
    {
        gen.setProgram(syn.getProgram(),syn.getTablesId(),syn.getTableFunc());
        gen.buildProgram();

        QFile file(nameAsm);
        if (!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::critical(this, "Ошибка", "Невозможно открыть файл");
            return;
        }
        QTextStream out(&file);
        out << gen.getAssembler();
        out.flush();
        file.close();

        QStringList args;
        QProcess process(this);
        args.clear();
        args << "/c" << "/coff" << nameAsm;
        process.start("ml",args);
        process.waitForFinished();
        process.close();
        textDebug->append("Message : Assemling finished with code is " + QString::number(process.exitCode()));
        if(process.exitCode()!=0)
            return;

        args.clear();
        nameAsm.replace(".asm",".obj");
        args << "/subsystem:console" << nameAsm;
        process.start("link",args);
        process.waitForFinished();
        process.close();
        textDebug->append("Message : Linking finished with code is " + QString::number(process.exitCode()));
        if(process.exitCode()!=0)
            return;

        args.clear();
        process.startDetached(nameProg);
        SaveFile();
        textDebug->append("Message : It's runing");
    }
    else
    {
        foreach(QString error,syn.getErrors())
            textDebug->append(error);
        textDebug->append("Message : Compile failed");
    }
}
