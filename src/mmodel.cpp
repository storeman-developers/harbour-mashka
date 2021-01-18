#include "mmodel.h"

#include <QTimer>
#include <QtConcurrentRun>
#include <QStandardPaths>
#include <QDirIterator>
#include <QSettings>
#include <QFileInfo>
#include <QTextCodec>

#include <mlite5/MDesktopEntry>

#include <known_apps.hpp>


qint64 getSize(const QString &path)
{
    qint64 res = 0;
    QFileInfo info(path);
    if (info.isDir())
    {
        QDirIterator it(path, QDir::AllEntries | QDir::Hidden, QDirIterator::Subdirectories);
        while (it.hasNext())
        {
            it.next();
            res += it.fileInfo().size();
        }
    }
    else if (info.isFile())
    {
        res += info.size();
    }
    return res;
}

void processKnownPaths(QStringList &paths, qint64 &size, const QStringList &known_paths)
{
    for (const auto &p : known_paths)
    {
        if (QFileInfo::exists(p))
        {
            paths << p;
            size += getSize(p);
        }
    }
}

template <typename T>
constexpr typename std::add_const<T>::type &asConst(T &t) noexcept
{
    return t;
}

MModel::MModel(QObject *parent)
    : QAbstractListModel{parent}
    , m_known_apps{known_apps_initializer()}
{
    const QLatin1String homevar{"$HOME"};
    const auto homepath = QDir::homePath();
    auto sethome = [&homevar, &homepath](QStringList &paths) {
        for (auto &p : paths)
        {
            p.replace(homevar, homepath);
        }
    };

    for (auto &app : m_known_apps)
    {
        sethome(app.config);
        sethome(app.cache);
        sethome(app.local_data);
    }

    QStringList exclude_paths{exclude_paths_initializer()};
    sethome(exclude_paths);
    m_exclude_paths = decltype(m_exclude_paths)::fromList(exclude_paths);

    QTimer::singleShot(500, this, &MModel::reset);
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

qint64 MModel::removePaths(const QStringList &paths)
{
    for (const auto &p : paths)
    {
        if (p.isEmpty())
        {
            qCritical("One of provided paths is empty");
            return 0;
        }
    }

    qint64 res = 0;
    for (const auto &p : paths)
    {
        auto size = getSize(p);

#ifndef SAFE_MODE
        QFileInfo info(p);
        bool ok = info.isDir()  ? QDir(p).removeRecursively() :
                  info.isFile() ? QFile::remove(p)            : false;

        if (ok)
        {
            qDebug("Deleted %lld bytes '%s'", size, qUtf8Printable(p));
            res += size;
        }
        else
        {
            qWarning("Error deleting '%s'", qUtf8Printable(p));
            emit this->deletionError(p);
        }
#else
        qDebug("SAFE MODE: Deleted %lld bytes '%s'", size, qUtf8Printable(p));
        res += size;
#endif
    }

    return res;
}

QVector<int> MModel::clearEntry(MEntry &entry, qint64 &deleted, DataTypes types)
{
    QVector<int> changed;

    if (types.testFlag(ConfigData) && entry.config_size > 0)
    {
        auto c = removePaths(entry.config_paths);
        if (c > 0)
        {
            entry.config_size = 0;
            deleted += c;
            changed << ConfigSizeRole;
        }
    }
    if (types.testFlag(CacheData) && entry.cache_size > 0)
    {
        auto c = removePaths(entry.cache_paths);
        if (c > 0)
        {
            entry.cache_size = 0;
            deleted += c;
            changed << CacheSizeRole;
        }
    }
    if (types.testFlag(LocalData) && entry.data_size > 0)
    {
        auto c = removePaths(entry.data_paths);
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

    // Process known apps
    for (const auto &app : asConst(m_known_apps))
    {
        MEntry e;
        processKnownPaths(e.config_paths, e.config_size, app.config);
        processKnownPaths(e.cache_paths, e.cache_size, app.cache);
        processKnownPaths(e.data_paths, e.data_size, app.local_data);
        if (e.exists())
        {
            qDebug("Found a known app '%s'", qUtf8Printable(app.name));
            m_names << app.name;
            m_entries.insert(app.name, e);
        }
    }

    // Search for other apps
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
            // Don't add paths of known apps
            if (m_exclude_paths.contains(dirpath))
            {
                continue;
            }
            auto size = getSize(dirpath);
            auto dirname = it.fileName();
            if (!m_entries.contains(dirname))
            {
                qDebug("Found a harbour app '%s'", qUtf8Printable(dirname));
                m_names << dirname;
            }
            auto &e = m_entries[dirname];
            switch (i)
            {
            case 0:
                e.config_paths << dirpath;
                e.config_size = size;
                break;
            case 1:
                e.cache_paths << dirpath;
                e.cache_size = size;
                break;
            case 2:
                e.data_paths << dirpath;
                e.data_size = size;
                break;
            default:
                Q_UNREACHABLE();
            }
        }
    }

    QLatin1String desktop_ext{".desktop"};
    std::vector<QString> icon_tmpls({
        QStringLiteral("/usr/share/icons/hicolor/86x86/apps/%1.png"),
        QStringLiteral("/usr/share/themes/sailfish-default/meegotouch/z1.0/icons/%1.png"),
        QStringLiteral("/usr/share/themes/sailfish-default/meegotouch/z1.75/icons/%1.png"),
    });
    for (auto it = m_entries.begin(), end = m_entries.end(); it != end; ++it)
    {
        MDesktopEntry desktop{QStandardPaths::locate(QStandardPaths::ApplicationsLocation, it.key() + desktop_ext)};
        if (desktop.isValid())
        {
            it->installed = true;
            it->title = desktop.name();
            auto icon_name = desktop.icon();
            for (const auto &tmpl : icon_tmpls)
            {
                auto icon = tmpl.arg(icon_name);
                if (QFileInfo(icon).isFile())
                {
                    it->icon = icon;
                    break;
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

    if (!e.exists())
    {
        this->beginRemoveRows(QModelIndex(), row, row);
        m_names.removeOne(name);
        m_entries.remove(name);
        this->endRemoveRows();
    }
    else
    {
        auto ind = this->createIndex(row, 0);
        emit this->dataChanged(ind, ind, changed);
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

        if (!e.exists())
        {
            this->beginRemoveRows(QModelIndex(), row, row);
            m_names.removeOne(name);
            it = m_entries.erase(it);
            this->endRemoveRows();
        }
        else
        {
            auto ind = this->createIndex(row, 0);
            emit this->dataChanged(ind, ind, changed);
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
    m_total_config_size = 0;
    m_total_cache_size = 0;
    m_total_localdata_size = 0;
    m_unused_config_size = 0;
    m_unused_cache_size = 0;
    m_unused_localdata_size = 0;
    for (const auto &e : asConst(m_entries))
    {
        m_total_config_size    += e.config_size;
        m_total_cache_size     += e.cache_size;
        m_total_localdata_size += e.data_size;
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
        return QVariant{};
    }

    auto &name  = m_names[index.row()];
    auto &entry = m_entries[name];
    auto &title = entry.title.isEmpty() ? name : entry.title;
    switch (role)
    {
    case NameRole:
        return name;
    case TitleRole:
        return title;
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
        return QString{title}.prepend(entry.installed ? QChar{'0'} : QChar{'1'});
    default:
        return QVariant{};
    }
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
