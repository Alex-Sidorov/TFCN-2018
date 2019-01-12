#ifndef PORT_H
#define PORT_H

#include <QSerialPort>
#include "crc.h"
#include "bit_stuffing.h"
#include <QMessageBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QThread>

class Port :public QObject
{
       Q_OBJECT

public:
    void set_widget(QSerialPort*,QCheckBox*,QCheckBox*,QCheckBox*,QLineEdit*);
    static void set_number_port(int port);
    Port();
    virtual ~Port();

public slots:
    void slot_read();

signals:
    void recv_marker(QString);
    void new_data(QString,int);
    void error_crc(QByteArray);
    void is_not_power(QString);
    void error_my_message();
    void finished();
private:
    int work_with_services_fields(QString &data);

    QCheckBox *check_bit_stuffing;
    QCheckBox *check_crc;
    QCheckBox *check_power;
    QLineEdit *value_polinom;
    QSerialPort *port;

    static int number_port;

};

#endif // PORT_H
