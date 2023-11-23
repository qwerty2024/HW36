#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTabWidget>
#include <QTableWidget>
#include <QPlainTextEdit>
#include "chat_form.h"
#include <sha1.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Client");

    socket = new QTcpSocket(this);
    socket->connectToHost("127.0.0.1", 55555);
    s = new StartScreen();

    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);

    // reg query
    connect(s, &StartScreen::reg_query, this, &MainWindow::regQuery);
    connect(this, &MainWindow::answer_reg_emit, s, &StartScreen::answer_reg);

    // login query
    connect(s, &StartScreen::login_query, this, &MainWindow::loginQuery);
    connect(this, &MainWindow::answer_login_emit, s, &StartScreen::answer_login);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void delete_symbol(QString &str);
void for_send(QString &str);
QVector<QString> parse_msg_all(QString message);

enum string_code
{
    null, reg, auth, online, all_one_msg, all_full, private_one_msg, all_private_msg, disconnect_a, ban
};

string_code command_code(QString const& str)
{
    if (str == "@reg") return reg;
    if (str == "@auth") return auth;
    if (str == "@online") return online;
    if (str == "@all_one_msg") return all_one_msg;
    if (str == "@all_full") return all_full;
    if (str == "@private_one_msg") return private_one_msg;
    if (str == "@all_private_msg") return all_private_msg;
    if (str == "@disconnect") return disconnect_a;
    if (str == "@ban") return ban;

return null;
}

bool MainWindow::createClient()
{
    auto result = s->exec();

    login = s->m_login;
    password = s->m_pass;

    this->setWindowTitle("Client - " + login);

    if (result != QDialog::Rejected)
    {
        // запросить сообщения всем
        SendToServer("@all_full " + login);
        socket->waitForReadyRead(30);
        // запросить пользователей онлайн
        SendToServer("@online " + login);

        return true;
    }
    else
    {
        return false;
    }
}

void MainWindow::slotReadyRead()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_14);
    if (in.status() == QDataStream::Ok)
    {
        QString str = "";
        in >> str;
        QString temp = "";
        parse_str(temp, 0, str);
        switch(command_code(temp))
        {

            case reg:
            {
                //emit error_reg_emit();
                QString code = "";
                parse_str(code, 1, str);

                emit answer_reg_emit(code);

                break;
            }

            case auth:
            {
                QString code = "";
                parse_str(code, 1, str);

                emit answer_login_emit(code);
                break;
            }

            case online:
            {
                QString number_users = "";
                parse_str(number_users, 1, str);
                online_users.clear();
                ui->listW_online->clear();
                for (int i = 0; i < number_users.toInt(); ++i)
                {
                    QString tmp = "";
                    parse_str(tmp, i + 2, str);
                    online_users.push_back(tmp);
                    ui->listW_online->addItem(tmp);
                }
                break;
            }

            case all_one_msg:
            {
                QString tmp_sender = "";
                parse_str(tmp_sender, 1, str);
                QString msg = "";
                parse_str(msg, 2, str);
                delete_symbol(msg);
                msg = tmp_sender + ": " + msg;

                ui->tabWidget->widget(0)->findChild<QTextEdit *>("textBrowser")->append(msg);
                break;
            }

            case all_full:
            {
                QString msg = "";
                parse_str(msg, 1, str);

                QVector<QString> items = parse_msg_all(msg);

                for (int i = 0; i < items.size(); ++i)
                    ui->tabWidget->widget(0)->findChild<QTextEdit *>("textBrowser")->append(items[i]);

                break;
            }

            case private_one_msg:
            {
                QString tmp_sender = "";
                parse_str(tmp_sender, 1, str);
                QString msg = "";
                parse_str(msg, 2, str);

                delete_symbol(msg);

                bool exist = false;
                int index = 0;
                for (int i = 0; i < ui->tabWidget->count(); i++)
                {
                    if (ui->tabWidget->tabText(i) == tmp_sender)
                    {
                        exist = true;
                        index = i;
                    }
                }

                if (!exist)
                {
                    index = ui->tabWidget->count();
                    ui->tabWidget->addTab(new Chat_form, tmp_sender);
                    ui->tabWidget->setCurrentIndex(index);
                    SendToServer("@all_private_msg " + login + " " + tmp_sender);
                }
                else
                {
                    ui->tabWidget->setCurrentIndex(index);
                }

                ui->tabWidget->widget(index)->findChild<QTextEdit *>("textBrowser")->append(tmp_sender + ": " + msg);

                break;
            }

            case all_private_msg:
            {
                QString recver = "";
                parse_str(recver, 1, str);
                QString msg = "";
                parse_str(msg, 2, str);

                // распарсить и запихать в текст браузер
                QVector<QString> items = parse_msg_all(msg);

                bool exist = false;
                int index = 0;
                for (int i = 0; i < ui->tabWidget->count(); i++)
                {
                    if (ui->tabWidget->tabText(i) == recver)
                    {
                        exist = true;
                        index = i;
                    }
                }

                if (!exist)
                {
                    index = ui->tabWidget->count();
                    ui->tabWidget->addTab(new Chat_form, recver);
                    ui->tabWidget->setCurrentIndex(index);

                }
                else
                {
                    ui->tabWidget->setCurrentIndex(index);
                }

                ui->tabWidget->widget(index)->findChild<QTextEdit *>("textBrowser")->clear();
                for (int i = 0; i < items.size(); ++i)
                    ui->tabWidget->widget(index)->findChild<QTextEdit *>("textBrowser")->append(items[i]);

                break;
            }

            case disconnect_a:
            {
                // тут нужно уронить окно
                this->close();
                break;
            }

            case ban:
            {
                this->close();
                break;
            }

            case null:
            {
                break;
            }

        }
    }
}

