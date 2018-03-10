#include "mproxymodel.h"


MProxyModel::MProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{}

void MProxyModel::sort(Qt::SortOrder order)
{
    QSortFilterProxyModel::sort(0, order);
}
