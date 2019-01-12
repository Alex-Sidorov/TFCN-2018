#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "port.h"
#include <QMainWindow>
#include <QMessageBox>
#include <QThread>
#include <QDebug>
#include <QSharedPointer>

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
    void on_send_button_clicked();
    void slot_error_in(QSerialPort::SerialPortError);
    void slot_error_out(QSerialPort::SerialPortError);
    void on_disconnect_button_in_clicked();
    void on_boud_in_currentIndexChanged(const QString &value);
    void on_if_bit_staffing_clicked(bool checked);
    void on_if_crc_clicked(bool checked);
    void slot_error_message();
    void on_connect_button_out_clicked();
    void on_connect_button_in_clicked();
    void on_disconnect_button_out_clicked();
    void slot_error_crc(QByteArray);
    void slot_print_new_data(QString,int);
    void slot_recv_marker(QString);
    void slot_send_message(QString &data);
    void slot_not_power(QString);
    void on_send_marker_clicked();
    void slot_read();

signals:
    void ready_read(bool, bool,bool,QString);

private:
    Ui::MainWindow *ui;
    QSerialPort *com_port_in;
    QSerialPort *com_port_out;

    QString real_message;
    QString new_message;

    int priority;
    int far_station;
    bool monitoring;

    bool if_null_and_one(QString &str, int size);
    void is_not_monitoring()const;
};

#endif // MAINWINDOW_H
