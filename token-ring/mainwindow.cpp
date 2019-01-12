#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <tuple>
static const int SIZE_BYTE = 8;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    priority = 2;
    far_station = 0;
    monitoring = false;

    com_port_in = new QSerialPort;
    com_port_out = new QSerialPort;

    connect(com_port_in,SIGNAL(readyRead()),this,SLOT(slot_read()));
    connect(com_port_out,SIGNAL(error(QSerialPort::SerialPortError)),SLOT(slot_error_out(QSerialPort::SerialPortError)));
    connect(ui->mesage,SIGNAL(returnPressed()),SLOT(on_send_button_clicked()));
}

void MainWindow::slot_read()
{
    QThread *thread = new QThread;
    Port *read_object = new Port;

    read_object->set_widget(com_port_in,ui->if_crc,ui->if_power,ui->if_bit_staffing,ui->polinom);
    Port::set_number_port(ui->port_box_in->currentIndex() + 1);
    read_object->moveToThread(thread);
    connect(read_object,SIGNAL(new_data(QString,int)),this,SLOT(slot_print_new_data(QString,int)));
    connect(read_object,SIGNAL(recv_marker(QString)),this,SLOT(slot_recv_marker(QString)));
    connect(read_object,SIGNAL(is_not_power(QString)),this,SLOT(slot_not_power(QString)));
    connect(read_object,SIGNAL(error_my_message()),this,SLOT(slot_error_message()));
    connect(read_object,SIGNAL(error_crc(QByteArray)),this,SLOT(slot_error_crc(QByteArray)));

    connect(thread,SIGNAL(started()),read_object,SLOT(slot_read()));
    connect(read_object,SIGNAL(finished()),thread,SLOT(quit()));
    connect(read_object,SIGNAL(finished()),read_object,SLOT(deleteLater()));
    connect(thread,SIGNAL(finished()),thread,SLOT(deleteLater()));
    thread->start();
}


void MainWindow::slot_error_message()
{
    ui->chat->addItem("ERROR(recv my message)");
    ui->chat->item(ui->chat->count() - 1)->setBackgroundColor(QColor(Qt::red));
    if(com_port_out->isOpen())
    {
        com_port_out->write(QByteArray::number(ui->port_box_in->currentIndex()+1));
    }
}

void MainWindow::slot_not_power(QString message)
{
    if(com_port_out->isOpen())
    {
        com_port_out->write(message.toLatin1());
    }
}

void MainWindow::is_not_monitoring()const
{
    if(!monitoring)
    {
        ui->send_marker->setVisible(false);
        ui->far_station->setVisible(false);
        ui->box_priority->setVisible(false);
        ui->label_far_station->setVisible(false);
        /*ui->send_marker->setEnabled(false);
        ui->far_station->setEnabled(false);
        ui->box_priority->setEnabled(false);*/
    }
}


void MainWindow::slot_recv_marker(QString marker)
{
    if(!com_port_out->isOpen())
    {
        return;
    }
    is_not_monitoring();
    if(real_message.isEmpty() || marker.toInt() != ui->port_box_in->currentIndex() + 1)
    {
        if(monitoring && ui->box_priority->isChecked())
        {
            far_station == priority ? priority = 2 : priority += 2;
            marker = QString::number(priority);
        }
        com_port_out->write(marker.toLatin1());
    }
    else if(marker.toInt() == ui->port_box_in->currentIndex() + 1)
    {
        com_port_out->write(real_message.toLatin1());
        if(monitoring && ui->box_priority->isChecked())
        {
            far_station == priority ? priority = 2 : priority += 2;
        }
        ui->chat->addItem("I:" + new_message.toLatin1());
        real_message.clear();
        new_message.clear();
        ui->mesage->setEnabled(true);
        ui->chat->scrollToBottom();
    }
    if(monitoring)
    {
        ui->priority->setText("Priority:"+QString::number(priority));
    }
}

void MainWindow::slot_print_new_data(QString data, int _priority)
{
    is_not_monitoring();
    if(!data.isEmpty())
    {
        ui->chat->addItem("He\\She:" + data);
    }
    if(com_port_out->isOpen())
    {
        com_port_out->write(QString::number(_priority).toLatin1());
    }
    ui->chat->scrollToBottom();
}

void MainWindow::slot_error_crc(QByteArray data)
{
    QMessageBox::warning(this,tr("ERROR"), tr("Message is invalid."));
    if(com_port_out->isOpen())
    {
        com_port_out->write(data);
    }
}

MainWindow::~MainWindow()
{
    if(com_port_in->isOpen())
    {
        com_port_in->close();
    }
    if(com_port_out->isOpen())
    {
        com_port_out->close();
    }
    delete com_port_in;
    delete com_port_out;
    delete ui;
}

void MainWindow::slot_error_in(QSerialPort::SerialPortError error)
{
    if(error!=QSerialPort::NoError)
    {
        ui->chat->addItem("Error connected");
        ui->disconnect_button_in->setEnabled(false);
        ui->connect_button_in->setEnabled(true);
    }
}

void MainWindow::slot_error_out(QSerialPort::SerialPortError error)
{
    if(error!=QSerialPort::NoError)
    {
        ui->chat->addItem("Error connected");
        ui->disconnect_button_out->setEnabled(false);
        ui->connect_button_out->setEnabled(true);
    }
}

