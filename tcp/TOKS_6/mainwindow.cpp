#include "mainwindow.h"
#include "ui_mainwindow.h"

static const int SIZE_BYTE = 8;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), timer(new QTimer)
{
    ui->setupUi(this);
    com_port = new QSerialPort;
    timer->setInterval(2000);
    connect(com_port,SIGNAL(readyRead()),SLOT(slot_read()));
    connect(com_port,SIGNAL(error(QSerialPort::SerialPortError)),SLOT(slot_error(QSerialPort::SerialPortError)));
    connect(ui->mesage,SIGNAL(returnPressed()),SLOT(on_send_button_clicked()));
    connect(timer,SIGNAL(timeout()),SLOT(timeout()));
}

MainWindow::~MainWindow()
{
    if(com_port->isOpen())
    {
        com_port->close();
    }
    delete timer;
    delete com_port;
    delete ui;
}

void MainWindow::timeout()
{
    if(!ui->mesage->isEnabled() && com_port->isOpen())
    {
        com_port->write(_message.toLatin1());
        timer->start();
    }
    else
    {
        timer->stop();
    }
}

void MainWindow::slot_error(QSerialPort::SerialPortError error)
{
    if(error!=QSerialPort::NoError)
    {
        ui->chat->addItem("Error connected");
        ui->disconnect_button->setEnabled(false);
        ui->connect_button->setEnabled(true);
    }
}

void MainWindow::slot_read()
{
    QByteArray data;
    data.append(com_port->readAll());

    if(data.size()<5)
    {
        timer->stop();
        if(data.toInt()==1)
        {
            ui->statusBar->showMessage("Message sended");
        }
        else
        {
            ui->statusBar->showMessage("Message did not send(package " + data.remove(0,1) + " was lost)");
        }
        ui->mesage->setEnabled(true);
        return;
    }

    if(ui->if_crc->isChecked())
    {
        QString temp;
        for (int i = 0; i<data.size(); i++)
        {
            temp+=QString("%1").arg(static_cast<unsigned char>(data[i]),SIZE_BYTE,2,QChar('0'));
        }

        QString polinom = ui->polinom->text();
        if(!div_polinom(temp,polinom).isEmpty())
        {
            QMessageBox::warning(this,tr("ERROR"), tr("Message is invalid."));
            return;
        }
        else
        {
            data.resize(data.size()-1);
        }
    }

    if(ui->if_bit_staffing->isChecked())
    {
        QString temp;
        decode(temp,data);
        data = temp.toLatin1();
    }

    int lose_package = -1;
    check_recv_message(data,lose_package);
    delete_flag(data);
    ui->chat->addItem("He\\She: " + data);

    if(!ui->ack->isChecked())
    {
        return;
    }
    if(lose_package!=-1)
    {
        com_port->write("2"+QByteArray::number(lose_package));
    }
    else
    {
        com_port->write("1");
    }
}

void MainWindow::delete_flag(QByteArray &data)
{
    data.remove(0,2);
    data.remove(data.size()-2,2);
    for(int i=1 ; i<data.size(); i++)
    {
        data.remove(i,4);
    }
}


bool compare(const QByteArray &one, const QByteArray &two)
{
    return one.mid(0,2).toInt() < two.mid(0,2).toInt();
}

int MainWindow::find_deleted_package(QByteArray &data)
{
    int index = 0;
    for(int i=0; i<data.size(); i+=5, index++)
    {
        if(data.mid(i,2).toInt()!=index)
        {
            return index;
        }
    }
    return -1;
}

void MainWindow::sort_packages(QByteArray &data)
{
    QList<QByteArray> list;
    while(!data.isEmpty())
    {
        list.push_back(data.mid(data.size()-5,5));
        data.remove(data.size()-5,5);
    }
    qSort(list.begin(),list.end(),compare);
    foreach (QByteArray val, list)
    {
        data.push_back(val);
    }
}


