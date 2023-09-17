#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFile>
#include <QSqlQuery>
#include <QSqlError>
#include <QJsonArray>
#include <QMainWindow>
#include <QJsonObject>
#include <QFileDialog>
#include <QJsonDocument>
#include <QtSql/QSqlDatabase>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnFind_clicked();

    void on_btnConnect_clicked();

    void on_openDir_clicked();

    void on_btnMigrate_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