void MainWindow::slot_send_message(QString &data)
{
    if(!data.isEmpty())
    {
        new_message = data;
        QString services_fields =
                QString::number(ui->port_box_send->currentIndex() + 1)+
                QString::number(ui->port_box_in->currentIndex() + 1);

        data.push_front(services_fields);


        if(ui->if_bit_staffing->isChecked())
        {
            QString flag = ui->staffing->text();
            if(!if_null_and_one(flag, SIZE_BYTE))
            {
                QMessageBox::warning(this,tr("ERROR"), tr("There is invalid flag for bit staffing."));
                return;
            }
            QString temp;
            code(temp,data,flag);
            data = temp;
        }
        if(ui->if_crc->isChecked())
        {
            QString polinom = ui->polinom->text();
            if(!if_null_and_one(polinom, 9))
            {
                QMessageBox::warning(this,tr("ERROR"), tr("There is invalid polinom."));
                return;
            }
            add_crc(data,polinom);
        }
        real_message = data;
    }
}

void MainWindow::on_send_button_clicked()
{
    if(com_port_out->isOpen() && !ui->mesage->text().isEmpty())
    {
        QString message = ui->mesage->text();
        slot_send_message(message);
        ui->mesage->clear();
        ui->mesage->setEnabled(false);
    }
}

void MainWindow::on_connect_button_in_clicked()
{
    com_port_in->setPortName(ui->port_box_in->currentText());
    if(com_port_in->open(QIODevice::ReadWrite))
    {
        com_port_in->setBaudRate(ui->boud_in->currentText().toInt());
        com_port_in->setDataBits(QSerialPort::Data8);
        com_port_in->setParity(QSerialPort::NoParity);
        com_port_in->setStopBits(QSerialPort::OneStop);
        com_port_in->setFlowControl(QSerialPort::NoFlowControl);

        ui->chat->addItem("Connected to "+ui->port_box_in->currentText());
        ui->connect_button_in->setEnabled(false);
        ui->disconnect_button_in->setEnabled(true);

        ui->send_marker->setEnabled(true);
        ui->far_station->setEnabled(true);
        real_message.clear();
    }
}

void MainWindow::on_disconnect_button_in_clicked()
{
    if(com_port_in->isOpen())
    {
        com_port_in->close();
        ui->chat->addItem("Disconnect.");
        ui->disconnect_button_in->setEnabled(false);
        ui->connect_button_in->setEnabled(true);
        ui->send_marker->setEnabled(false);
        ui->far_station->setEnabled(false);
    }
}


void MainWindow::on_boud_in_currentIndexChanged(const QString &value)
{
    if(com_port_in->isOpen())
    {
        if(!com_port_in->setBaudRate(value.toInt()))
        {
            QMessageBox::warning(this,tr("ERROR"), tr("Could not change baudrate."));
        }
    }
}

void MainWindow::on_if_bit_staffing_clicked(bool checked)
{
    if(checked)
    {
        ui->staffing->setEnabled(true);
    }
    else
    {
        ui->staffing->setEnabled(false);
    }
}

bool MainWindow::if_null_and_one(QString &str, int size)
{
    if(str.isEmpty() || str.size()!=size)
    {
        return false;
    }

    foreach (QChar value, str)
    {
        if(value!='0' && value!='1')
        {
            return false;
        }
    }
    return true;
}

void MainWindow::on_if_crc_clicked(bool checked)
{
    if(checked)
    {
        ui->polinom->setEnabled(true);
    }
    else
    {
        ui->polinom->setEnabled(false);
    }
}

void MainWindow::on_connect_button_out_clicked()
{
    com_port_out->setPortName(ui->port_box_out->currentText());
    if(com_port_out->open(QIODevice::ReadWrite))
    {
        com_port_out->setBaudRate(ui->boud_out->currentText().toInt());
        com_port_out->setDataBits(QSerialPort::Data8);
        com_port_out->setParity(QSerialPort::NoParity);
        com_port_out->setStopBits(QSerialPort::OneStop);
        com_port_out->setFlowControl(QSerialPort::NoFlowControl);

        ui->chat->addItem("Connected to "+ui->port_box_out->currentText());
        ui->connect_button_out->setEnabled(false);
        ui->disconnect_button_out->setEnabled(true);
        //ui->send_marker->setEnabled(true);
        //ui->far_station->setEnabled(true);
        ui->send_marker->setVisible(true);
        ui->far_station->setVisible(true);
        ui->box_priority->setVisible(true);
        ui->label_far_station->setVisible(true);
    }
}

void MainWindow::on_disconnect_button_out_clicked()
{
    if(com_port_out->isOpen())
    {
        com_port_out->close();
        ui->chat->addItem("Disconnect.");
        ui->disconnect_button_out->setEnabled(false);
        ui->connect_button_out->setEnabled(true);
        ui->send_marker->setEnabled(false);
        ui->far_station->setEnabled(false);
    }
}

void MainWindow::on_send_marker_clicked()
{
    if(!com_port_out->isOpen())
    {
        return;
    }
    far_station = ui->far_station->value();
    if(!far_station)
    {
        QMessageBox::warning(this,"ERROR","Stations are nothing");
        return;
    }
    monitoring = true;
    ui->send_marker->setEnabled(false);
    ui->far_station->setEnabled(false);
    ui->mesage->setEnabled(true);
    QByteArray marker = QString::number(priority).toLatin1();
    com_port_out->write(marker);
}
