#include "source/includes/mainwindow.h"
#include "source/includes/update_dbase.h"
#include "source/includes/helplistview.h"
#include "source/includes/antifreeze.h"

#include <QStandardItemModel>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    helpCtx = 0;

    installEventFilter(this);
    editor = ui->editorWidget;

    connect(ui->actionOpen_File,
            &QAction::triggered,
            this,
            &MainWindow::on_FileOpen);

    connect(ui->actionSpeichern,
            &QAction::triggered,
            this,
            &MainWindow::on_SaveFile);

    connect(ui->actionSpeichern_unter,
            &QAction::triggered,
            this,
            &MainWindow::on_SaveFileAs);

    connect(ui->actionAbout_Qt,
            &QAction::triggered,
            this,
            &MainWindow::on_AboutQt
            );

    connect(ui->actionEinstellungen,
            &QAction::triggered,
            this,
            &MainWindow::on_actionEinstellungen
            );

	show();


/*
    update = new update_dbase(dynamic_cast<QDialog*>(this));
    update->show();
    update->exec();*/
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    ui->editPage->resize(width(), height());
    QMainWindow::resizeEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton  btn;
    QMessageBox::StandardButtons btns=(
    QMessageBox::Yes |
    QMessageBox::Cancel); btn=
    QMessageBox::question(this,tr("Attention"),tr("You would like to Exit the application?"),btns);

    if (btn == QMessageBox::Yes)    { event->accept(); close(); } else
    if (btn == QMessageBox::Cancel) { event->ignore(); }

    if (editor->document()->isModified()) {
        editor->document()->setModified(true);
        on_FileOpen(1);
    }
}

bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {   QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        switch (keyEvent->key())
        {
        case Qt::Key_F1:
            { //  delEditPanel();
                addHelpPanel();
                return true;
            }
            break;


        case Qt::Key_Escape:
            { //  delHelpPanel();
              //  addEditPanel();
              //  return true;
            }
            close();
            return true;
            break;
        }
    }
    return QMainWindow::eventFilter(target,event);
}

void MainWindow::on_dockHelpOpen()
{ //  delEditPanel();
    addHelpPanel();
}

void MainWindow::on_dockEditOpen()
{ //  delHelpPanel();
  //  addEditPanel();
}

void MainWindow::delEditPanel() { /*if (helpWindow) helpWindow->show(); ui->editPage->hide(); */ }
void MainWindow::addEditPanel() { /*if (helpWindow) helpWindow->hide(); ui->editPage->show(); */ }

void MainWindow::delHelpPanel() { /* helpWindow->hide();*/ }
void MainWindow::addHelpPanel()
{
	QFileInfo info(
		QString(
			QApplication::applicationDirPath() +
			"/help/dBaseHelp.qhc"
		)
	);

	if (info.exists() == false) {
        QMessageBox::critical(this,
        tr("Applicaton Error"),
        tr("Helpfile not found."));
        return;
	}

	helpEngine = new QHelpEngine(info.absoluteFilePath(),this);
    if (helpEngine->setupData() == false) {
        delete helpEngine;
        QMessageBox::critical(this,
        tr("Applicaton Error"),
        tr("Helpfile not found."));
        return;
    }

	QHelpContentModel   * contentModel	= helpEngine->contentModel	();
    QHelpContentWidget  * contentWidget = helpEngine->contentWidget ();
    QHelpIndexModel 	* indexModel	= helpEngine->indexModel	();
    QHelpIndexWidget	* indexWidget	= helpEngine->indexWidget	();

	if (indexModel == nullptr)
	QMessageBox::critical(0,"Error", "indexModel null");

    QSplitter   * m_pannel = new QSplitter(Qt::Horizontal);
	HelpBrowser * m_phtml  = new HelpBrowser(helpEngine,this);

	m_phtml->setSource(QUrl("qthelp://dbase.center"
	"/doc/index.html"));

    m_pannel->insertWidget(0, contentWidget);
    m_pannel->insertWidget(1, m_phtml      );
	m_pannel->show();

	connect(contentWidget,
	        SIGNAL(linkActivated(const QUrl &)),
	        m_phtml, SLOT(setSource(const QUrl &)));

	qDebug() << "Help ok";
}

