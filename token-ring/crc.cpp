#include "crc.h"

static const int SIZE_BYTE = 8;

QString div_polinom(QString &str, QString &polinom)
{
    bool ok = true;
    int divider = polinom.toInt(&ok,2);

    str.remove(0,str.indexOf('1'));
    while(str.size()>=polinom.size())
    {
        int value = str.mid(0,polinom.size()).toInt(&ok,2);
        value ^= divider;

        str.replace(0,polinom.size(),QString("%1").arg(value,SIZE_BYTE,2,QChar('0')));
        str.remove(0,str.indexOf('1'));
    }
    if(str.indexOf('1')==-1)
    {
        str.clear();
    }
    return str;
}

void add_crc(QString &str, QString &polinom)
{
    polinom.remove(0,polinom.indexOf('1'));
    if(polinom.isEmpty())
    {
        return;
    }

    QString temp;

    for (int i = 0; i<str.size(); i++)
    {
        temp+=QString("%1").arg(static_cast<unsigned char>(str.toLatin1()[i]),SIZE_BYTE,2,QChar('0'));
    }

    temp.append(QString("%1").arg(0,polinom.size()-1,2,QChar('0')));

    QString remainder = div_polinom(temp,polinom);

    bool ok = true;
    str.append(remainder.toInt(&ok,2));
}

bool is_complete_send(QString &str, QString &polinom)
{
    polinom.remove(0,polinom.indexOf('1'));
    if(polinom.isEmpty())
    {
        return true;
    }

    QString temp;
    foreach (QChar value, str)
    {
        temp+=QString("%1").arg(value.toLatin1(),SIZE_BYTE,2,QChar('0'));
    }
    if(div_polinom(temp,polinom).isEmpty())
    {
        return true;
    }
    else
    {
        return false;
    }
}
