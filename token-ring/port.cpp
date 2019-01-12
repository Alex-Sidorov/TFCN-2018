#include "port.h"

static const int SIZE_MARKER = 1;
static const int SIZE_BYTE = 8;

int Port::number_port = 0;

void Port::set_widget(QSerialPort *_port, QCheckBox *crc, QCheckBox *power, QCheckBox *bit_stuffing, QLineEdit *polinom)
{
    check_bit_stuffing = bit_stuffing;
    check_crc = crc;
    check_power = power;
    value_polinom = polinom;
    port = _port;
}

void Port::slot_read()
{
    if(!port->isOpen())
    {
        emit finished();
        return;
    }
    bool power = check_power->isChecked();
    bool if_crc = check_crc->isChecked();
    bool if_bit_stuffing = check_bit_stuffing->isChecked();
    QString polinom = value_polinom->text();

    QByteArray work_data;
    work_data.append(port->readAll());
    QByteArray real_data(work_data);

    if(!power)
    {
        emit is_not_power(work_data);
        emit finished();
        return;
    }

    if(work_data.size()<=1)
    {
        emit recv_marker(work_data);
        emit finished();
        return;
    }

    bool if_error_crc = false;

    if(if_crc)
    {
        QString temp;
        for (int i = 0; i<work_data.size(); i++)
        {
            temp+=QString("%1").arg(static_cast<unsigned char>(work_data[i]),SIZE_BYTE,2,QChar('0'));
        }
        if(!div_polinom(temp,polinom).isEmpty())
        {
            if_error_crc = true;
            emit finished();
            return;
        }
        else
        {
            work_data.resize(work_data.size()-1);
        }
    }

    QString temp;
    if(if_bit_stuffing)
    {
        decode(temp,work_data);
    }
    else
    {
        temp = work_data;
    }

    int priority = real_data.mid(1,1).toInt();
    switch (work_with_services_fields(temp))
    {
    case 1:
        emit error_my_message();
        break;
    case 2:
        emit is_not_power(real_data);
        break;
    case 0:
        if(if_error_crc)
        {
            emit error_crc(real_data);
        }
        else
        {
            emit new_data(temp,priority);
        }
    }
    emit finished();
}

int Port::work_with_services_fields(QString &data)
{
    if(data.mid(1,1).toInt() == number_port)
    {
        return 1;
    }
    if(data.mid(0,1).toInt() != number_port)
    {
        return 2;
    }
    data.remove(0,2);
    return 0;
}

void Port::set_number_port(int _port)
{
    number_port = _port;
}

Port::Port()
{
    number_port = 0;
}

Port::~Port()
{

}

