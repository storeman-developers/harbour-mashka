#ifndef VMODEL_H
#define VMODEL_H

#include <QAbstractListModel>

#include "mentry.h"


class MModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(int totalApps READ rowCount NOTIFY totalChanged)
    Q_PROPERTY(qint64 totalDataSize READ totalDataSize NOTIFY totalChanged)
    Q_PROPERTY(int totalUnused READ totalUnused NOTIFY totalChanged)
    Q_PROPERTY(qint64 totalUnusedSize READ totalUnusedSize NOTIFY totalChanged)

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
        SortRole
    };
    Q_ENUM(Role)

    enum DataType
    {
        UnknownDataType,
        ConfigData,
        CacheData,
        LocalData,
        AllData
    };
    Q_ENUM(DataType)

    explicit MModel(QObject *parent = nullptr);

    bool busy() const;
    qint64 totalDataSize() const;

    int totalUnused() const;
    qint64 totalUnusedSize() const;

signals:
    void busyChanged();
    void totalChanged();
    void cleared(const qint64 &size);
    void error(const QString &path);

public slots:
    void reset();
    void clearData(const QString &name, const DataType &type);
    void clearUnusedData();

private:
    bool m_busy;
    int m_unused;
    qint64 m_total_size;
    qint64 m_unused_size;
    QStringList m_names;
    QMap<QString, MEntry> m_entries;

    void resetImpl();
    void clearDataImpl(const QString &name, const DataType &type);
    void clearUnusedDataImpl();
    void calculateTotal();

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;
};

#endif // VMODEL_H
