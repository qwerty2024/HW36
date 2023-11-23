#include "server.h"
#include <QDataStream>
#include <QSqlRecord>
#include <QMessageBox>

void delete_symbol(QString &str);
void for_send(QString &str);

enum string_code
{
    reg, auth, all_one_msg, all_full, private_one_msg, all_private_msg, online, null
};

string_code command_code(QString const& str)
{
    if (str == "@reg") return reg;
    if (str == "@auth") return auth;
    if (str == "@all_one_msg") return all_one_msg;
    if (str == "@all_full") return all_full;
    if (str == "@private_one_msg") return private_one_msg;
    if (str == "@all_private_msg") return all_private_msg;
    if (str == "@online") return online;

return null;
}

Server::Server()
{
    this->listen(QHostAddress::Any, 55555);
}

void Server::SendToClient(QString str)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_14);
    out << str;
    socket->write(Data);
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &Server::dissconnectUser);
    emit print_log_emit("Client connected " + QString::number(socketDescriptor));
}

void Server::slotReadyRead()
{
    socket = (QTcpSocket*)sender();
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_14);
    if (in.status() == QDataStream::Ok)
    {
        QString str;
        in >> str;
        QString temp = "";
        parse_str(temp, 0, str);

        // распарсить запрос от клиента
        switch(command_code(temp))
        {
        case reg:
            {
                QString tmp_login;
                parse_str(tmp_login, 1, str);
                QString tmp_pass;
                parse_str(tmp_pass, 2, str);

                int error = db.user_exists(tmp_login);

                if (error == -4) // это бан
                {
                    emit print_log_emit("The " + QString::number(socket->socketDescriptor()) + " client tried to register, but he is already in the ban.");
                    SendToClient("@reg -4");
                    break;
                }

                if (error == -1) // уже существует такой юзер
                {
                    emit print_log_emit("The " + QString::number(socket->socketDescriptor()) + " client tried to register, but he is already exist.");
                    SendToClient("@reg -1");
                    break;
                }

                bool test = false;
                QMap<QString, QTcpSocket*>::iterator it;
                for (it = Sockets.begin(); it != Sockets.end(); ++it)
                {
                     if (tmp_login == it.key())
                     {
                         test = true;
                     }
                }

                if (test) // если уже онлайн
                {
                    emit print_log_emit("The " + QString::number(socket->socketDescriptor()) + " client tried to register, but he is already online.");
                    SendToClient("@reg -3");
                    break;
                }

                emit print_log_emit("The " + QString::number(socket->socketDescriptor()) + " client sucessful to register. Login: " + tmp_login);
                SendToClient("@reg 0");
                emit print_log_emit("The server added a user to the database.");
                db.add_user(tmp_login, tmp_pass);
                Sockets[tmp_login] = socket;

                QString to_client = "@online " + QString::number(Sockets.size()) + " ";
                for (it = Sockets.begin(); it != Sockets.end(); ++it)
                {
                    to_client += it.key() + " ";
                }

                // посылаем всем сокетам
                emit print_log_emit("The server has updated users online.");
                for (it = Sockets.begin(); it != Sockets.end(); ++it)
                {
                    if (it.value() != Sockets[tmp_login])
                    {
                        socket = it.value();
                        SendToClient(to_client);
                    }
                }

                break;
            }

        case auth:
            {
                QString tmp_login;
                parse_str(tmp_login, 1, str);
                QString tmp_pass;
                parse_str(tmp_pass, 2, str);

                int error = db.user_go_login(tmp_login, tmp_pass);

                if (error == -4) // это бан
                {
                    emit print_log_emit("The " + QString::number(socket->socketDescriptor()) + " client tried to login, but he is already in the ban.");
                    SendToClient("@auth -4");
                    break;
                }

                if (error == -1)
                {
                    emit print_log_emit("The " + QString::number(socket->socketDescriptor()) + " client tried to login, but passwords don't match.");
                    SendToClient("@auth -1"); // пароли не совпадают
                    break;
                }

                if (error == -2) // нет такого пользователя
                {
                    emit print_log_emit("The " + QString::number(socket->socketDescriptor()) + " client tried to login, but the user does not exist.");
                    SendToClient("@auth -2");
                    break;
                }

                bool test = false;
                QMap<QString, QTcpSocket*>::iterator it;
                for (it = Sockets.begin(); it != Sockets.end(); ++it)
                {
                     if (tmp_login == it.key())
                     {
                         test = true;
                     }
                }

                if (test) // если уже онлайн
                {
                    emit print_log_emit("The " + QString::number(socket->socketDescriptor()) + " client tried to login, but the user is online.");
                    SendToClient("@reg -3");
                    break;
                }


                SendToClient(QString("@auth 0"));
                if (error == 0)
                {
                    emit print_log_emit("The " + QString::number(socket->socketDescriptor()) + " client sucessful to login. Login: " + tmp_login);
                    Sockets[tmp_login] = socket;
                    // новый список онлайн
                    QMap<QString, QTcpSocket*>::iterator it;
                    QString to_client = "@online " + QString::number(Sockets.size()) + " ";
                    for (it = Sockets.begin(); it != Sockets.end(); ++it)
                    {
                        to_client += it.key() + " ";
                    }

                    emit print_log_emit("The server has updated users online.");
                    // посылаем всем сокетам
                    for (it = Sockets.begin(); it != Sockets.end(); ++it)
                    {
                        if (it.value() != Sockets[tmp_login])
                        {
                            socket = it.value();
                            SendToClient(to_client);
                        }
                    }
                }
                break;
            }

        case online:
            {
                QString tmp_login;
                parse_str(tmp_login, 1, str);

                QString to_client = "@online " + QString::number(Sockets.size()) + " ";
                QMap<QString, QTcpSocket*>::iterator it;
                for (it = Sockets.begin(); it != Sockets.end(); ++it)
                {
                    to_client += it.key() + " ";
                }
                emit print_log_emit("The server has updated users online.");
                SendToClient(to_client);
                break;
            }

        case all_one_msg:
            {
                QString tmp_login = "";
                parse_str(tmp_login, 1, str);
                QString msg = "";
                parse_str(msg, 2, str);

                // сделать всем сокетам рассылку, кроме себя
                QMap<QString, QTcpSocket*>::iterator it;
                QString to_client = "@all_one_msg " + tmp_login + " " + msg;

                emit print_log_emit("The server sent out a broadcast message to everyone.");
                // посылаем всем сокетам
                for (it = Sockets.begin(); it != Sockets.end(); ++it)
                {
                    if (it.value() != Sockets[tmp_login])
                    {
                        socket = it.value();
                        SendToClient(to_client);
                    }
                }

                emit print_log_emit("The server added a message to the database.");
                // нужно убрать # и положить в базу данных
                delete_symbol(msg);
                db.add_message(tmp_login, msg);

                break;
            }

        case all_full:
            {
                QString tmp_login = "";
                parse_str(tmp_login, 1, str);

                QString msg = db.give_all_message();
                for_send(msg);

                emit print_log_emit("The server has sent all broadcast messages to the user: " + tmp_login);
                QString to_client = "@all_full " + msg;
                SendToClient(to_client);

                break;
            }

        case private_one_msg:
            {
                QString tmp_login = "";
                parse_str(tmp_login, 1, str);
                QString recver = "";
                parse_str(recver, 2, str);
                QString msg = "";
                parse_str(msg, 3, str);

                // отправить в нужный сокет
                socket = Sockets.value(recver);
                QString to_client = "@private_one_msg " + tmp_login + " " + msg;
                SendToClient(to_client);
                emit print_log_emit("The server sent a private message. " + tmp_login);

                // положить в базу
                delete_symbol(msg);
                db.add_message_private(tmp_login, recver, msg);
                emit print_log_emit("The server added a message to the database.");

                break;
            }

        case all_private_msg:
            {
                QString sender = "";
                parse_str(sender, 2, str);
                QString recver = "";
                parse_str(recver, 1, str);

                QString msg = db.give_all_private_message(sender, recver);
                for_send(msg);

                QString to_client = "@all_private_msg " + sender + " " + msg;
                SendToClient(to_client);
                emit print_log_emit("The server sent all private message. " + sender);
                break;
            }

        case null:
            {
                break;
            }
        }
    }
    else
    {
        qDebug() << "DataStream error";
    }
}

