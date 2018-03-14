#include "mproxymodel.h"
#include "mmodel.h"


MProxyModel::MProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_process_config(false)
{}

bool MProxyModel::processConfig() const
{
    return m_process_config;
}

void MProxyModel::setProcessConfig(bool value)
{
    if(m_process_config != value)
    {
        m_process_config = value;
        emit this->processConfigChanged();
        this->invalidateFilter();
    }
}

void MProxyModel::sort(Qt::SortOrder order)
{
    QSortFilterProxyModel::sort(0, order);
}

bool MProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent);

    if (m_process_config)
    {
        return true;
    }
    auto model = static_cast<MModel *>(this->sourceModel());
    auto &e = model->m_entries[model->m_names[source_row]];
    return e.cache_size + e.data_size > 0;
}
