#pragma once

#include <QStringList>

struct KnownApp
{
    QString     name;
    QStringList config;
    QStringList cache;
    QStringList local_data;
};
