#ifndef CRC_H
#define CRC_H

#include <QString>
#include <QByteArray>

QString div_polinom(QString &str, QString &polinom);
void add_crc(QString &str, QString &polinom);
bool is_complete_send(QString &str, QString &polinom);


#endif // CRC_H
