#include "source/includes/mainwindow.h"
#include "source/includes/update_dbase.h"

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

    update = new update_dbase(dynamic_cast<QWidget*>(this));
    update->show();
    update->exec();
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
            {   delEditPanel();
                addHelpPanel();
                return true;
            }
            break;

        case Qt::Key_Escape:
            {   delHelpPanel();
                addEditPanel();
                return true;
            }
            close();
            return true;
            break;
        }
    }
    return QMainWindow::eventFilter(target,event);
}

void MainWindow::on_dockHelpOpen()
{   delEditPanel();
    addHelpPanel();
}

void MainWindow::on_dockEditOpen()
{   delHelpPanel();
    addEditPanel();
}

void MainWindow::delEditPanel() { if (helpWindow) helpWindow->show(); ui->editPage->hide(); }
void MainWindow::addEditPanel() { if (helpWindow) helpWindow->hide(); ui->editPage->show(); }

void MainWindow::delHelpPanel() { helpWindow->hide(); }
void MainWindow::addHelpPanel()
{
    helpEngine= new QHelpEngine(
    QApplication::applicationDirPath() +
    "/help/dBaseHelp.qhc",dynamic_cast<QMainWindow*>(w));

    bool flag= helpEngine->setupData();
    if (!flag) {
        delete helpEngine;
        helpEngine = new QHelpEngine(
        "/help/dBaseHelp.qhc",this);

        if (!(flag = helpEngine->setupData())) {
            delete helpEngine;
            QMessageBox::critical(this,
            tr("Applicaton Error"),
            tr("Helpfile not found."));
            return;
        }
    }

    QTabWidget * tWidget = new QTabWidget;
    tWidget->addTab(helpEngine->contentWidget(),tr("Content"));
    tWidget->addTab(helpEngine->indexWidget  (),tr("Index"));

    tWidget->widget(0)->resize(200,height());

    HelpBrowser * textViewer = new HelpBrowser(helpEngine);
    textViewer->setSource(QUrl("qthelp://kallup.net/doc/index.html"));

    connect(helpEngine->contentWidget(),
            SIGNAL(linkActivated(QUrl)),
            textViewer,
            SLOT(setSource(QUrl))
            );

    connect(helpEngine->indexWidget(),
            SIGNAL(linkActivated(QUrl, QString)),
            textViewer,
            SLOT(setSource(QUrl))
            );

    hsplit = new QSplitter(Qt::Horizontal);
    hsplit->insertWidget(0,tWidget);
    hsplit->insertWidget(1,textViewer);

    if (!helpWindow) {
        helpWindow          = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(helpWindow);

        layout->setSizeConstraint(QLayout::SetMinimumSize);
        layout->addWidget(hsplit);

        helpWindow->setLayout(layout);
        helpWindow->move(0,ui->menubar->fontMetrics().height()+2);
        helpWindow->resize(width(),height());
    }   helpWindow->show();
}

void MainWindow::on_AboutQt() {
    QApplication::aboutQt();
}

void MainWindow::on_FileOpen(int state)
{
    if (editor->document()->isModified()) {
        editor->document()->setModified(false );
        QString fname = editor->documentTitle();
        if (fname.size() < 1) {
            fname = QFileDialog::getSaveFileName(
            this,
            tr("Save File"),
            QString(QDir::homePath() + QString("/test.prg")),
            "Programs *.prg (*.prg);;All Files *.* (*.*)");

            std::string fn = fname.toStdString();
            if ((fn.substr(fn.find_last_of(".")+1) == "prg") == false) {
            fname.append(".prg");
            }   }
        {   QFile f(fname); f.open(
            QIODevice::WriteOnly  |
            QIODevice::Truncate   |
            QIODevice::Text);

            QString src = editor->document()->toPlainText();
            f.write(src.toLatin1().data(),src.size());
            f.close();
        }
    }       if (state == 1)  return ;
    QString dname = QDir::homePath();
    QString fname = QFileDialog::getOpenFileName(
            this,
            tr("Open File"),
            dname,
            "Programs *.prg (*.prg);;All Files *.* (*.*)");

    if (fname.size() < 1)
    return;

    std::string fn = fname.toStdString();
    if ((fn.substr(fn.find_last_of(".")+1) == "prg") == false)
    fname.append(".prg");

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
