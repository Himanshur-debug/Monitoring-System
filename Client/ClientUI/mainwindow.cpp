#include "mainwindow.h"
#include <QVBoxLayout>

#include <iostream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), clientProcess(new QProcess(this)) {
    setupUi();
}

MainWindow::~MainWindow() {
    delete clientProcess;
}

void MainWindow::Client() {
    if (clientProcess->state() == QProcess::NotRunning) {
        clientProcess->setWorkingDirectory("/home/vboxuser/test/MonitoringSys/build/Client");
        clientProcess->start("/home/vboxuser/test/MonitoringSys/build/Client/Client");
        if (clientProcess->state() == QProcess::NotRunning) {
            qDebug() << "Failed to start Client";
        } else {
            qDebug() << "Client started successfully";
        }
        clientButton->setText("Stop Client");
    } else {
        clientProcess->kill();
        clientButton->setText("Start Client");
    }
}
void MainWindow::updateClientButtonText(int exitCode, QProcess::ExitStatus exitStatus) {
//    if (exitStatus == QProcess::NormalExit) {
//        clientButton->setText("Start Client");
//    } else {
//        // Handle abnormal exit
//        clientButton->setText("Stop Client");
//    }
    outputTextEdit->insertPlainText("\n......Client STOPPED");
    clientButton->setText("Start Client");

}


void MainWindow::setupUi() {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    clientButton = new QPushButton("Start Client", centralWidget);
    outputTextEdit = new QTextEdit(centralWidget);

    layout->addWidget(clientButton);
    layout->addWidget(outputTextEdit);

    connect(clientButton, &QPushButton::clicked, this, &MainWindow::Client);
    connect(clientProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &MainWindow::updateClientButtonText);

    connect(clientProcess, &QProcess::readyRead, [this]() {
        outputTextEdit->append(clientProcess->readAllStandardOutput());
    });

    connect(clientProcess, &QProcess::errorOccurred, [this](QProcess::ProcessError error) {
        qDebug() << "Client started successfully" << error ;
    });

    this->setCentralWidget(centralWidget);
}
