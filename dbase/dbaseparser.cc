#include <iostream>
#include <cctype>
using namespace std;

#include <string>
#include <cstdlib>
#include <cxxabi.h>

#include "dbasevisitor.h"
#include "dbaseaddnumber.h"
#include "dbasemulnumber.h"
#include "dbaseassign.h"
#include "dbasedownvisitor.h"

#include <QDebug>

#include <QString>
#include <QFile>
#include <QStack>
#include <QByteArray>
#include <QMessageBox>
#include <QTranslator>

#include "source/includes/mainclass.h"
#include "source/includes/dbaseexception.h"

// ------------------------------------------------------
// "catch" newline feeds, and try to handle next line ...
// ------------------------------------------------------
class dBaseNewLine {public:
      dBaseNewLine() {}  };

class dBaseEndOfProgram {public:
      dBaseEndOfProgram() {}  };

class dBaseEndOfComment {public:
      dBaseEndOfComment() {}  };

enum enumMathCommand {
    ncMUL
};
enum enumClassType {
    ctAssign,
    ctAssignADD,
    ctAssignSUB,
    ctAssignDIV
};


enum enumStaticType {
    CHAR,
    SHORT,
    INT,
    LONG,
    stFLOAT,
    DOUBLE,
    ARRAY
};

const int TOKEN_IS_NUMBER     = 2;
const int TOKEN_IS_LETTER     = 3;

int line_no;
QFile * srcfile = nullptr;

class dBaseParser {
public:
    virtual void start() {
        QString codestart = "  ssds =  44* 2 ; \n    ** /* dsdas dass d asd d */ xxx = 1223"
        "\n\n// dsgfdsdsg && gfegefg xcyxcc = 111\n"
        "sss = 1212"
        " car = 133.122";
        
        codestart = codestart.replace("\n",
        QString("\n# line %1\n").arg(++line_no));
        
        QStringList codelineList;
        QString     codeline;
        int rc = 0;
        int k = 0;

        QString substr = "# line 1";
        line_no = 1;
        while (codestart.indexOf(substr) != -1) {        
            codestart.replace(codestart.indexOf(substr), substr.length(),
            QString("# line %1").arg(line_no));
            ++line_no;
        }
        line_no = 1;
        
        codelineList = codestart.split(QRegExp("[\n]+"));
        
        for (int i = 0; i < codelineList.count(); i++)
        {
            codeline = codelineList.at(i);
            codeline = codeline.trimmed();
            
            if (codeline.at(0) == '#') {
                //qDebug() << codeline;
                continue;
            }   else
            if (codeline.at(0) == '/'
            &&  codeline.at(1) == '/') {
                //qDebug() << "skip: " << codeline;
                continue;
            }   else
            if (codeline.at(0) == '*'
            &&  codeline.at(1) == '*') {
                //qDebug() << "skip: " << codeline;
                continue;
            }   else
            if (codeline.at(0) == '&'
            &&  codeline.at(1) == '&') {
                //qDebug() << "skip: " << codeline;
                continue;
            }   else
            if (codeline.at(0).isLetter()) {
                //qDebug() << "letter: " << codeline;
                QStringList lstr = codeline.split(QRegExp("\\s+"));
                
                qDebug() << "------------------------";
                qDebug() << lstr;
                
                QString cl;
                QStringList lstr2;
                for (int j = 0; j < lstr.count(); j++) {
                    cl = lstr.at(j);
                    if (cl.contains("*")) {
                        cl.remove(QChar('*'));
                        lstr2.append(cl);
                        lstr2.append("*");
                    }   else
                    lstr2.append(cl);
                }
                
                lstr.clear();
                for (int j = 0; j < lstr2.count(); j++) {
                    cl = lstr2.at(j);
                    if (cl.contains(";")) {
                        cl.remove(QChar(';'));
                        lstr.append(cl);
                        lstr.append(";");
                    }   else
                    lstr.append(cl);
                }
                qDebug() << "-==> " << lstr;
                continue;
            }   else
            if (codeline.at(0).isDigit()) {
                qDebug() << "number: " << codeline;
                continue;
            }   else  {
                rc = 1;
                for (int j = 0; j < codeline.length(); j++)
                {
                    rc = 1;
                    k  = j;
                    if (codeline.at(k) == ' ') {
                        while (1) {
                            if (codeline.at(++k) == ' ') {
                                ++rc;
                                continue;
                            }
                            break;
                        }
                        codeline =
                        codeline.remove(0,rc);
                        qDebug() << codeline;
                        break;
                    }
                }
            }
        }
        
        //qDebug()  << "==> " << codelineList;

        qDebug() << "===================";
        qDebug() << "parser start debase";
        
        qDebug() << "ende";
    }

private:
};

