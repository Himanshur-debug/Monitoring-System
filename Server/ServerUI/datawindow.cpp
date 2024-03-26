#include "datawindow.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>

DataWindow::DataWindow(QWidget *parent) : QMainWindow(parent) {
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("Sysmonitor");
    db.setUserName("root");
    db.setPassword("hello World @123");

    if (!db.open()) {
        QMessageBox::critical(this, "Error", "Could not open database");
        return;
    }
    setupUi();
}

DataWindow::~DataWindow() {
    delete tableView;
    delete model;
    delete buttonClient_details;
    delete buttonSystem_Info;
}

void DataWindow::setupUi() {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    tableView = new QTableView(centralWidget);
    layout->addWidget(tableView);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonClient_details = new QPushButton("Fetch Data from client_details", centralWidget);
    buttonSystem_Info = new QPushButton("Fetch Data from system_Info", centralWidget);
    buttonLayout->addWidget(buttonClient_details);
    buttonLayout->addWidget(buttonSystem_Info);
    layout->addLayout(buttonLayout);

    this->setCentralWidget(centralWidget);

    connect(buttonClient_details, &QPushButton::clicked, [this]() {
        fetchData("client_details");
    });
    connect(buttonSystem_Info, &QPushButton::clicked, [this]() {
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