void Server::parse_str(QString &str, int n, QString &mess) // достаем n-ое слово из строки (разделитель пробел)
{
    int count = 0;
    for (int i = 0; i < mess.size(); ++i)
    {
        if (count == n && mess[i] != ' ')
        {
            str += mess[i];
            continue;
        }

        if (mess[i] == ' ')
            count++;

        if (count > n) break;
        if (mess[i] == '\0') break;
    }
}

void Server::dissconnectUser()
{
    // находим логин для удаления из списка онлайн
    QString tmp_login = "";
    QTcpSocket* tmp_socket = (QTcpSocket*)sender();
    QMap<QString, QTcpSocket*>::iterator it;
    for (it = Sockets.begin(); it != Sockets.end(); ++it)
    {
        if(tmp_socket == it.value())
        {
            tmp_login = it.key();
            break;
        }
    }

    // удаляем...
    Sockets.erase(it);
    emit print_log_emit(tmp_login + " - client disconnected.");

    // новый список онлайн
    QString to_client = "@online " + QString::number(Sockets.size()) + " ";
    for (it = Sockets.begin(); it != Sockets.end(); ++it)
    {
        to_client += it.key() + " ";
    }

    // посылаем всем сокетам
    for (it = Sockets.begin(); it != Sockets.end(); ++it)
    {
        socket = it.value();
        SendToClient(to_client);
    }

    tmp_socket->deleteLater();
}

