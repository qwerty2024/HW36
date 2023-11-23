#include "database.h"
#include <QSqlResult>

Database::Database()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./chatdb");
    if (db.open())
    {
        qDebug() << "Open database";
    }
    else
    {
        qDebug() << "NOT open database";
    }

    query = new QSqlQuery(db);
    query->exec("CREATE TABLE Users(login VARCHAR(255) NOT NULL PRIMARY KEY, pass VARCHAR(255) NOT NULL, ban VARCHAR(255) NOT NULL DEFAULT '0');");
    query->exec("CREATE TABLE Messages(id INT AUTO_INCREMENT PRIMARY KEY, Sender VARCHAR(255) NOT NULL, Recver VARCHAR(255) NOT NULL, Message TEXT);");
}

QVector<QString> Database::get_users()
{
    QVector<QString> result;
    query = new QSqlQuery(db);

    if(query->exec("SELECT login FROM Users"))
    {
        while (query->next())
        {
            //Здесь получаете Ваши данные
            result.push_back(query->value(0).toString());
        }
    }

    return result;
}

int Database::user_exists(QString login)
{
    QString result = "";
    QString ban = "";
    query = new QSqlQuery(db);

    if(query->exec("SELECT login,ban FROM Users WHERE login = '" + login + "';"))
    {
        while (query->next())
        {
            result.push_back(query->value(0).toString());
            ban = query->value(1).toString();
        }
    }

    if (ban == "1")
        return -4;

    qDebug() << result;
    if (result == "")
        return 0;
    else
        return -1;
}

int Database::user_go_login(QString login, QString pass)
{
    QString result = "";
    QString ban = "";
    query = new QSqlQuery(db);

    if(query->exec("SELECT pass,ban FROM Users WHERE login = '" + login + "';"))
    {
        while (query->next())
        {
            result.push_back(query->value(0).toString());
            ban = query->value(1).toString();
        }
    }

    if (ban == "1")
    {
        return -4;
    }

    if (pass == result)
    {
        return 0;
    }

    if (result == "")
    {
        return -2;
    }

    if (pass != result)
    {
        return -1;
    }

    return 666;
}

void Database::add_user(QString login, QString pass)
{
    query = new QSqlQuery(db);
    query->exec("INSERT INTO Users(login,pass) VALUES('" + login + "','" + pass + "');");
}

void Database::add_message(QString login, QString msg)
{
    query = new QSqlQuery(db);
    query->exec("INSERT INTO Messages(Sender,Recver,Message) VALUES('" + login + "','ALL','" + msg + "')");
}

void Database::add_message_private(QString sender, QString recver, QString msg)
{
    query = new QSqlQuery(db);
    query->exec("INSERT INTO Messages(Sender,Recver,Message) VALUES('" + sender + "','" + recver + "','" + msg + "')");
}

QString Database::give_all_private_message(QString sender, QString recver)
{
    QString result = "";
    query = new QSqlQuery(db);

    if(query->exec("SELECT Sender,Message FROM Messages WHERE (Recver = '" + recver + "' AND Sender = '" + sender + "') OR (Recver = '" + sender + "' AND Sender = '" + recver + "');"))
    {
        while (query->next())
        {
            result += "$" + query->value(0).toString() + " " + query->value(1).toString();
        }
    }
    return result;
}

void Database::set_ban(QString login)
{
    QString result = "";
    query = new QSqlQuery(db);
    query->exec("UPDATE Users SET ban = '1' WHERE login = '" + login + "';");
}

QString Database::give_all_message()
{
    QString result = "";
    query = new QSqlQuery(db);

    if(query->exec("SELECT Sender,Message FROM Messages WHERE Recver = 'ALL';"))
    {
        while (query->next())
        {
            result += "$" + query->value(0).toString() + " " + query->value(1).toString();
        }
    }
    return result;
}
