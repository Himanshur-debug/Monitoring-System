#include <datawindow.h>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include <graphwindow.h>

#include <QDebug>

#include <QtCharts>

using namespace QtCharts;

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
    this->resize(500, 400);
    setupUi();
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