template <class T>
class Parser {
public:
    QString getParserClassName()
    {
        int status;
        std::string tname = typeid(T).name();
        char *demangled_name = abi::__cxa_demangle(tname.c_str(), NULL, NULL, &status);
        if(status == 0) {
            tname = demangled_name;
            std::free(demangled_name);
        }   
        return QString(tname.c_str());
    }
    void start() {
        QString pc = getParserClassName();
        if (pc == "dBaseParser") {
            dBaseParser * pc = new
            dBaseParser ; pc->start();
        }
    }
};

void testParser()
{
    Parser<dBaseParser> dbasep;
    dbasep.start();
}

void dBaseAssign   ::accept(class dBaseVisitor &v) { v.visit(this); }
void dBaseADDnumber::accept(class dBaseVisitor &v) { v.visit(this); }
void dBaseMULnumber::accept(class dBaseVisitor &v) { v.visit(this); }

void testAST()
{    
    QVector<dBaseComando *> cmds;
    
    dBaseComando * cmd1 = new dBaseAssign("varA",12)  ; cmds.append(cmd1);
    dBaseComando * cmd2 = new dBaseADDnumber(cmd1,15) ; cmds.append(cmd2);
    
    dBaseComando * cmd3 = new dBaseAssign   ("varB", 8) ; cmds.append(cmd3);
    dBaseComando * cmd4 = new dBaseMULnumber(cmd3,   4) ; cmds.append(cmd4);
    
    dBaseDownVisitor down;
    for (int i = 0 ; i < cmds.count(); i++) {
        cmds.at(i)->accept(down);
    }
}


// --------------------------------
// reset variables for next run ...
// --------------------------------
void reset_program()
{
    line_no = 1;
}

void run_dbase_code()
{
    try
    {   if (w == nullptr)
        throw new dBaseError("window object memory is null");
    
        if (w->ui->EditorWidget == nullptr)
        throw new dBaseError("editor object is null");
        
        QString fname = w->ui->EditorWidget->documentTitle();
        if (fname.size() < 1) {
            fname = QApplication::applicationDirPath() + "/unnamed.prg";
            w->ui->EditorWidget->setDocumentTitle(fname);
            w->on_SaveFileAs();

            fname = w->ui->EditorWidget->documentTitle();
            qDebug() << "name: " << fname;
        }
        else {
            w->on_SaveFile();
        }

        qDebug() << "filer: " << fname;
        
        QFileInfo info(fname);
        if (!info.exists(fname))
        throw new dBaseError("source file can not be open.");

        srcfile = new QFile(fname);
        srcfile->open(QIODevice::ReadOnly | QIODevice::Text);
        srcfile->seek(0);

        reset_program ();
        testAST();
        
        srcfile->close();
        qDebug() << "ende: " << line_no;
    }
    catch (dBaseEndOfProgram *e) {
        Q_UNUSED(e);
        QMessageBox::information(w,"Information",
        QString("End of Program.\nParsed lines: %1").arg(line_no));

        srcfile->close();
        return;
    }
    catch (dBaseError *e) {
        if ((srcfile != nullptr)
        &&  (srcfile->isOpen()))
        srcfile->close();
        
        QMessageBox::critical(w,"Error",
        QString("%1\n%2")
        
        .arg(e->m_message)
        .arg("\nLine: %1")
        .arg(line_no));
    }
    catch (...) {
        srcfile->close();
        QMessageBox::critical(w,
        "Error",
        "unknown error");
    }
}
