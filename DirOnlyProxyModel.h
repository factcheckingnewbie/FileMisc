#pragma once

#include <KDirSortFilterProxyModel>

class DirOnlyProxyModel : public KDirSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit DirOnlyProxyModel(QObject *parent = nullptr);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
};
