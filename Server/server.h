#pragma once
#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QTime>
#include <QSqlTableModel>
#include "database.h"
#include <QMessageBox>
#include <QMainWindow>

class Server : public QTcpServer
{
    Q_OBJECT

public:
    Server();
    QTcpSocket *socket;
    Database db;
    QSqlTableModel *model_users;
    QSqlTableModel *model_messages;
    QMap<QString, QTcpSocket*> Sockets;

signals:
    void print_log_emit(const QString &str);

private:
    QByteArray Data;
    void SendToClient(QString str);

public slots:
    void incomingConnection(qintptr socketDescriptor) override;
    void slotReadyRead();
    void parse_str(QString &str, int n, QString &mess);
    void dissconnectUser();
    void disconnectFORCE(QString login, QMainWindow *ptr);
    void banFORCE(QString login, QMainWindow *ptr);
};

#endif // SERVER_H
