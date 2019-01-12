#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSerialPort>
#include <QMainWindow>
#include <QMessageBox>
#include <QTimer>
#include <QTime>
#include <qalgorithms.h>
#include "bit_stuffing.h"
#include "crc.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_connect_button_clicked();
    void on_send_button_clicked();
    void slot_read();
    void slot_error(QSerialPort::SerialPortError);
    void on_disconnect_button_clicked();
    void on_boud_currentIndexChanged(const QString &value);
    void on_if_bit_staffing_clicked(bool checked);
    void on_if_crc_clicked(bool checked);
    void timeout();

private:
    Ui::MainWindow *ui;
    QSerialPort *com_port;
    QString shuffel_message;
    QString _message;
    QTimer *timer;

    void manipulations_with_packages(QString &str);
    void check_recv_message(QByteArray &data, int &lose_package);
    bool if_null_and_one(QString &str, int size);

    void delete_package(QString &str);
    void shuffel_packages(QString &str);
    void add_flag(QString &str);
    void delete_flag(QByteArray &data);
    int find_deleted_package(QByteArray &data);
    void sort_packages(QByteArray &data);

};

#endif // MAINWINDOW_H
