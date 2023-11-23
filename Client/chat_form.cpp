#include "chat_form.h"
#include "ui_chat_form.h"

Chat_form::Chat_form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Chat_form)
{
    ui->setupUi(this);
}

Chat_form::~Chat_form()
{
    delete ui;
}
