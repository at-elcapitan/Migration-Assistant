#include "mainwindow.h"
#include "ui_mainwindow.h"

bool botReady;
bool connectionReady;
QJsonObject jdata;
QSqlDatabase db;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->status->append("AT Migration Assistant <b>v1.0</b> ready.");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnFind_clicked()
{
    bool error = false;
    QTextBrowser* status = ui->status;

    if (botReady) {
        status->append("<b><span style='color: #FF4444'>ERROR</span></b>: JSON preparation is already completed.");
        return;
    }

    status->insertPlainText("\nPrepearing... ");
    if (ui->botPath->text() == "") {
        status->append("<b><span style='color: #FF4444'>ERROR</span></b>: Please set BOT path.");
        error = true;
    }

    QString lookFor;

    switch (ui->versionSelect->currentIndex()) {
    case 0:
        status->append("<b><span style='color: #FF4444'>ERROR</span></b>: Please select bot version.");
        error = true;
        break;
    case 1:
        lookFor = "files/lists.json";
        break;
    case 2:
        lookFor = "./lists.json";
    }

    if (error) {
        return;
    }
    ui->progressBar->setValue(33);
    status->insertPlainText("OK\n");

    status->insertPlainText("Looking for file... ");
    QString path = ui->botPath->text() + lookFor;
    QFile jfile(path);

    jfile.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = jfile.readAll();
    jfile.close();
    ui->progressBar->setValue(66);

    QJsonParseError errorPtr;
    QJsonDocument doc = QJsonDocument::fromJson(data, &errorPtr);
    if (doc.isNull()) {
        status->insertPlainText("Failed.");
        status->append("<b><span style='color: #FF4444'>ERROR</span></b>: File don't found.");
        return;
    }
    ui->progressBar->setValue(100);
    status->insertPlainText("OK");

    jdata = doc.object();
    botReady = true;
}


void MainWindow::on_btnConnect_clicked()
{
    QTextBrowser* status = ui->status;

    if (connectionReady) {
        status->append("<b><span style='color: #FF4444'>ERROR</span></b>: The connection is already established.");
        return;
    }

    ui->progressBar->setValue(0);
    db = QSqlDatabase::addDatabase("QPSQL");

    if (ui->password->text() == "") {
        status->append("<b><span style='color: #FF4444'>ERROR</span></b>: Please, set DB password.");
        return;
    }

    status->insertPlainText("\nConfiguring connection... ");
    db.setHostName(ui->url->text());
    db.setPort(ui->port->text().toInt());
    db.setDatabaseName("nextmdb");
    db.setUserName(ui->username->text());
    db.setPassword(ui->password->text());
    status->insertPlainText("OK\n");
    ui->progressBar->setValue(50);

    status->insertPlainText("Connecting to PSQL... ");
    if (!db.open()) {
        status->insertPlainText("Failed");
        status->append("<b><span style='color: #FF4444'>ERROR</span></b>: Can't connect to PSQL DB.");
        return;
    }
    status->insertPlainText("OK");
    ui->progressBar->setValue(100);
    connectionReady = true;
}


void MainWindow::on_openDir_clicked()
{
    QString filePath = QFileDialog::getExistingDirectory(this, "Select BOT directory", QDir::homePath());
    if (!filePath.isEmpty()) {
        ui->botPath->setText(filePath + "/");
    }
}


void MainWindow::on_btnMigrate_clicked()
{
    QTextBrowser* status = ui->status;
    ui->progressBar->setValue(0);
    status->insertPlainText("\nChecking DB conn... ");
    if (!connectionReady) {
        status->insertPlainText("Failed");
        status->append("<b><span style='color: #FF4444'>ERROR</span></b>: Please connect to PSQL DB.");
        return;
    }
    status->insertPlainText("OK\n");
    status->insertPlainText("Checking if JSON is loaded... ");
    if (!botReady) {
        status->insertPlainText("Failed");
        status->append("<b><span style='color: #FF4444'>ERROR</span></b>: Please prepear JSON.");
        return;
    }
    status->insertPlainText("OK\n");
    ui->progressBar->setValue(25);
    status->insertPlainText("Calculating dependencies... ");

    QStringList jkeys = jdata.keys();
    double length = 0;
    double progressBarPos = 50;

    for (QString key : jkeys) {
        length += jdata.value(key).toArray().size();
    }

    double cost = 50/length;

    status->insertPlainText("Done!\n");
    status->insertPlainText("Migrating... ");
    ui->progressBar->setValue(progressBarPos);

    for (QString key : jkeys) {
        QJsonArray outerArray = jdata.value(key).toArray();
        for (int x = 0; x != outerArray.size(); x++) {
            QJsonArray innerArray = outerArray[x].toArray();
            QString query = "INSERT INTO music_data (music_name, music_url, provider, user_id) VALUES ('" + innerArray[0].toString() + "', '" + innerArray[1].toString() + "', 'YT_OLD', '" + key + "')";
            progressBarPos += cost;
            ui->progressBar->setValue(progressBarPos);
            db.transaction();
            db.exec(query);
            db.commit();
        }
    }

    status->insertPlainText("Done!");

    db.close();
    connectionReady = false;
    botReady = false;
}
