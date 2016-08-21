#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include "source/includes/mainwindow.h"
#include "source/includes/settings_dialog.h"

#include ".uic/ui_settings_dialog.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog: public QDialog
{
public:
    explicit SettingsDialog(QWidget *parent = 0);
protected:

private slots:

    void on_pushButton_3_pressed();
    void on_pushButton_2_pressed();
    void on_listWidget_clicked(const QModelIndex &index);

    void on_pushButton_toggled(bool checked);

private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGS_DIALOG_H
