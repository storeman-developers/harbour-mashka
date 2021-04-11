#include "mproxymodel.h"
#include "mmodel.h"


MProxyModel::MProxyModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{}

void MProxyModel::setProcessConfig(bool value)
{
    if(m_process_config != value)
    {
        m_process_config = value;
        emit this->processConfigChanged();
        this->invalidateFilter();
    }
}

bool MProxyModel::filterAcceptsRow(int source_row, [[maybe_unused]] const QModelIndex &source_parent) const
{
    auto model       = static_cast<MModel *>(this->sourceModel());
    const auto &name = model->m_names[source_row];
    const auto &e    = model->m_entries[name];
    const auto regex = this->filterRegExp();

    if (name.contains(regex) || e.title.contains(regex))
    {
        if (m_process_config)
        {
            return true;
        }
        return e.cache_size + e.data_size > 0;
    }

    return false;
}
