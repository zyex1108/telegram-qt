#include "CDialogModel.hpp"

#include "CTelegramCore.hpp"

CDialogModel::CDialogModel(CTelegramCore *backend, QObject *parent) :
    QAbstractTableModel(parent),
    m_backend(backend)
{
}

int CDialogModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return ColumnsCount;
}

int CDialogModel::rowCount(const QModelIndex &parent) const
{
    return 0;
}

QVariant CDialogModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Section s = static_cast<Section>(section);

    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    switch (section) {
    case PeerType:
        return tr("Type");
    case PeerId:
        return tr("Id");
    case PeerName:
        return tr("Name");
    case Picture:
        return tr("Picture");
    default:
        break;
    }

    return QVariant();
}

QVariant CDialogModel::data(const QModelIndex &index, int role) const
{
    return QVariant();
}