void Server::disconnectFORCE(QString login, QMainWindow *ptr)
{
    bool test = false;
    QTcpSocket* tmp_socket = (QTcpSocket*)sender();

    QMap<QString, QTcpSocket*>::iterator it;
    for (it = Sockets.begin(); it != Sockets.end(); ++it)
    {
        if(login == it.key())
        {
            tmp_socket = it.value();
            test = true;
            break;
        }
    }

    if (!test)
    {
        QMessageBox::critical(ptr, tr("Error"), tr("User is offline!"));
        return;
    }

    socket = tmp_socket;
    QString to_client = "@disconnect ";
    SendToClient(to_client);
    emit print_log_emit("The server forcibly disconnected the user " + login);
}

void Server::banFORCE(QString login, QMainWindow *ptr)
{
    QTcpSocket* tmp_socket = (QTcpSocket*)sender();

    bool test = false;
    QMap<QString, QTcpSocket*>::iterator it;
    for (it = Sockets.begin(); it != Sockets.end(); ++it)
    {
        if(login == it.key())
        {
            test = true;
            tmp_socket = it.value();
            break;
        }
    }

    if (!test)
    {
        QMessageBox::information(ptr, tr("Info"), tr("User is offline!"));
    }

    // запрос в базу, что нужно установить бан
    db.set_ban(login);

    socket = tmp_socket;
    QString to_client = "@ban ";
    SendToClient(to_client);
    emit print_log_emit("The server ban the user " + login);
}

void delete_symbol(QString &str)
{
    str.remove(0, 1);
    for (int i = 0; i < str.size(); ++i)
    {
        if (str[i] == '#') str[i] = ' ';
    }
}

void for_send(QString &str)
{
    for (int i = 0; i < str.size(); ++i)
    {
        if (str[i] == ' ') str[i] = '#';
    }
}
