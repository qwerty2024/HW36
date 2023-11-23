#ifndef REG_FORM_H
#define REG_FORM_H

#include <QWidget>

namespace Ui {
class Reg_form;
}

class Reg_form : public QWidget
{
    Q_OBJECT

public:
    explicit Reg_form(QWidget *parent = nullptr);
    ~Reg_form();

public slots:
    void answer_reg(QString code);
    //void error_reg();
    //void success_reg();

private slots:
    void on_loginButton_clicked();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();


signals:
    void loginRequested();
    void rejected();
    void reg_query(QString qu);
    void accepted(QString login, QString pass);

private:
    Ui::Reg_form *ui;
};

#endif // REG_FORM_H
