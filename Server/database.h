#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>

class Database
{
public:
    Database();

    QSqlDatabase& get_db() { return db; }
    QVector<QString> get_users();
    int user_exists(QString login);
    int user_go_login(QString login, QString pass);
    void add_user(QString login, QString pass);
    void add_message(QString login, QString msg);
    QString give_all_message();
    void add_message_private(QString sender, QString recver, QString msg);
    QString give_all_private_message(QString sender, QString recver);
    void set_ban(QString login);

private:
    QSqlDatabase db;
    QSqlQuery *query;
};

#endif // DATABASE_H