void MainWindow::SendToServer(QString str)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_14);
    out << str;
    socket->write(Data);
}

void MainWindow::regQuery(QString str)
{
    SendToServer(str);
}

void MainWindow::loginQuery(QString str)
{
    SendToServer(str);
}

void MainWindow::parse_str(QString &str, int n, QString &mess) // достаем n-ое слово из строки (разделитель пробел)
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

void MainWindow::on_listW_online_itemDoubleClicked(QListWidgetItem *item)
{
    // проверить, есть ли такая страница
    // если есть, перейти в нее
    bool exist = false;
    int index = 0;
    for (int i = 0; i < ui->tabWidget->count(); i++)
    {
        if (ui->tabWidget->tabText(i) == item->text())
        {
            exist = true;
            index = i;
        }
    }

    if (!exist)
    {
        index = ui->tabWidget->count();
        ui->tabWidget->addTab(new Chat_form, item->text());
        ui->tabWidget->setCurrentIndex(index);
    }
    else
    {
        ui->tabWidget->setCurrentIndex(index);
    }

    // запрос всех сообщений от этого пользователя и этому пользователю
    SendToServer("@all_private_msg " + login + " " + item->text());
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    if (index != 0)
    {
        ui->tabWidget->removeTab(index);
    }
}

void MainWindow::on_pushButton_send_clicked()
{
    int index = ui->tabWidget->currentIndex();

    if (index == 0) // send all
    {
        QString str = ui->lineEdit->text();
        if (str == "") return;
        ui->tabWidget->widget(index)->findChild<QTextEdit *>("textBrowser")->append(login + ": " + str);
        for_send(str);
        SendToServer("@all_one_msg " + login + " #" + str);

        ui->lineEdit->clear();
    }
    else // send private
    {
        QString str = ui->lineEdit->text();
        if (str == "") return;
        for_send(str);
        QString recver = ui->tabWidget->tabText(index);
        SendToServer("@private_one_msg " + login + " " + recver + " #" + str);
        ui->lineEdit->clear();
    }
}

void MainWindow::on_lineEdit_returnPressed()
{
    int index = ui->tabWidget->currentIndex();

    if (index == 0) // send all
    {
        QString str = ui->lineEdit->text();
        if (str == "") return;
        ui->tabWidget->widget(index)->findChild<QTextEdit *>("textBrowser")->append(login + ": " + str);
        for_send(str);
        SendToServer("@all_one_msg " + login + " #" + str);

        ui->lineEdit->clear();
    }
    else // send private
    {
        QString str = ui->lineEdit->text();
        if (str == "") return;
        for_send(str);
        QString recver = ui->tabWidget->tabText(index);
        SendToServer("@private_one_msg " + login + " " + recver + " #" + str);
        ui->lineEdit->clear();
    }
}

void for_send(QString &str)
{
    for (int i = 0; i < str.size(); ++i)
    {
        if (str[i] == ' ') str[i] = '#';
    }
}

void delete_symbol(QString &str)
{
    str.remove(0, 1);
    for (int i = 0; i < str.size(); ++i)
    {
        if (str[i] == '#') str[i] = ' ';
    }
}

QVector<QString> parse_msg_all(QString message)
{
    int i = 0;
    QVector<QString> result;

    while(message[i] != '\0')
    {
        QString tmp = "";
        if (message[i] != '$')
        {
            while(message[i] != '#')
                tmp += message[i++];
            tmp += ": ";
            i++;
            while(message[i] != '$' && message[i] != '\0')
            {
                if (message[i] == '#')
                {
                    tmp += " ";
                    i++;
                }
                else
                    tmp += message[i++];
            }

        }else
        {
            i++;
            continue;
        }
    result.push_back(tmp);
    }

    return result;
}
