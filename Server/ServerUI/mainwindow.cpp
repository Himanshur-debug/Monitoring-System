#include <QApplication>
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QTextEdit>
#include <QMessageBox>

#include <QInputDialog>

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), serverProcess(new QProcess(this)) {

    this->resize(600, 500);
    setupUi();
    setConfigFilePath();
}

MainWindow::~MainWindow() {
    delete db;
    delete serverProcess;
    delete serverButton;
    delete outputTextEdit;
    delete dataWindowButton;
    delete dataWindow;
}

void MainWindow::setConfigFilePath() {
    bool ok;
    configFilePath = QInputDialog::getText(this, tr("Configuration File"), tr("Input Configuration file path:"), QLineEdit::Normal, "", &ok).trimmed();
    if (ok && configFilePath.isEmpty()) {
        QMessageBox::critical(this, "Error", "empty path!!! Retry...");
        close();
        exit(0);

    } else if (ok && !configFilePath.isEmpty()) {
        if(!dbConnect()) {
            setConfigFilePath();
        }
    }
    else {
        close();
        exit(0);
   }
}

bool MainWindow::dbConnect() {
    QFile configFile(configFilePath);
    if (!configFile.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "Failed to open " + configFilePath);
        return false;
    }

    QJsonParseError parseError;
    QJsonDocument configDoc = QJsonDocument::fromJson(configFile.readAll(), &parseError);
    configFile.close();

    if (parseError.error != QJsonParseError::NoError) {
        QMessageBox::critical(this, "Error", "Error parsing config.json: " + parseError.errorString());
        return false;
    }

    QJsonArray configArray = configDoc.array();
    if (configArray.size() < 2) {
        QMessageBox::critical(this, "Error", "Invalid config.json: Array size less than expected");
        return false;
    }

    QJsonObject dbConfigObj = configArray[0].toObject();

    dbAddress = dbConfigObj["dbAddress"].toString();
    dbUser = dbConfigObj["dbUser"].toString();
    dbName = dbConfigObj["dbName"].toString();
    dbPassword = dbConfigObj["dbPassword"].toString();

    // Set up QSqlDatabase using the configuration values
    db = new QSqlDatabase(QSqlDatabase::addDatabase("QMYSQL"));
    db->setHostName(dbAddress.split(":").first());
    db->setPort(dbAddress.split(":").last().toInt());
    db->setDatabaseName(dbName);
    db->setUserName(dbUser);
    db->setPassword(dbPassword);

    if (!db->open()) {
        QMessageBox::critical(this, "Error", "Could not open database");

    }
    return true;
}

void MainWindow::Server() {
        if (serverProcess->state() == QProcess::NotRunning) {
            serverProcess->start("./Server");

            if (serverProcess->state() == QProcess::NotRunning) {
                qDebug() << "Failed to start server";
            } else {
                qDebug() << "Server started successfully";
                serverButton->setText("Stop Server");
                emit sendInput();
            }

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
    dataWindow = new DataWindow(this, db);
    dataWindow->show();
}

void MainWindow::sendConfigFilePathToServer() {
    serverProcess->write(configFilePath.toUtf8()+'\n');
    serverProcess->waitForBytesWritten();
}

void MainWindow::setupUi() {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    setWindowTitle("ServerUI: Main Window");

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

    // Connect the new slot to get input from the message box and send it to the server
    connect(this, &MainWindow::sendInput, this, &MainWindow::sendConfigFilePathToServer);

    this->setCentralWidget(centralWidget);
}
