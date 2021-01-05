#ifndef KNOWNAPP_H
#define KNOWNAPP_H

#include <QStringList>

struct KnownApp
{
    QString     name;
    QStringList config;
    QStringList cache;
    QStringList local_data;
};

#endif // KNOWNAPP_H
