#pragma once

#include <QSortFilterProxyModel>

class MProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(bool processConfig READ processConfig WRITE setProcessConfig NOTIFY processConfigChanged)

public:
    explicit MProxyModel(QObject *parent = nullptr);

    bool processConfig() const {
        return m_process_config;
    }

    void setProcessConfig(bool value);

    Q_INVOKABLE void sort(int column = 0, Qt::SortOrder order = Qt::AscendingOrder) override {
        QSortFilterProxyModel::sort(column, order);
    }

signals:
    void processConfigChanged();

private:
    bool m_process_config{false};

    // QSortFilterProxyModel interface
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
};
