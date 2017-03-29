#ifndef CDIALOGMODEL_HPP
#define CDIALOGMODEL_HPP

#include <QAbstractTableModel>

#include "TelegramNamespace.hpp"

class CTelegramCore;

class CDialogModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Section {
        PeerType,
        PeerId,
        PeerName,
        Picture, // Photo (in terms of Telegram)
        ColumnsCount
    };

    explicit CDialogModel(CTelegramCore *backend, QObject *parent = nullptr);

    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
//    QVariant data(const QString &phone, Section section) const;
//    QVariant data(quint32 id, Section section) const;
//    QVariant data(int contactIndex, Section section) const;

private:
    CTelegramCore *m_backend;

};

#endif // CDIALOGMODEL_HPP
