#pragma once

#include <QStringList>

struct MEntry
{
    MEntry() = default;
    ~MEntry() = default;

    bool exists() const {
        return config_size > 0 || cache_size > 0 || data_size > 0;
    }

    bool installed{false};
    qint64 config_size{0};
    qint64 cache_size{0};
    qint64 data_size{0};
    QString title;
    QString icon;
    QStringList config_paths;
    QStringList cache_paths;
    QStringList data_paths;
};
