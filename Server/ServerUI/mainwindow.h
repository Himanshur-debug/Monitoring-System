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
    void Server();
    void updateServerButtonText(int exitCode, QProcess::ExitStatus exitStatus);
    void openDataWindow();

private:
    QPushButton *serverButton;
    QProcess *serverProcess;
    QTextEdit *outputTextEdit;
    QPushButton *dataWindowButton;
    DataWindow *dataWindow;
    void setupUi();
};

#endif // MAINWINDOW_H

