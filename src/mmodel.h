#ifndef VMODEL_H
#define VMODEL_H

#include <QAbstractListModel>

#include "mentry.h"


class MModel : public QAbstractListModel
{
    friend class MProxyModel;

    Q_OBJECT
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(bool resetting READ resetting NOTIFY resettingChanged)
    Q_PROPERTY(int totalAppsCount READ rowCount NOTIFY totalChanged)
    Q_PROPERTY(qint64 totalConfigSize READ totalConfigSize NOTIFY totalChanged)
    Q_PROPERTY(qint64 totalCacheSize READ totalCacheSize NOTIFY totalChanged)
    Q_PROPERTY(qint64 totalLocaldataSize READ totalLocaldataSize NOTIFY totalChanged)
    Q_PROPERTY(int unusedAppsCount READ unusedAppsCount NOTIFY totalChanged)
    Q_PROPERTY(qint64 unusedConfigSize READ unusedConfigSize NOTIFY totalChanged)
    Q_PROPERTY(qint64 unusedCacheSize READ unusedCacheSize NOTIFY totalChanged)
    Q_PROPERTY(qint64 unusedLocaldataSize READ unusedLocaldataSize NOTIFY totalChanged)

public:
    enum Role
    {
        NameRole = Qt::UserRole,
        TitleRole,
        IconRole,
        InstalledRole,
        ConfigSizeRole,
        CacheSizeRole,
        LocalDataSizeRole,
        SortRole,
    };
    Q_ENUM(Role)

    enum DataType
    {
        ConfigData = 0x001,
        CacheData  = 0x002,
        LocalData  = 0x004,
        AllData    = ConfigData | CacheData | LocalData
    };
    Q_ENUM(DataType)
    Q_DECLARE_FLAGS(DataTypes, DataType)
    Q_FLAGS(DataTypes)

    explicit MModel(QObject *parent = nullptr);

    bool busy() const;
    bool resetting() const;

    qint64 totalConfigSize() const;
    qint64 totalCacheSize() const;
    qint64 totalLocaldataSize() const;

    int unusedAppsCount() const;
    qint64 unusedConfigSize() const;
    qint64 unusedCacheSize() const;
    qint64 unusedLocaldataSize() const;

signals:
    void busyChanged();
    void resettingChanged();
    void totalChanged();
    void dataDeleted(const qint64 &size);
    void deletionError(const QString &path);

public slots:
    void reset();
    void deleteData(const QString &name, DataTypes types);
    void deleteUnusedData(DataTypes types);

private:
    bool m_busy;
    bool m_resetting;
    int m_unused_apps_count;
    qint64 m_total_cache_size;
    qint64 m_total_config_size;
    qint64 m_total_localdata_size;
    qint64 m_unused_config_size;
    qint64 m_unused_cache_size;
    qint64 m_unused_localdata_size;
    QStringList m_names;
    QMap<QString, MEntry> m_entries;

    void setBusy(bool busy);
    qint64 removePaths(const QStringList &paths);
    QVector<int> clearEntry(MEntry &entry, qint64 &deleted, DataTypes types);
    void resetImpl();
    void deleteDataImpl(const QString &name, DataTypes types);
    void deleteUnusedDataImpl(DataTypes types);
    void calculateTotal();

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(MModel::DataTypes)

#endif // VMODEL_H
