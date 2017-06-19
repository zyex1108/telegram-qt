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
    return static_cast<int>(Column::ColumnsCount);
}

int CDialogModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_dialogs.count();
}

QVariant CDialogModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Column s = static_cast<Column>(section);

    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    switch (s) {
    case Column::PeerType:
        return tr("Type");
    case Column::PeerId:
        return tr("Id");
    case Column::PeerName:
        return tr("Name");
    case Column::Picture:
        return tr("Picture");
    default:
        break;
    }

    return QVariant();
}

QVariant CDialogModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        break;
    default:
        return QVariant();
    }

    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= m_dialogs.count()) {
        return QVariant();
    }

    const Column ownColumn = static_cast<Column>(index.column());
    const Role ownRole = columnToRole(ownColumn);
    if (ownRole == Role::Invalid) {
        return QVariant();
    }

    return data(index.row(), ownRole);
}

QVariant CDialogModel::data(int dialogIndex, CDialogModel::Role role) const
{
    const Telegram::DialogInfo *dialog = m_dialogs.at(dialogIndex);

    switch (role) {
    case Role::PeerType:
        return dialog->peer().type;
    case Role::PeerId:
        return dialog->peer().id;
    case Role::PeerName:
        return dialog->peerName();
    case Role::Picture:
//        return dialog;
    case Role::Invalid:
        return QVariant();
    }
    return QVariant();
}

void CDialogModel::setDialogs(const QVector<Telegram::Peer> &dialogs)
{
    beginResetModel();
    qDeleteAll(m_dialogs);
    m_dialogs.clear();
    for (const Telegram::Peer &peer : dialogs) {
        Telegram::DialogInfo *info = new Telegram::DialogInfo();
        m_backend->getDialogInfo(info, peer);
    }
    endResetModel();
}

CDialogModel::Role CDialogModel::columnToRole(Column column)
{
    switch (column) {
    case Column::PeerType:
        return Role::PeerType;
    case Column::PeerId:
        return Role::PeerId;
    case Column::PeerName:
        return Role::PeerName;
    case Column::Picture:
        return Role::Picture;
    default:
        return Role::Invalid;
    }
}
