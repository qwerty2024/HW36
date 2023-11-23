#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTime>
#include <memory>
#include <QListWidgetItem>
#include "startscreen.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    StartScreen *s;
    QVector<QString> online_users;
    bool createClient();

private slots:
    void parse_str(QString &str, int n, QString &mess);
    void slotReadyRead();
    void regQuery(QString str);
    void loginQuery(QString str);

    void on_listW_online_itemDoubleClicked(QListWidgetItem *item);
    void on_tabWidget_tabCloseRequested(int index);
    void on_pushButton_send_clicked();
    void on_lineEdit_returnPressed();

signals:
    void answer_login_emit(QString code);
    void answer_reg_emit(QString code);

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    QByteArray Data;
    void SendToServer(QString str);

    QString login;
    QString password;
};
#endif // MAINWINDOW_H
