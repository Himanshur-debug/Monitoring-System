#include <datawindow.h>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include <graphwindow.h>

#include <QDebug>

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QtCharts>

using namespace QtCharts;

DataWindow::DataWindow(QWidget *parent) : QMainWindow(parent) {

    dbConnect();
    this->resize(500, 400);
    setupUi();
}

void DataWindow::dbConnect() {
    QFile configFile("config.json");
    if (!configFile.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "Failed to open config.json");
        return;
    }

    QJsonParseError parseError;
    QJsonDocument configDoc = QJsonDocument::fromJson(configFile.readAll(), &parseError);
    configFile.close();

    if (parseError.error != QJsonParseError::NoError) {
        QMessageBox::critical(this, "Error", "Error parsing config.json: " + parseError.errorString());
        return;
    }

    QJsonArray configArray = configDoc.array();
    if (configArray.size() < 2) {
        QMessageBox::critical(this, "Error", "Invalid config.json: Array size less than expected");
        return;
    }

    QJsonObject dbConfigObj = configArray[0].toObject();

    QString dbAddress = dbConfigObj["dbAddress"].toString();
    QString dbUser = dbConfigObj["dbUser"].toString();
    QString dbName = dbConfigObj["dbName"].toString();
    QString dbPassword = dbConfigObj["dbPassword"].toString();

    // Set up QSqlDatabase using the configuration values
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(dbAddress.split(":").first());
    db.setPort(dbAddress.split(":").last().toInt());
    db.setDatabaseName(dbName);
    db.setUserName(dbUser);
    db.setPassword(dbPassword);

    if (!db.open()) {
        QMessageBox::critical(this, "Error", "Could not open database");
        return;
    }
}
DataWindow::~DataWindow() {
    delete tableView;
    delete model;
    delete buttonClient_details;
    delete buttonSystem_Info;
    delete buttonGraph;
}

void DataWindow::setupUi() {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    setWindowTitle("ServerUI: Data Window");

    tableView = new QTableView(centralWidget);
    layout->addWidget(tableView);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonClient_details = new QPushButton("Client Details", centralWidget);
    buttonSystem_Info = new QPushButton("System Info", centralWidget);
    buttonGraph = nullptr;
    buttonLayout->addWidget(buttonClient_details);
    buttonLayout->addWidget(buttonSystem_Info);
    layout->addLayout(buttonLayout);

    this->setCentralWidget(centralWidget);

    connect(buttonClient_details, &QPushButton::clicked, [this, layout]() {
        if (layout->indexOf(buttonGraph) > 1) {
            layout->removeWidget(buttonGraph);
            delete buttonGraph;
            buttonGraph = nullptr;
            layout->update();
        }
        fetchData("client_details");
    });
    connect(buttonSystem_Info, &QPushButton::clicked, [this, layout]() {
        if (layout->indexOf(buttonGraph) == 1) {
            if (!buttonGraph) {
                    buttonGraph = new QPushButton("Graph", this);
            }
            layout->addWidget(buttonGraph);
            layout->update();

            connect(buttonGraph, &QPushButton::clicked, this, &DataWindow::createGraph);
        }
        fetchData("system_Info");
    });
}

void DataWindow::fetchData(const QString &tableName) {

    // Set up the model and fetch data
    model = new QSqlTableModel(this, db);
    model->setTable(tableName);
    model->select();

    // Set the model for the table view
    tableView->setModel(model);
}

void DataWindow::createGraph() {

    GraphWindow *graphWindow = new GraphWindow(db, this);
    graphWindow->show();
}
