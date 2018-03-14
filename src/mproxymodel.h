#ifndef PROXYMODEL_H
#define PROXYMODEL_H

#include <QSortFilterProxyModel>


class MProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(bool processConfig READ processConfig WRITE setProcessConfig NOTIFY processConfigChanged)

public:
    explicit MProxyModel(QObject *parent = nullptr);

    bool processConfig() const;
    void setProcessConfig(bool value);

    Q_INVOKABLE void sort(Qt::SortOrder order = Qt::AscendingOrder);

signals:
    void processConfigChanged();

private:
    bool m_process_config;

    // QSortFilterProxyModel interface
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
};

#endif // PROXYMODEL_H
