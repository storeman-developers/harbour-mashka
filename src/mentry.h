#ifndef MENTRY_H
#define MENTRY_H


#include <QtGlobal>

struct MEntry
{
    MEntry()
        : installed(false)
        , config_size(0)
        , cache_size(0)
        , data_size(0)
    {}

    bool installed;
    qint64 config_size;
    qint64 cache_size;
    qint64 data_size;
    QString title;
    QString icon;
};

#endif // MENTRY_H
