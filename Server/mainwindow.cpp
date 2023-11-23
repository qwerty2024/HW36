#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDataStream>
#include <QTimer>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Server");
    print_log("Server start!");

    s.model_users = new QSqlTableModel(this, s.db.get_db());
    s.model_messages = new QSqlTableModel(this, s.db.get_db());

    s.model_users->setTable("Users");
    s.model_messages->setTable("Messages");

    s.model_users->select();
    s.model_messages->select();

    ui->tableView->setModel(s.model_users);
    ui->tableView_2->setModel(s.model_messages);
    ui->tableView_2->hideColumn(0);

    connect(&s, &Server::print_log_emit, this, &MainWindow::print_log);

    connect(this, &MainWindow::disconnect_emit, &s, &Server::disconnectFORCE);
    connect(this, &MainWindow::ban_emit, &s, &Server::banFORCE);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_updateTabs_clicked() // немного костыль, но пока так
{
    s.model_users = new QSqlTableModel(this, s.db.get_db());
    s.model_messages = new QSqlTableModel(this, s.db.get_db());

    s.model_users->setTable("Users");
    s.model_messages->setTable("Messages");

    s.model_users->select();
    s.model_messages->select();

    ui->tableView->setModel(s.model_users);
    ui->tableView_2->setModel(s.model_messages);
    ui->tableView_2->hideColumn(0);
}

void MainWindow::on_pushButton_BAN_clicked()
{
    QString *login = nullptr;
    QModelIndex test;

    // это дичь!
    QModelIndex index = ui->tableView->currentIndex();
    if (test == index)
    {
        QMessageBox::critical(this, tr("Error"), tr("Set target in Users table!"));
        return;
    }
    QVariant login1 = index.data();
    void * data = login1.data();
    login = static_cast<QString *> (data);

    emit ban_emit(*login, this);
}

void MainWindow::on_pushButton_disconnect_clicked()
{
    QString *login = nullptr;
    QModelIndex test;

    // это дичь!
    QModelIndex index = ui->tableView->currentIndex();
    if (test == index)
    {
        QMessageBox::critical(this, tr("Error"), tr("Set target in Users table!"));
        return;
    }
    QVariant login1 = index.data();
    void * data = login1.data();
    login = static_cast<QString *> (data);

    emit disconnect_emit(*login, this);
}

void MainWindow::print_log(const QString &str)
{
    QTime t = QTime::currentTime();
    ui->textBrowser_logs->append(t.toString());
    ui->textBrowser_logs->append(str);
    ui->textBrowser_logs->append("=========================");
    ui->textBrowser_logs->update();
}
