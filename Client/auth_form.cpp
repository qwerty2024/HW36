#include "auth_form.h"
#include "ui_auth_form.h"
#include <QMessageBox>
#include <sha1.h>

Auth_form::Auth_form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Auth_form)
{
    ui->setupUi(this);
}

Auth_form::~Auth_form()
{
    delete ui;
}

void Auth_form::answer_login(QString code)
{
    if (code == "0")
    {
        emit accepted(ui->lineEdit_login->text(), ui->lineEdit_password->text());
    }

    if (code == "-1")
    {
        QMessageBox::critical(this, tr("Error"), tr("Incorrect password!"));
        ui->lineEdit_password->clear();
    }

    if (code == "-2")
    {
        QMessageBox::critical(this, tr("Error"), tr("There is no such user!"));
        ui->lineEdit_login->clear();
        ui->lineEdit_password->clear();
    }

    if (code == "-3") // уже залогинен
    {
        QMessageBox::critical(this, tr("Error"), tr("This user alredy logined!"));
        ui->lineEdit_login->clear();
        ui->lineEdit_password->clear();
    }

    if (code == "-4") // забанен
    {
        QMessageBox::critical(this, tr("Error"), tr("User BAN!"));
        ui->lineEdit_login->clear();
        ui->lineEdit_password->clear();
    }
}

void Auth_form::on_registrationButton_clicked()
{
    emit registrationRequested();
}

void Auth_form::on_buttonBox_accepted()
{
    QString login = ui->lineEdit_login->text();
    QString pass = ui->lineEdit_password->text();

    if (pass == "" || login == "")
    {
        QMessageBox::critical(this, tr("Error"), tr("All fields must be filled in!"));
        ui->lineEdit_login->clear();
        ui->lineEdit_password->clear();
        return;
    }

    Hash passHash = sha1(pass.toStdString());
    pass = passHash.toQString();

    // отправить данные на сервер, и получить ответ...
    QString data_to_server = "@auth " + login + " " + pass;
    emit login_query(data_to_server);
}

void Auth_form::on_buttonBox_rejected()
{
    emit rejected();
}
