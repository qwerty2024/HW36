#ifndef AUTH_FORM_H
#define AUTH_FORM_H

#include <QWidget>

namespace Ui {
class Auth_form;
}

class Auth_form : public QWidget
{
    Q_OBJECT

public:
    explicit Auth_form(QWidget *parent = nullptr);
    ~Auth_form();

public slots:
    void answer_login(QString code);

private slots:
    void on_registrationButton_clicked();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

signals:
    void registrationRequested();
    void rejected();
    void login_query(QString qu);
    void accepted(QString login, QString pass);

private:
    Ui::Auth_form *ui;
};

#endif // AUTH_FORM_H
