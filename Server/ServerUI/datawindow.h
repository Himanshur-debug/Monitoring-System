#ifndef DATAWINDOW_H
#define DATAWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSqlTableModel>
#include <QPushButton>

class DataWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit DataWindow(QWidget *parent = nullptr);
    ~DataWindow();

private slots:

private:
    QTableView *tableView;
    QSqlTableModel *model;
    QPushButton *buttonClient_details;
    QPushButton *buttonSystem_Info;
    QSqlDatabase db;
    void setupUi();
    void fetchData(const QString &tableName);
};

#endif


