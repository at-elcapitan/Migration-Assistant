#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->status->append("Migration Assistant <b>AT_MA-16092023xxxx</b> ready.");
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btnFind_clicked()
{
    QString lookFor;
    bool newFormat = false;
    QTextBrowser* status = ui->status;

    switch (ui->versionSelect->currentIndex()) {
    case 0:
        status->append("<b><span style='color: #FF4444'>ERROR</span></b>: Please select bot version.");
        return;
    case 1:
        newFormat = true;
        lookFor = "files/lists.json";
        break;
    case 2:
        lookFor = "files/lists.json";
        break;
    case 3:
        lookFor = "./lists.json";
    }
}


void MainWindow::on_btnConnect_clicked()
{
    QSqlDatabase database = QSqlDatabase::addDatabase("QPOSTGRESQL");
}

