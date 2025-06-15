#include "DirOnlyProxyModel.h"
#include <KDirModel>

DirOnlyProxyModel::DirOnlyProxyModel(QObject *parent)
    : KDirSortFilterProxyModel(parent)
{
}

bool DirOnlyProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    if (!index.isValid())
        return false;
    // Only accept directories
    QVariant isDirData = index.data(KDirModel::IsDirRole);
    return isDirData.isValid() && isDirData.toBool();
}
