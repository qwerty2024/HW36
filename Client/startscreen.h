#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include <QDialog>

namespace Ui {
class StartScreen;
}

class StartScreen : public QDialog
{
    Q_OBJECT

public:
    explicit StartScreen(QWidget *parent = nullptr);
    ~StartScreen();
    void setAuthForm();
    void setRegForm();
    QString m_login;
    QString m_pass;

public slots:
    void onRejectRequested();
    void regQuery(QString);
    void loginQuery(QString);
    void answer_reg(QString code);
    void answer_login(QString code);
    void onLoggedIn(QString login, QString pass);

signals:
    void reg_query(QString qu);
    void login_query(QString qu);
    void answer_reg_emit(QString code);
    void answer_login_emit(QString code);

private:
    Ui::StartScreen *ui;
};

#endif // STARTSCREEN_H
