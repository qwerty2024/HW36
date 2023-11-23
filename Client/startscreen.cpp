#include "startscreen.h"
#include "ui_startscreen.h"
#include <QDebug>

StartScreen::StartScreen(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartScreen)
{
    ui->setupUi(this);
    this->setWindowTitle("Authentification");

    connect(ui->page_login, &Auth_form::registrationRequested, this, &StartScreen::setRegForm);
    connect(ui->page_reg, &Reg_form::loginRequested, this, &StartScreen::setAuthForm);

    connect(ui->page_login, &Auth_form::rejected, this, &StartScreen::onRejectRequested);
    connect(ui->page_reg, &Reg_form::rejected, this, &StartScreen::onRejectRequested);

    // reg query
    connect(ui->page_reg, &Reg_form::reg_query, this, &StartScreen::regQuery);
    connect(this, &StartScreen::answer_reg_emit, ui->page_reg, &Reg_form::answer_reg);
    connect(ui->page_reg, &Reg_form::accepted, this, &StartScreen::onLoggedIn);

    // login query
    connect(ui->page_login, &Auth_form::login_query, this, &StartScreen::loginQuery);
    connect(this, &StartScreen::answer_login_emit, ui->page_login, &Auth_form::answer_login);
    connect(ui->page_login, &Auth_form::accepted, this, &StartScreen::onLoggedIn);
}

StartScreen::~StartScreen()
{
    delete ui;
}

void StartScreen::setAuthForm()
{
    ui->stackedWidget->setCurrentIndex(0);
    this->setWindowTitle("Authentification");
}

void StartScreen::setRegForm()
{
    ui->stackedWidget->setCurrentIndex(1);
    this->setWindowTitle("Registration");
}

void StartScreen::onRejectRequested()
{
    reject();
}

void StartScreen::regQuery(QString str)
{
    emit reg_query(str);
}

void StartScreen::loginQuery(QString str)
{
    emit login_query(str);
}

void StartScreen::answer_reg(QString code)
{
    emit answer_reg_emit(code);
}

void StartScreen::answer_login(QString code)
{
    emit answer_login_emit(code);
}

void StartScreen::onLoggedIn(QString login, QString pass)
{
    m_login = login;
    m_pass = pass;
    accept();
}
