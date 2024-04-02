#include <graphwindow.h>
#include <QInputDialog>
#include <QDebug>
#include <QSqlError>


GraphWindow::GraphWindow(QSqlDatabase db, QWidget *parent): QMainWindow(parent), db(db), query(db) {
    this->resize(400, 400);
    setupUi();
}

GraphWindow::~GraphWindow()
{
    delete cpuSeries;
    delete ramSeries;
    delete hddSeries;
    delete chart;
    delete chartView;
    delete mainLayout;
    delete inputLayout;
    delete drawButton;
    delete macAddressInput;
}

void GraphWindow::setupUi()
{
    QWidget *centralWidget = new QWidget(this);
    mainLayout = new QVBoxLayout(centralWidget);

    setWindowTitle("ServerUI: Graph Window");

    QLabel *macAddressLabel = new QLabel("Mac Address:", this);
    macAddressInput = new QLineEdit(centralWidget);
    drawButton = new QPushButton("Draw Graph", centralWidget);

    inputLayout = new QHBoxLayout();
    inputLayout->addWidget(macAddressLabel);
    inputLayout->addWidget(macAddressInput);

    mainLayout->addLayout(inputLayout);
    mainLayout->addWidget(drawButton);

    this->setCentralWidget(centralWidget);

    connect(drawButton, &QPushButton::clicked, this, &GraphWindow::on_drawButton_clicked);
}

void GraphWindow::on_drawButton_clicked()
{
    QString macAddress = macAddressInput->text();

    fetchData(macAddress);// columns);
}

void GraphWindow::fetchData(const QString &macAddress) {

    QString queryString = "SELECT CPU_Utilization, RAM_Usage, HDD_Utilization FROM system_Info WHERE mac_address = ?";
    query.prepare(queryString);
    query.addBindValue(macAddress);

    if (!query.exec() || !query.next()) {
//        qDebug() << query.lastError().text();
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