void MainWindow::on_AboutQt() {
    QApplication::aboutQt();
}

void MainWindow::on_SaveFile()
{
    if (editor->document()->isModified()) {
        editor->document()->setModified(false);
        QString fname = editor->documentTitle();
        if (fname.length() < 1) {
            fname = QFileDialog::getSaveFileName(
            this,
            tr("Save File"),
            QString(QDir::homePath() + fname),
            "Forms *.frm (*.frm);;Programs *.prg (*.prg);;All Files *.* (*.*)");
            if (fname.length() > 1) {
                QFile f(fname); f.open(
                QIODevice::WriteOnly  |
                QIODevice::Truncate   |
                QIODevice::Text);

                QString src = editor->document()->toPlainText();

                QTextStream out(&f);
                out.setCodec("UTF-8");
                out.setGenerateByteOrderMark(false);
                out << src;

                f.close();
            }
        }
    }
}

void MainWindow::on_SaveFileAs()
{
    //if (editor->document()->isModified())
    {
        editor->document()->setModified(false);
        QString fname = editor->documentTitle();
        if (fname.length() < 1) {
            fname = QFileDialog::getSaveFileName(
            this,
            tr("Save File As ..."),
            QString(QDir::homePath() + fname),
            "Forms *.frm (*.frm);;Programs *.prg (*.prg);;All Files *.* (*.*)");
            if (fname.length() > 1) {
                QFile f(fname); f.open(
                QIODevice::WriteOnly  |
                QIODevice::Truncate   |
                QIODevice::Text);

                QString src = editor->document()->toPlainText();

                QTextStream out(&f);
                out.setCodec("UTF-8");
                out.setGenerateByteOrderMark(false);
                out << src;

                f.close();
            }
        }
    }
}


void MainWindow::on_FileOpen(int state)
{
    if (editor->document()->isModified()) {
        editor->document()->setModified(false );
        QString fname = editor->documentTitle();
        if (fname.length() < 1) {
            fname = QFileDialog::getSaveFileName(
            this,
            tr("Save File"),
            QString(QDir::homePath() + fname),
            "Forms *.frm (*.frm);;Programs *.prg (*.prg);;All Files *.* (*.*)");

            //std::string fn = fname.toStdString();
            //if ((fn.substr(fn.find_last_of(".")+1) == "prg") == false) {
            //fname.append(".prg");
        }
        QFile f(fname); f.open(
        QIODevice::WriteOnly  |
        QIODevice::Truncate   |
        QIODevice::Text);

        QString src =
        editor->document()->toPlainText();

        QTextStream out(&f);
        out.setCodec("UTF-8");
        out.setGenerateByteOrderMark(false);
        out << src;

        f.close();
    }   if (state == 1)  return ;
    QString dname = QDir::homePath();
    QString fname = QFileDialog::getOpenFileName(
        this,
        tr("Open File"),
        dname,
        "Forms *.frm (*.frm);;Programs *.prg (*.prg);;All Files *.* (*.*)");

    if (fname.size() < 1)
    return;

    /*
    std::string fn = fname.toStdString();

    if ((fn.substr(fn.find_last_of(".")+1) == "prg") == true) { fname.append(".prg"); }
    if ((fn.substr(fn.find_last_of(".")+1) == "frm") == true) { fname.append(".frm"); }
    */

    editor->setDocumentTitle(fname);

    QFile f(fname);
    f.open(QIODevice::ReadOnly | QIODevice::Text); QByteArray barr=
    f.readAll();
    f.close();

    editor->clear();
    editor->insertPlainText(barr.data());
    editor->document()->setModified(false);
}

void MainWindow::on_actionBeenden_triggered()
{
    if (editor->document()->isModified())
    on_FileOpen(1);
    close();
}

void MainWindow::on_MainWindow_clicked()
{
    on_dockEditOpen();
}

void MainWindow::on_actionEinstellungen()
{
    SettingsDialog *se = new SettingsDialog(this);
    se->show();
    se->exec();

    delete  se;
}
