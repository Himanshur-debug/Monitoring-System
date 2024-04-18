#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QPushButton>
#include <QTextEdit>
#include <QDebug>
#include "datawindow.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    bool dbConnect();
    void Server();
    void updateServerButtonText(int exitCode, QProcess::ExitStatus exitStatus);
    void openDataWindow();

    void sendConfigFilePathToServer();

signals:
    void sendInput();

private:
    QString configFilePath;
    QString dbAddress;
    QString dbUser;
    QString dbName;
    QString dbPassword;
    QSqlDatabase *db;

    QPushButton *serverButton;
    QProcess *serverProcess;
    QTextEdit *outputTextEdit;
    QPushButton *dataWindowButton;
    DataWindow *dataWindow;
    void setupUi();
    void setConfigFilePath();
};

#endif // MAINWINDOW_H

