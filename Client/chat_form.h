#ifndef CHAT_FORM_H
#define CHAT_FORM_H

#include <QWidget>

namespace Ui {
class Chat_form;
}

class Chat_form : public QWidget
{
    Q_OBJECT

public:
    explicit Chat_form(QWidget *parent = nullptr);
    ~Chat_form();

private:
    Ui::Chat_form *ui;
};

#endif // CHAT_FORM_H
