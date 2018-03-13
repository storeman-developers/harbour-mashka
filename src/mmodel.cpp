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
    , m_unused(0)
    , m_unused_size(0)
{
    QTimer::singleShot(500, this, &MModel::reset);
}

bool MModel::busy() const
{
    return m_busy;
}

qint64 MModel::totalDataSize() const
{
    return m_total_size;
}

int MModel::totalUnused() const
{
    return m_unused;
}

qint64 MModel::totalUnusedSize() const
{
    return m_unused_size;
}

void MModel::reset()
{
    QtConcurrent::run(this, &MModel::resetImpl);
}

void MModel::clearData(const QString &name, const DataType &type)
{
    QtConcurrent::run(this, &MModel::clearDataImpl, name, type);
}

void MModel::clearUnusedData()
{
    QtConcurrent::run(this, &MModel::clearUnusedDataImpl);
}

void MModel::resetImpl()
{
    this->beginResetModel();
    m_busy = true;
    emit this->busyChanged();

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
            auto size = dirSize(it.next());
            auto dirname = it.fileName();
            if (!m_entries.contains(dirname))
            {
                m_names << dirname;
            }
            auto &e = m_entries[dirname];
            switch (i)
            {
            case 0:
                e.config_size = size;
                break;
            case 1:
                e.cache_size = size;
                break;
            case 2:
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
    m_busy = false;
    emit this->busyChanged();
    this->calculateTotal();
}

void MModel::clearDataImpl(const QString &name, const MModel::DataType &type)
{
    if (!m_entries.contains(name))
    {
        qWarning("Model doesn't contain the '%s' entry", qUtf8Printable(name));
        return;
    }
    m_busy = true;
    emit this->busyChanged();

    QList<QStandardPaths::StandardLocation> locations;
    if (type == AllData || type == ConfigData)
    {
        locations << QStandardPaths::GenericConfigLocation;
    }
    if (type == AllData || type == CacheData)
    {
        locations << QStandardPaths::GenericCacheLocation;
    }
    if (type == AllData || type == LocalData)
    {
        locations << QStandardPaths::GenericDataLocation;
    }

    qint64 cleared = 0;
    auto &e = m_entries[name];
    QVector<int> changed;
    for (const auto &location : locations)
    {
        auto dirpath = QStandardPaths::locate(location, name, QStandardPaths::LocateDirectory);
        if (dirpath.isEmpty())
        {
            continue;
        }
        auto size = dirSize(dirpath);
        if (QDir(dirpath).removeRecursively())
        {
            qDebug("Removed %lld bytes '%s'", size, qUtf8Printable(dirpath));
            cleared += size;
            switch (location)
            {
            case QStandardPaths::GenericConfigLocation:
                e.config_size = 0;
                changed << ConfigSizeRole;
                break;
            case QStandardPaths::GenericCacheLocation:
                e.cache_size = 0;
                changed << CacheSizeRole;
                break;
            case QStandardPaths::GenericDataLocation:
                e.data_size = 0;
                changed << LocalDataSizeRole;
                break;
            default:
                Q_UNREACHABLE();
            }
        }
        else
        {
            qWarning("Error removing '%s'", qUtf8Printable(dirpath));
            emit this->error(dirpath);
        }
    }

    if (!changed.isEmpty())
    {
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
    }

    m_busy = false;
    emit this->busyChanged();
    this->calculateTotal();
    if (cleared > 0)
    {
        emit this->cleared(cleared);
    }
}

void MModel::clearUnusedDataImpl()
{
    m_busy = true;
    emit this->busyChanged();

    qint64 cleared = 0;
    for (auto it = m_entries.cbegin(); it != m_entries.cend(); ++it)
    {
        auto &entry = it.value();
        if (entry.installed)
        {
            continue;
        }
        auto &name = it.key();
        QStringList dirpaths = {
            QStandardPaths::locate(QStandardPaths::GenericConfigLocation, name, QStandardPaths::LocateDirectory),
            QStandardPaths::locate(QStandardPaths::GenericCacheLocation, name, QStandardPaths::LocateDirectory),
            QStandardPaths::locate(QStandardPaths::GenericDataLocation, name, QStandardPaths::LocateDirectory)
        };
        for (const auto &dirpath : dirpaths)
        {
            if (!dirpath.isEmpty())
            {
                auto size = dirSize(dirpath);
                if (QDir(dirpath).removeRecursively())
                {
                    qDebug("Removed %lld bytes '%s'", size, qUtf8Printable(dirpath));
                    cleared += size;
                }
                else
                {
                    qWarning("Error removing '%s'", qUtf8Printable(dirpath));
                    emit this->error(dirpath);
                }
            }
        }
    }

    this->resetImpl();

    if (cleared > 0)
    {
        emit this->cleared(cleared);
    }
}

void MModel::calculateTotal()
{
    m_unused = 0;
    m_total_size = 0;
    m_unused_size = 0;
    for (const auto &e : m_entries)
    {
        auto s = e.config_size + e.cache_size + e.data_size;
        m_total_size += s;
        if (!e.installed)
        {
            ++m_unused;
            m_unused_size += s;
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
