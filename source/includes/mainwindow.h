#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "source/includes/pch.h"
#include "source/includes/scene.h"
#include "source/includes/designergraphicsview.h"
#include "source/includes/designeritem.h"
#include "source/includes/helpbrowser.h"
#include "source/includes/components_draglist.h"
#include "source/includes/events_and_methods.h"
#include "source/includes/helpbrowser.h"
#include "source/includes/editorgutter.h"
#include "source/includes/helpdocker.h"
#include "source/includes/click.h"
#include "source/includes/keypress.h"
#include "source/includes/settings_dialog.h"

#include ".uic/ui_mainwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    bool eventFilter(QObject *target, QEvent *event);

    void resizeEvent(QResizeEvent * event);
    void closeEvent(QCloseEvent   * event);

    void on_FileOpen(int state = 0);
    void on_SaveFile();
    void on_AboutQt();

    void on_actionEinstellungen();
public:
    //void keyPressEvent(QKeyEvent  * event);

    void addEditPanel();
    void addHelpPanel();

    void delHelpPanel();
    void delEditPanel();

    class MyEditor *editor;

    class components_draglist *components_drag;
    class events_and_methods  *eventsAmethods;

    class update_dbase *update;

    Ui::MainWindow *ui;

public slots:
    void on_actionBeenden_triggered();
    void on_dockHelpOpen();
    void on_dockEditOpen();

private slots:
    void on_MainWindow_clicked();

private:
    QSplitter  * hsplit;

public:
    QHelpEngine* helpEngine;
    QWidget    * helpWindow;

    int helpCtx;
};

extern class MainWindow *w;
#endif
