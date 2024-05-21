#include <graphwindow.h>
#include <QInputDialog>
#include <QDebug>
#include <QSqlError>
//#include <QSqlQuery>


GraphWindow::GraphWindow(QSqlDatabase *db, QWidget *parent): QMainWindow(parent), db(db), query(*db) {
    this->resize(400, 400);
    setupUi();
}

GraphWindow::~GraphWindow()
{
    delete db;
    delete cpuSeries;
    delete ramSeries;
    delete hddSeries;
    delete chart;
    delete chartView;
    delete mainLayout;
    delete inputLayout;
    delete drawButton;
    delete comboBox;
}

void GraphWindow::setupUi()
{
    // Fetch MAC addresses from the database
    QStringList macAddresses;
    QString queryString = "SELECT mac_address FROM client_details;";
    query.prepare(queryString);
    query.exec();

    while (query.next()) {
        macAddresses.append(query.value(0).toString());
    }

    //set Layout
    QWidget *centralWidget = new QWidget(this);
    mainLayout = new QVBoxLayout(centralWidget);
    setWindowTitle("ServerUI: Graph Window");

    QHBoxLayout *layout = new QHBoxLayout(centralWidget);
    QLabel *macAddressLabel = new QLabel("Mac Address:", this);
    comboBox = new QComboBox(centralWidget);
    comboBox->addItems(macAddresses);

    drawButton = new QPushButton("Get Resource Usages Graph", centralWidget);

    layout->addWidget(macAddressLabel);
    layout->addWidget(comboBox);


    mainLayout->addLayout(layout);
    mainLayout->addWidget(drawButton);

    this->setCentralWidget(centralWidget);
    connect(drawButton, &QPushButton::clicked, this, &GraphWindow::on_drawButton_clicked);

}

void GraphWindow::on_drawButton_clicked(int index)
{
    QString macAddress = comboBox->currentText();
    fetchData(macAddress);
}

void GraphWindow::fetchData(const QString &macAddress) {

    QString queryString = "SELECT CPU_Utilization, RAM_Usage, HDD_Utilization FROM system_Info WHERE mac_address = ?";
    query.prepare(queryString);
    query.addBindValue(macAddress);

    if (!query.exec() || !query.next()) {
        QMessageBox::critical(this, "Error", "No MAC address Found. \nFailed to fetch data.");
        return;
    }
    drawGraph(macAddress);
}

void GraphWindow::drawGraph(const QString &macAddress) {

    cpuSeries = new QLineSeries();  cpuSeries->setName("CPU");
    ramSeries = new QLineSeries();  ramSeries->setName("RAM");
    hddSeries = new QLineSeries();  hddSeries->setName("HDD");
    int i = 0;
    while(query.next()) {
        cpuSeries->append(i, query.value(0).toInt());
        ramSeries->append(i, query.value(1).toInt());
        hddSeries->append(i, query.value(2).toInt());
        i++;
    }

    chart = new QChart();
    chart->addSeries(cpuSeries);
    chart->addSeries(ramSeries);
    chart->addSeries(hddSeries);
    chart->createDefaultAxes();
    chart->setTitle("System Usage for User " + macAddress);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    setCentralWidget(chartView);
}
