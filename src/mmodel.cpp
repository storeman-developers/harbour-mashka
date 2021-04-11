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
    qint64 res{0};
    const QFileInfo info{path};
    if (info.isDir())
    {
        QDirIterator it{path, QDir::AllEntries | QDir::Hidden, QDirIterator::Subdirectories};
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

    qint64 res{0};
    for (const auto &p : paths)
    {
        const auto size = getSize(p);

#ifndef SAFE_MODE
        const QFileInfo info{p};
        if (   (info.isDir()  && QDir{p}.removeRecursively())
            || (info.isFile() && QFile::remove(p)))
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
    const auto proc = [this, &types, &deleted, &changed](auto dtype, auto &size, const auto &paths, auto role) {
        if (types.testFlag(dtype) && size > 0)
        {
            const auto c = removePaths(paths);
            if (c > 0)
            {
                size = 0;
                deleted += c;
                changed << role;
            }
        }
    };

    proc(ConfigData, entry.config_size, entry.config_paths, ConfigSizeRole);
    proc(CacheData,  entry.cache_size,  entry.cache_paths,  CacheSizeRole);
    proc(LocalData,  entry.data_size,   entry.data_paths,   LocalDataSizeRole);

    return changed;
}

void MModel::search(int location_kind, MModel::getter_t getter)
{
    static const QStringList filters{QStringLiteral("harbour-*")};
    const auto location = QStandardPaths::writableLocation(static_cast<QStandardPaths::StandardLocation>(location_kind));
    QDirIterator it{location, filters, QDir::Dirs | QDir::NoDotAndDotDot};
    while (it.hasNext()) {
        const auto dirpath = it.next();
        // Don't add paths of known apps
        if (m_exclude_paths.contains(dirpath))
        {
            return;
        }
        const auto size = getSize(dirpath);
        const auto dirname = it.fileName();
        if (!m_entries.contains(dirname))
        {
            qDebug("Found a harbour app '%s'", qUtf8Printable(dirname));
            m_names << dirname;
        }
        auto vals = getter(m_entries[dirname]);
        vals.first << dirpath;
        vals.second = size;
    }
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
        processKnownPaths(e.cache_paths,  e.cache_size,  app.cache);
        processKnownPaths(e.data_paths,   e.data_size,   app.local_data);
        if (e.exists())
        {
            qDebug("Found a known app '%s'", qUtf8Printable(app.name));
            m_names << app.name;
            m_entries.insert(app.name, e);
        }
    }

    // Search for other apps
    search(QStandardPaths::GenericConfigLocation, [](MEntry &e) {
        return entry_vals_t{e.config_paths, e.config_size};
    });
    search(QStandardPaths::GenericCacheLocation, [](MEntry &e) {
        return entry_vals_t{e.cache_paths, e.cache_size};
    });
    search(QStandardPaths::GenericDataLocation, [](MEntry &e) {
        return entry_vals_t{e.data_paths, e.data_size};
    });

    const QLatin1String desktop_ext{".desktop"};
    const QStringList icon_tmpls{
        QStringLiteral("/usr/share/icons/hicolor/86x86/apps/%1.png"),
        QStringLiteral("/usr/share/themes/sailfish-default/meegotouch/z1.0/icons/%1.png"),
        QStringLiteral("/usr/share/themes/sailfish-default/meegotouch/z1.75/icons/%1.png"),
    };
    for (auto it = m_entries.begin(), end = m_entries.end(); it != end; ++it)
    {
        const MDesktopEntry desktop{QStandardPaths::locate(QStandardPaths::ApplicationsLocation, it.key() + desktop_ext)};
        if (desktop.isValid())
        {
            it->installed = true;
            it->title = desktop.name();
            const auto icon_name = desktop.icon();
            for (const auto &tmpl : icon_tmpls)
            {
                const auto icon = tmpl.arg(icon_name);
                if (QFileInfo{icon}.isFile())
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
    const auto changed = clearEntry(e, deleted, types);
    const int row = m_names.indexOf(name);

    if (e.exists())
    {
        const auto ind = this->createIndex(row, 0);
        emit this->dataChanged(ind, ind, changed);
    }
    else
    {
        this->beginRemoveRows(QModelIndex(), row, row);
        m_names.removeOne(name);
        m_entries.remove(name);
        this->endRemoveRows();
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

        const auto changed = this->clearEntry(e, deleted, types);
        const auto &name = it.key();
        const int row = m_names.indexOf(name);

        if (e.exists())
        {
            const auto ind = this->createIndex(row, 0);
            emit this->dataChanged(ind, ind, changed);
            ++it;
        }
        else
        {
            this->beginRemoveRows(QModelIndex(), row, row);
            m_names.removeOne(name);
            it = m_entries.erase(it);
            this->endRemoveRows();
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

    const auto &name  = m_names[index.row()];
    const auto &entry = m_entries[name];
    const auto &title = entry.title.isEmpty() ? name : entry.title;
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
        { NameRole,          QByteArrayLiteral("name") },
        { TitleRole,         QByteArrayLiteral("title") },
        { IconRole,          QByteArrayLiteral("icon") },
        { InstalledRole,     QByteArrayLiteral("installed") },
        { ConfigSizeRole,    QByteArrayLiteral("configSize") },
        { CacheSizeRole,     QByteArrayLiteral("cacheSize") },
        { LocalDataSizeRole, QByteArrayLiteral("localDataSize") }
    };
}
