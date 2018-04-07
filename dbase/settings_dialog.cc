#include "source/includes/settings_dialog.h"
#include <QAbstractButton>

SettingsWidget::SettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);

    connect(ui->listWidget,
            &QAbstractItemView::clicked,
            this,
            &SettingsWidget::on_listWidget_clicked
            );
    connect(ui->pushButton_3,
            &QAbstractButton::toggled,
            this,
            &SettingsWidget::on_pushButton_3_pressed
            );
}

void SettingsWidget::on_pushButton_3_pressed()
{
    close();
}

void SettingsWidget::on_pushButton_2_pressed()
{
    close();
}

void SettingsWidget::on_listWidget_clicked(const QModelIndex &index)
{
    if (ui->listWidget->item(index.row())->text() == "Update server") {
        ui->label->setText("Update Server");
    }
}

void SettingsWidget::on_pushButton_toggled(bool checked)
{

}
