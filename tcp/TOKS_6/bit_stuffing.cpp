#include <bit_stuffing.h>

static const int SIZE_BYTE = 8;

void code(QString &result, QString &str, QString &flag)
{
    QString change_flag = flag;
    change_flag.insert(SIZE_BYTE-1,'1');

    QString temp;
    foreach (QChar value, str)
    {
        temp+=QString("%1").arg(value.toLatin1(),SIZE_BYTE,2,QChar('0'));
    }
    temp.replace(flag,change_flag);
    while(temp.size()%SIZE_BYTE)
    {
        temp.insert(temp.size()-temp.size()%SIZE_BYTE,QString('0'));
    }

    bool ok = true;

    result.append(flag.mid(0,SIZE_BYTE).toInt(&ok,2));
    while(!temp.isEmpty())
    {
        result.append(temp.mid(0,SIZE_BYTE).toInt(&ok,2));
        temp.remove(0,SIZE_BYTE);
    }
}

void decode(QString &result, QByteArray &str)
{
    QString flag = QString("%1").arg(static_cast<unsigned char>(str[0]),SIZE_BYTE,2,QChar('0'));
    QString change_flag = flag;
    change_flag.insert(SIZE_BYTE-1,'1');

    str.remove(0,1);

    QString temp;
    for (int i = 0; i<str.size(); i++)
    {
        temp+=QString("%1").arg(static_cast<unsigned char>(str[i]),SIZE_BYTE,2,QChar('0'));
    }

    temp.remove(temp.size()-SIZE_BYTE,temp.mid(temp.size()-SIZE_BYTE).indexOf('1'));
    temp.replace(change_flag,flag);

    bool ok = true;
    while(!temp.isEmpty())
    {
        result.append(temp.mid(0,SIZE_BYTE).toInt(&ok,2));
        temp.remove(0,SIZE_BYTE);
    }
}

