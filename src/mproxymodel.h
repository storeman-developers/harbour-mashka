#ifndef PROXYMODEL_H
#define PROXYMODEL_H

#include <QSortFilterProxyModel>


class MProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit MProxyModel(QObject *parent = nullptr);

public:
    Q_INVOKABLE void sort(Qt::SortOrder order = Qt::AscendingOrder);
};

#endif // PROXYMODEL_H
