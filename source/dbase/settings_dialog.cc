#include "source/includes/settings_dialog.h"
#include <QAbstractButton>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    connect(ui->listWidget,
            &QAbstractItemView::clicked,
            this,
            &SettingsDialog::on_listWidget_clicked
            );
    connect(ui->pushButton_3,
            &QAbstractButton::toggled,
            this,
            &SettingsDialog::on_pushButton_3_pressed
            );
}

void SettingsDialog::on_pushButton_3_pressed()
{
    close();
}

void SettingsDialog::on_pushButton_2_pressed()
{
    close();
}

void SettingsDialog::on_listWidget_clicked(const QModelIndex &index)
{
    if (ui->listWidget->item(index.row())->text() == "Update server") {
        ui->label->setText("Update Server");
    }
}

void SettingsDialog::on_pushButton_toggled(bool checked)
{

}
