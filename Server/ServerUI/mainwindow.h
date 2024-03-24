#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QPushButton>
#include <QTextEdit>
#include <QDebug>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
//    void startServer();
    void Server();
    void updateServerButtonText(int exitCode, QProcess::ExitStatus exitStatus);
//    void stopServer();

private:
    QPushButton *serverButton;
    QProcess *serverProcess;
    QTextEdit *outputTextEdit;
    void setupUi();
};

#endif // MAINWINDOW_H

