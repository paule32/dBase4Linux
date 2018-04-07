#include "source/includes/mainwindow.h"
#include "source/includes/update_dbase.h"
//#include "source/includes/helplistview.h"
//#include "source/includes/antifreeze.h"

#include <QDesktopServices>
#include <QStandardItemModel>
#include <QStatusBar>

#include "source/includes/dbaseexception.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    helpCtx = 0;

    installEventFilter(this);
    //editor = ui->editorWidget;

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


    connect(ui->toolButton_Project,
            &customContextMenuRequested,
            this,
            &MainWindow::ShowContextMenuProject);

    
    connect(ui->toolButton_Welcome,
            &QToolButton::clicked,
            this,
            &MainWindow::on_toolButton_8_clicked
            );
            
    connect(ui->toolButton_Home,
            &QToolButton::clicked,
            this,
            &MainWindow::on_toolButton_Home_clicked
            );
                   
    ui->DesignerTabWidget->setVisible(false);
    
    ui->ErrorOutputWidget->setColumnWidth(0,24);
    ui->ErrorOutputWidget->setColumnWidth(1,450);
    ui->ErrorOutputWidget->setColumnWidth(2,140);
    ui->ErrorOutputWidget->setColumnWidth(3,64);
    
    ui->ErrorOutputWidget->setRowHeight(0,22);
    
    addHelpPanel();
    
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
//    ui->editPage->resize(width(), height());
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

    if (ui->EditorWidget->document()->isModified()) {
        ui->EditorWidget->document()->setModified(true);
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
        tr("Helpfile not found2."));
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

	m_phtml->setSource(QUrl("qthelp://kallup.net"
	"/doc/index.html"));

    m_pannel->insertWidget(0, contentWidget);
    m_pannel->insertWidget(1, m_phtml      );
    m_pannel->widget(0)->setMaximumWidth(210);
    m_pannel->show();
    
    ui->HelpLayout->addWidget(m_pannel);

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
    if (ui->EditorWidget->document()->isModified()) {
        ui->EditorWidget->document()->setModified(false);
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
    ui->EditorWidget->document()->setModified(false);
    QString fname = ui->EditorWidget->documentTitle();

    fname = QFileDialog::getSaveFileName(
    this,
    tr("Save File As ..."),
    QString(QDir::homePath()),
    "Forms *.frm (*.frm);;Programs *.prg (*.prg);;All Files *.* (*.*)");
    
    qDebug() << "before: " << fname;
    
    if (fname.length() > 1) {
        QFile f(fname); f.open(
        QIODevice::WriteOnly  |
        QIODevice::Truncate   |
        QIODevice::Text);

        if (!f.isOpen())
        throw new dBaseError("file can not be saved");
        
        QString src = ui->EditorWidget->document()->toPlainText();

        QTextStream out(&f);
        out.setCodec("UTF-8");
        out.setGenerateByteOrderMark(false);
        out << src;
        
        ui->EditorWidget->setDocumentTitle(fname);
        
        qDebug() << "after: " << fname;
        qDebug() << "bytes: " << src.size();
        
        f.close();
    }
}


void MainWindow::on_FileOpen(int state)
{
    if (ui->EditorWidget->document()->isModified()) {
        ui->EditorWidget->document()->setModified(false );
        QString fname = ui->EditorWidget->documentTitle();
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
        ui->EditorWidget->document()->toPlainText();

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

    ui->EditorWidget->setDocumentTitle(fname);

    QFile f(fname);
    f.open(QIODevice::ReadOnly | QIODevice::Text); QByteArray barr=
    f.readAll();
    f.close();

    ui->EditorWidget->clear();
    ui->EditorWidget->insertPlainText(barr.data());
    ui->EditorWidget->document()->setModified(false);
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
//    Settings *se = new SettingsDialog(this);
//    se->show();
  

//    delete  se;
}

void MainWindow::on_toolButton_triggered(QAction *arg1)
{
    qDebug() << "Home";
}

void MainWindow::on_toolButton_2_triggered(QAction *arg1)
{
    qDebug() << "Edit";
}

void MainWindow::on_toolButton_3_triggered(QAction *arg1)
{
    qDebug() << "Design";
}

void MainWindow::on_toolButton_5_triggered(QAction *arg1)
{
    qDebug() << "Help";
}

void MainWindow::on_toolButton_6_triggered(QAction *arg1)
{
    qDebug() << "Run";
}

void MainWindow::on_toolButton_4_triggered(QAction *arg1)
{
    qDebug() << "Build";
}

void MainWindow::on_LayoutView_itemActivated(QTreeWidgetItem *item, int column)
{
    qDebug() << item->text(0) << column;
}

void MainWindow::on_LayoutView_itemClicked(QTreeWidgetItem *item, int column)
{
    qDebug() << item->text(0) << column;
}




void MainWindow::ShowContextMenuProject()
{
    QPoint globalPos = mapFromGlobal(QCursor::pos());
    QMenu myPopUp;

    QAction *sess1 = new QAction("Last Session ...");
    QFont font;
    
    font.setBold(true);
    sess1->setFont(font);
    myPopUp.addAction(sess1);
    
    myPopUp.addSeparator();
    myPopUp.addAction("Session 2 - ProjectA");
    myPopUp.addAction("Session 3 - ProjectB");
    myPopUp.addAction("Session 4 - ProjectC");
    myPopUp.setObjectName("myPopUp");
    myPopUp.setStyleSheet(
R"(

#myPopUp {
color: #333;
border: 2px solid #555;
border-radius: 11px;
padding: 5px;
background: qradialgradient(cx: 0.3, cy: -0.4,
fx: 0.3, fy: -0.4,
radius: 1.35, stop: 0 #fff, stop: 1 #888);
min-width: 80px;
}

QMenu:hover:#myPopUp {
background: qradialgradient(cx: 0.3, cy: -0.4,
fx: 0.3, fy: -0.4,
radius: 1.35, stop: 0 #fff, stop: 1 #bbb);
}

QMenu:pressed:#myPopUp {
background: qradialgradient(cx: 0.4, cy: -0.1,
fx: 0.4, fy: -0.1,
radius: 1.35, stop: 0 #fff, stop: 1 #ddd);
}
)");
      
    QAction* selectedItem = myPopUp.exec(globalPos);
    if (selectedItem == nullptr)
    return;    
}

void MainWindow::on_toolButton_Project_pressed()
{
    ShowContextMenuProject();
}

void MainWindow::on_pushButton_7_clicked()
{
    QDesktopServices::openUrl(QUrl(
    "https://www.paypal.com/cgi-bin/webscr/?"
    "hosted_button_id=FBESEMEXP8HSL&"
    "cmd=_s-xclick"
    ));
}

void MainWindow::on_toolButton_8_clicked()
{
    ui->welcomeLabel->setVisible(false);
    ui->introPage->setVisible(false);
    
    ui->EditorTab->setVisible(false);
    ui->DesignerTab->setVisible(false);
    
    ui->DesignerTabWidget->setVisible(true);
    ui->DesignerTabWidget->setMinimumHeight(200);
}

void MainWindow::on_tabWidget_tabBarClicked(int index)
{
    if (index == 0) {
        ui->welcomeLabel->setVisible(true);
        ui->introPage->setVisible(true);
        ui->DesignerTabWidget->setVisible(false);
    }
}

void MainWindow::on_toolButton_Home_clicked()
{
    on_tabWidget_tabBarClicked(0);
}
