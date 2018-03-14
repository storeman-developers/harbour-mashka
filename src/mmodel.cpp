#include "mmodel.h"

#include <QTimer>
#include <QtConcurrentRun>
#include <QStandardPaths>
#include <QDirIterator>
#include <QSettings>
#include <QFileInfo>


qint64 dirSize(const QString &path)
{
    qint64 res = 0;
    QDirIterator it(path, QDir::Files | QDir::Hidden, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        it.next();
        res += it.fileInfo().size();
    }
    return res;
}


MModel::MModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_busy(false)
    , m_resetting(false)
    , m_unused_apps_count(0)
    , m_total_cache_size(0)
    , m_total_config_size(0)
    , m_total_localdata_size(0)
    , m_unused_config_size(0)
    , m_unused_cache_size(0)
    , m_unused_localdata_size(0)
{
    QTimer::singleShot(500, this, &MModel::reset);
}

bool MModel::busy() const
{
    return m_busy;
}

bool MModel::resetting() const
{
    return m_resetting;
}

qint64 MModel::totalConfigSize() const
{
    return m_total_config_size;
}

qint64 MModel::totalCacheSize() const
{
    return m_total_cache_size;
}

qint64 MModel::totalLocaldataSize() const
{
    return m_total_localdata_size;
}

int MModel::unusedAppsCount() const
{
    return m_unused_apps_count;
}

qint64 MModel::unusedConfigSize() const
{
    return m_unused_config_size;
}

qint64 MModel::unusedCacheSize() const
{
    return m_unused_cache_size;
}

qint64 MModel::unusedLocaldataSize() const
{
    return m_unused_localdata_size;
}

void MModel::reset()
{
    QtConcurrent::run(this, &MModel::resetImpl);
}

void MModel::deleteData(const QString &name, DataTypes types)
{
    QtConcurrent::run(this, &MModel::deleteDataImpl, name, types);
}

void MModel::deleteUnusedData(DataTypes types)
{
    QtConcurrent::run(this, &MModel::deleteUnusedDataImpl, types);
}

void MModel::setBusy(bool busy)
{
    m_busy = busy;
    emit this->busyChanged();
}

qint64 MModel::removeDir(const QString &path)
{
    auto size = dirSize(path);
#ifndef SAFE_MODE
    if (QDir(path).removeRecursively())
    {
        qDebug("Deleted %lld bytes '%s'", size, qUtf8Printable(path));
        return size;
    }
    qWarning("Error deleting '%s'", qUtf8Printable(path));
    emit this->deletionError(path);
    return 0;
#else
    qDebug("SAFE MODE: Deleted %lld bytes '%s'", size, qUtf8Printable(path));
    return size;
#endif
}

QVector<int> MModel::clearEntry(MEntry &entry, qint64 &deleted, DataTypes types)
{
    QVector<int> changed;

    if (types.testFlag(ConfigData) && entry.config_size > 0)
    {
        auto c = removeDir(entry.config_path);
        if (c > 0)
        {
            entry.config_size = 0;
            deleted += c;
            changed << ConfigSizeRole;
        }
    }
    if (types.testFlag(CacheData) && entry.cache_size > 0)
    {
        auto c = removeDir(entry.cache_path);
        if (c > 0)
        {
            entry.cache_size = 0;
            deleted += c;
            changed << CacheSizeRole;
        }
    }
    if (types.testFlag(LocalData) && entry.data_size > 0)
    {
        auto c = removeDir(entry.data_path);
        if (c > 0)
        {
            entry.data_size = 0;
            deleted += c;
            changed << LocalDataSizeRole;
        }
    }

    return changed;
}

void MModel::resetImpl()
{
    this->setBusy(true);
    this->beginResetModel();
    m_resetting = true;
    emit this->resettingChanged();

    m_names.clear();
    m_entries.clear();

    QStringList filters(QStringLiteral("harbour-*"));
    QStringList app_paths = {
        QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation),
        QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation),
        QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
    };

    for (int i = 0; i < 3; ++i)
    {
        QDirIterator it(app_paths[i], filters, QDir::Dirs | QDir::NoDotAndDotDot);
        while (it.hasNext())
        {
            auto dirpath = it.next();
            auto size = dirSize(dirpath);
            auto dirname = it.fileName();
            if (!m_entries.contains(dirname))
            {
                m_names << dirname;
            }
            auto &e = m_entries[dirname];
            switch (i)
            {
            case 0:
                e.config_path = dirpath;
                e.config_size = size;
                break;
            case 1:
                e.cache_path = dirpath;
                e.cache_size = size;
                break;
            case 2:
                e.data_path = dirpath;
                e.data_size = size;
                break;
            default:
                Q_UNREACHABLE();
            }
        }
    }

    QString name_key(QStringLiteral("Desktop Entry/Name"));
    QString icon_key(QStringLiteral("Desktop Entry/Icon"));
    QString icon_tmpl(QStringLiteral("/usr/share/icons/hicolor/86x86/apps/%1.png"));
    for (const auto &path : QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation))
    {
        QDirIterator it(path, filters, QDir::Files);
        while (it.hasNext())
        {
            auto desktop_path = it.next();
            auto name = it.fileInfo().completeBaseName();
            if (m_entries.contains(name))
            {
                auto &e = m_entries[name];
                e.installed = true;
                QSettings desktop(desktop_path, QSettings::IniFormat);
                e.title = desktop.value(name_key).toString();
                auto icon = icon_tmpl.arg(desktop.value(icon_key, name).toString());
                if (QFileInfo(icon).isFile())
                {
                    e.icon = icon;
                }
            }
        }
    }

    this->endResetModel();
    this->calculateTotal();
    this->setBusy(false);
    m_resetting = false;
    emit this->resettingChanged();
}

