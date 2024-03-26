#include "mainwindow.h"
#include <QVBoxLayout>
#include <QTextEdit>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), serverProcess(new QProcess(this)) {
    this->resize(600, 500);
    setupUi();
}

MainWindow::~MainWindow() {
    delete serverProcess;
    delete serverButton;
    delete outputTextEdit;
    delete dataWindowButton;
    delete dataWindow;
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
            QMessageBox::information(this, "Server Process", "The Server has stopped.");
        }
    }

void MainWindow::updateServerButtonText(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::NormalExit) {
        qDebug() << "normal exit";
//        serverButton->setText("Start Server");
    } else {
        // Handle abnormal exit
//        serverButton->setText("Server Crashed");
    }
    serverButton->setText("Start Server");
}

void MainWindow::openDataWindow() {
    if (dataWindow && dataWindow->isVisible()) {
        dataWindow->activateWindow();
        return;
    }
    dataWindow = new DataWindow(this);
    dataWindow->show();
}

void MainWindow::setupUi() {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    serverButton = new QPushButton("Start Server", centralWidget);
    dataWindowButton = new QPushButton("Fetch Data", this);
    outputTextEdit = new QTextEdit(centralWidget);
    outputTextEdit->setReadOnly(true);

    layout->addWidget(serverButton);
    layout->addWidget(dataWindowButton);
    layout->addWidget(outputTextEdit);
//    setLayout(layout);

    connect(serverButton, &QPushButton::clicked, this, &MainWindow::Server);
    connect(dataWindowButton, &QPushButton::clicked, this, &MainWindow::openDataWindow);
    connect(serverProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &MainWindow::updateServerButtonText);

    // Connect the server process's output to the text edit
    connect(serverProcess, &QProcess::readyRead, [this]() {
        outputTextEdit->append(serverProcess->readAllStandardOutput());
    });

    connect(serverProcess, &QProcess::errorOccurred, [this](QProcess::ProcessError error) {
        qDebug() << "Server started successfully" << error ;
//        QMessageBox::critical(this, "Error", "Server Stopped");
    });

    this->setCentralWidget(centralWidget);
}
