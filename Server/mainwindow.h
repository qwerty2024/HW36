#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include "server.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    Server s;

public slots:
    void print_log(const QString &str);

signals:
    void disconnect_emit(QString login, QMainWindow *ptr);
    void ban_emit(QString login, QMainWindow *ptr);

private slots:
    void on_pushButton_updateTabs_clicked();
    void on_pushButton_BAN_clicked();
    void on_pushButton_disconnect_clicked();


private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