void MModel::deleteDataImpl(const QString &name, DataTypes types)
{
    if (!m_entries.contains(name))
    {
        qWarning("Model doesn't contain the '%s' entry", qUtf8Printable(name));
        return;
    }
    this->setBusy(true);

    qint64 deleted = 0;
    auto &e = m_entries[name];
    auto changed = clearEntry(e, deleted, types);
    int row = m_names.indexOf(name);

    if (e.config_size + e.cache_size + e.data_size == 0)
    {
        this->beginRemoveRows(QModelIndex(), row, row);
        m_names.removeOne(name);
        m_entries.remove(name);
        this->endRemoveRows();
    }
    else
    {
        auto ind = this->createIndex(row, 0);
        this->dataChanged(ind, ind, changed);
    }

    if (deleted > 0)
    {
        this->calculateTotal();
        emit this->dataDeleted(deleted);
    }
    this->setBusy(false);
}

void MModel::deleteUnusedDataImpl(DataTypes types)
{
    this->setBusy(true);

    qint64 deleted = 0;
    auto it = m_entries.begin();
    while (it != m_entries.end())
    {
        auto &e = it.value();
        if (e.installed)
        {
            ++it;
            continue;
        }

        auto changed = this->clearEntry(e, deleted, types);
        auto &name = it.key();
        int row = m_names.indexOf(name);

        if (e.config_size + e.cache_size + e.data_size == 0)
        {
            this->beginRemoveRows(QModelIndex(), row, row);
            m_names.removeOne(name);
            it = m_entries.erase(it);
            this->endRemoveRows();
        }
        else
        {
            auto ind = this->createIndex(row, 0);
            this->dataChanged(ind, ind, changed);
            ++it;
        }
    }

    if (deleted > 0)
    {
        this->calculateTotal();
        emit this->dataDeleted(deleted);
    }

    this->setBusy(false);
}

void MModel::calculateTotal()
{
    m_unused_apps_count = 0;
    m_total_localdata_size = 0;
    m_total_cache_size = 0;
    m_total_config_size = 0;
    m_unused_config_size = 0;
    m_unused_cache_size = 0;
    m_unused_localdata_size = 0;
    for (const auto &e : m_entries)
    {
        m_total_localdata_size += e.config_size;
        m_total_cache_size     += e.cache_size;
        m_total_config_size    += e.data_size;
        if (!e.installed)
        {
            ++m_unused_apps_count;
            m_unused_config_size    += e.config_size;
            m_unused_cache_size     += e.cache_size;
            m_unused_localdata_size += e.data_size;
        }
    }
    emit this->totalChanged();
}

int MModel::rowCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? m_names.size() : 0;
}

QVariant MModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    auto &name  = m_names[index.row()];
    auto &entry = m_entries[name];
    switch (role)
    {
    case NameRole:
        return name;
    case TitleRole:
        return entry.title.isEmpty() ? name : entry.title;
    case IconRole:
        return entry.icon;
    case InstalledRole:
        return entry.installed;
    case ConfigSizeRole:
        return entry.config_size;
    case CacheSizeRole:
        return entry.cache_size;
    case LocalDataSizeRole:
        return entry.data_size;
    case SortRole:
        return QString::number(int(!entry.installed))
                    .append(entry.title.isEmpty() ? name : entry.title);
    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> MModel::roleNames() const
{
    return {
        { NameRole,          "name" },
        { TitleRole,         "title" },
        { IconRole,          "icon" },
        { InstalledRole,     "installed" },
        { ConfigSizeRole,    "configSize" },
        { CacheSizeRole,     "cacheSize" },
        { LocalDataSizeRole, "localDataSize" }
    };
}
