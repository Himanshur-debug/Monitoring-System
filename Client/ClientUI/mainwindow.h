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
    void Client();
    void updateClientButtonText(int exitCode, QProcess::ExitStatus exitStatus);
//    void sendInputToClient();

signals:
    void sendInput();

private:
    QString configFilePath;
    QPushButton *clientButton;
    QProcess *clientProcess;
    QTextEdit *outputTextEdit;
    void setupUi();
    void setConfigFilePath();
    void sendConfigFilePathToClient();
};

#endif // MAINWINDOW_H

