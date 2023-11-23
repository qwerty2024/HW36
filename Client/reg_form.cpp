#include "reg_form.h"
#include "ui_reg_form.h"
#include <QMessageBox>
#include <QDebug>
#include "sha1.h"

Reg_form::Reg_form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Reg_form)
{
    ui->setupUi(this);
}

Reg_form::~Reg_form()
{
    delete ui;
}

void Reg_form::answer_reg(QString code)
{
    if (code == "0")
    {
        emit accepted(ui->lineEdit_login->text(), ui->lineEdit_password->text());
    }

    if (code == "-1") // уже существует
    {
        QMessageBox::critical(this, tr("Error"), tr("This user exist!"));
        ui->lineEdit_login->clear();
        ui->lineEdit_password->clear();
        ui->lineEdit_confirmation->clear();
    }

    if (code == "-3") // уже залогинен
    {
        QMessageBox::critical(this, tr("Error"), tr("This user alredy logined!"));
        ui->lineEdit_login->clear();
        ui->lineEdit_password->clear();
        ui->lineEdit_confirmation->clear();
    }

    if (code == "-4") // забанен
    {
        QMessageBox::critical(this, tr("Error"), tr("User BAN!"));
        ui->lineEdit_login->clear();
        ui->lineEdit_password->clear();
        ui->lineEdit_confirmation->clear();
    }
}

void Reg_form::on_loginButton_clicked()
{
    emit loginRequested();
}

void Reg_form::on_buttonBox_accepted()
{
    QString login = ui->lineEdit_login->text();
    QString pass = ui->lineEdit_password->text();
    QString conf = ui->lineEdit_confirmation->text();

    if (pass != conf)
    {
        QMessageBox::critical(this, tr("Error"), tr("Passwords don't match!"));
        ui->lineEdit_login->clear();
        ui->lineEdit_password->clear();
        ui->lineEdit_confirmation->clear();
        return;
    }

    if (pass == "" || login == "" || conf == "")
    {
        QMessageBox::critical(this, tr("Error"), tr("All fields must be filled in!"));
        ui->lineEdit_login->clear();
        ui->lineEdit_password->clear();
        ui->lineEdit_confirmation->clear();
        return;
    }

    Hash passHash = sha1(pass.toStdString());
    pass = passHash.toQString();
    // отправить данные на сервер, и получить ответ...
    QString data_to_server = "@reg " + login + " " + pass;
    emit reg_query(data_to_server);
}

void Reg_form::on_buttonBox_rejected()
{
    emit rejected();
}