void MainWindow::check_recv_message(QByteArray &data, int &lose_package)
{
    if(ui->options->currentIndex() == 1)
    {
        lose_package = find_deleted_package(data);
    }
    else if(ui->options->currentIndex() == 2)
    {
        sort_packages(data);
    }
}

void MainWindow::add_flag(QString &str)
{
    str.push_front("00");
    int flag = 1;
    QString temp;
    for(int i = 3; i < str.size();i+=5, flag++)
    {
        temp = flag < 10 ? "0"+QString::number(flag)+"0"+QString::number(flag)
                        : QString::number(flag) + QString::number(flag);
        str.insert(i,temp);
    }
    str.push_back("0!");
}

void MainWindow::delete_package(QString &str)
{
    qsrand(QTime::currentTime().msec());
    int index = 0;
    index = qrand() % str.size();
    str.remove(index - index%5,5);
}

void MainWindow::shuffel_packages(QString &data)
{
    QStringList list;
    while(!data.isEmpty())
    {
        list.push_back(data.mid(data.size()-5,5));
        data.remove(data.size()-5,5);
    }
    std::random_shuffle(list.begin(),list.end());
    foreach (QString val, list)
    {
        shuffel_message+=val[2];
        data.push_back(val);
    }
}

void MainWindow::manipulations_with_packages(QString &str)
{
    if(ui->options->currentIndex() == 1)
    {
        delete_package(str);
    }
    else if(ui->options->currentIndex() == 2)
    {
        shuffel_packages(str);
    }
}

void MainWindow::on_send_button_clicked()
{

    if(com_port->isOpen() && !ui->mesage->text().isEmpty())
    {
        QString str = ui->mesage->text();
        add_flag(str);
        manipulations_with_packages(str);
        if(ui->if_bit_staffing->isChecked())
        {
            QString flag = ui->staffing->text();
            if(!if_null_and_one(flag, SIZE_BYTE))
            {
                QMessageBox::warning(this,tr("ERROR"), tr("There is invalid flag for bit staffing."));
                return;
            }
            QString temp;
            code(temp,str,flag);
            str = temp;
        }
        if(ui->if_crc->isChecked())
        {
            QString polinom = ui->polinom->text();
            if(!if_null_and_one(polinom, 9))
            {
                QMessageBox::warning(this,tr("ERROR"), tr("There is invalid polinom."));
                return;
            }
            add_crc(str,polinom);
        }
        com_port->write(str.toLatin1());
        _message = str;
        timer->start();
        ui->mesage->setEnabled(false);
        ui->chat->addItem("I:" + ui->mesage->text());
        ui->mesage->clear();
        ui->statusBar->clearMessage();
        ui->shuffel_message->setText(shuffel_message.isEmpty() ? "SHUFFEL MESSAGE:"
                                                               : "SHUFFEL MESSAGE:"+shuffel_message);
        shuffel_message.clear();
    }
}

void MainWindow::on_connect_button_clicked()
{
    com_port->setPortName(ui->port_box->currentText());
    if(com_port->open(QIODevice::ReadWrite))
    {
        com_port->setBaudRate(ui->boud->currentText().toInt());
        com_port->setDataBits(QSerialPort::Data8);
        com_port->setParity(QSerialPort::NoParity);
        com_port->setStopBits(QSerialPort::OneStop);
        com_port->setFlowControl(QSerialPort::NoFlowControl);

        ui->chat->addItem("Connected to "+ui->port_box->currentText());
        ui->connect_button->setEnabled(false);
        ui->disconnect_button->setEnabled(true);
        ui->mesage->setEnabled(true);
    }
}

void MainWindow::on_disconnect_button_clicked()
{
    if(com_port->isOpen())
    {
        com_port->close();
        ui->chat->addItem("Disconnect.");
        ui->disconnect_button->setEnabled(false);
        ui->connect_button->setEnabled(true);
        ui->statusBar->clearMessage();
    }
}


void MainWindow::on_boud_currentIndexChanged(const QString &value)
{
    if(com_port->isOpen())
    {
        if(!com_port->setBaudRate(value.toInt()))
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
