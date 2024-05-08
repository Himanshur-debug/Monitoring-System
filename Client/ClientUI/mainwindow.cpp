#include "mainwindow.h"
#include <QVBoxLayout>
#include <QRegularExpression>
#include <QInputDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), clientProcess(new QProcess(this)) {
    setConfigFilePath();
    this->resize(400, 300);
    setupUi();

}

MainWindow::~MainWindow() {
    delete clientProcess;
}

void MainWindow::setConfigFilePath() {
    bool ok;
    configFilePath = QInputDialog::getText(this, tr("Configuration File"), tr("Input Configuration file path:"), QLineEdit::Normal, "", &ok).trimmed();
    if (ok && !configFilePath.isEmpty()) {
        return;
    }
    else if (ok && configFilePath.isEmpty()) {
        QMessageBox::critical(this, "Error", "empty path!!! Retry...");
        close();
        exit(0);

    }
    else {
        close();
        exit(0);
   }
}

void MainWindow::Client() {
    if (clientProcess->state() == QProcess::NotRunning) {
        clientProcess->start("./Client");
        if (clientProcess->state() == QProcess::NotRunning) {
            qDebug() << "Failed to start Client";
        } else {
            emit sendInput();
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

void MainWindow::sendConfigFilePathToClient() {
    clientProcess->write(configFilePath.toUtf8()+'\n');
    clientProcess->waitForBytesWritten();
}

void MainWindow::setupUi() {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    clientButton = new QPushButton("Start Client", centralWidget);
    outputTextEdit = new QTextEdit(centralWidget);
    outputTextEdit->setReadOnly(true);

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

    // Connect the new slot to get input from the message box and send it to the server
    connect(this, &MainWindow::sendInput, this, &MainWindow::sendConfigFilePathToClient);

    connect(clientProcess, &QProcess::readyReadStandardError, [&]() {
            // Read the standard error output and append it to the QTextEdit
            QByteArray errorOutput = clientProcess->readAllStandardError();
            QMessageBox::critical(this, "Error", QString::fromUtf8(errorOutput));
            setConfigFilePath();
        }
    );

    this->setCentralWidget(centralWidget);
}
