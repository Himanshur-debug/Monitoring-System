#ifndef GRAPHWINDOW_H
#define GRAPHWINDOW_H


#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

#include <QtCharts>
using namespace QtCharts;

class GraphWindow: public QMainWindow
{
    Q_OBJECT
public:
    explicit GraphWindow(QSqlDatabase *db, QWidget *parent = nullptr);
    ~GraphWindow();

private slots:
    void on_drawButton_clicked();

private:
    void setupUi();
    void fetchData(const QString &userId);
    void drawGraph(const QString &userId);

    QSqlDatabase *db;
    QSqlQuery query;
    QLineSeries *cpuSeries;
    QLineSeries *ramSeries;
    QLineSeries *hddSeries;
    QChart *chart;
    QChartView *chartView;
    QVBoxLayout *mainLayout;
    QHBoxLayout *inputLayout;
    QPushButton *drawButton;
    QLineEdit *macAddressInput;
};

#endif // GraphWindow_H
