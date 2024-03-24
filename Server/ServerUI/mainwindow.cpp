#include "mainwindow.h"
#include <QVBoxLayout>
#include <QTextEdit>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), serverProcess(new QProcess(this)) {
    setupUi();
}

MainWindow::~MainWindow() {
    delete serverProcess;
}

void MainWindow::Server() {
        if (serverProcess->state() == QProcess::NotRunning) {
            serverProcess->setWorkingDirectory("/home/vboxuser/test/MonitoringSys/build/Server");
            serverProcess->start("/home/vboxuser/test/MonitoringSys/build/Server/Server");
            if (serverProcess->state() == QProcess::NotRunning) {
                qDebug() << "Failed to start server";
            } else {
                qDebug() << "Server started successfully";
            }
            serverButton->setText("Stop Server");
        } else {
            serverProcess->kill();
            outputTextEdit->insertPlainText("\n......SERVER STOPPED");
            serverButton->setText("Start Server");
        }
    }

void MainWindow::updateServerButtonText(int exitCode, QProcess::ExitStatus exitStatus) {
//        if (exitStatus == QProcess::NormalExit) {
//            serverButton->setText("Start Server");
//        } else {
//            // Handle abnormal exit
//            serverButton->setText("Server Crashed");
//        }
        serverButton->setText("Start Server");
    }

void MainWindow::setupUi() {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    serverButton = new QPushButton("Start Server", centralWidget);
//    QPushButton *stopButton = new QPushButton("Stop Server", centralWidget);
    outputTextEdit = new QTextEdit(centralWidget);

    layout->addWidget(serverButton);
//    layout->addWidget(stopButton);
    layout->addWidget(outputTextEdit);

    connect(serverButton, &QPushButton::clicked, this, &MainWindow::Server);
    connect(serverProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &MainWindow::updateServerButtonText);

    // Connect the server process's output to the text edit
//    connect(serverProcess, &QProcess::readyRead, [this, outputTextEdit]() {
//        outputTextEdit->append(serverProcess->readAllStandardOutput());
//    });
    connect(serverProcess, &QProcess::readyRead, [this]() {
        outputTextEdit->append(serverProcess->readAllStandardOutput());
    });

    connect(serverProcess, &QProcess::errorOccurred, [this](QProcess::ProcessError error) {
        qDebug() << "Server started successfully" << error ;
    });

    this->setCentralWidget(centralWidget);
}
