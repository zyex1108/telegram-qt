/*
   Copyright (C) 2014-2015 Alexandr Akulich <akulichalexander@gmail.com>

   This file is a part of TelegramQt library.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

 */

#include "CChatInfoModel.hpp"
#include "CTelegramCore.hpp"

#include <QPixmapCache>

CChatInfoModel::CChatInfoModel(CTelegramCore *backend, QObject *parent) :
    QAbstractTableModel(parent),
    m_backend(backend)
{
    connect(m_backend, SIGNAL(peerAdded(Telegram::Peer)), this, SLOT(onPeerAdded(Telegram::Peer)));
    connect(m_backend, SIGNAL(chatChanged(quint32)), SLOT(onChatChanged(quint32)));
    connect(m_backend, SIGNAL(filePartReceived(quint32,QByteArray,QString,quint32,quint32)),
            SLOT(onFilePartReceived(quint32,QByteArray,QString,quint32,quint32)));
    connect(m_backend, SIGNAL(fileRequestFinished(quint32,Telegram::RemoteFile)),
            this, SLOT(onFileRequestFinished(quint32,Telegram::RemoteFile)));
}

QVariant CChatInfoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    switch (section) {
    case Id:
        return tr("Id");
    case Title:
        return tr("Title");
    case Picture:
        return tr("Picture");
    case ParticipantsCount:
        return tr("Participants");
    default:
        break;
    }

    return QVariant();
}

QVariant CChatInfoModel::data(const QModelIndex &index, int role) const
{
    int section = index.column();
    int chatIndex = index.row();
    if (chatIndex >= rowCount()) {
        return QVariant();
    }

    if (role == Qt::DecorationRole && index.column() == Picture) {
        return m_chats.at(chatIndex).m_picture;
    }

    if ((role != Qt::DisplayRole) && (role != Qt::EditRole)) {
        return QVariant();
    }

    switch (section) {
    case Id:
        return m_chats.at(chatIndex).id;
    case Title:
        return m_chats.at(chatIndex).title;
    case Picture:
        if (m_chats.at(chatIndex).m_picture.isNull()) {
            return m_backend->peerPictureToken(m_chats.at(chatIndex).m_peer);
        }
        break;
    case ParticipantsCount:
        return m_chats.at(chatIndex).participantsCount;
    default:
        break;
    }

    return QVariant();
}

void CChatInfoModel::addChat(quint32 id)
{
    Q_UNUSED(id);
    return;

//    if (haveChat(id)) {
//        return;
//    }

//    beginInsertRows(QModelIndex(), m_chats.count(), m_chats.count());
//    m_chats.append(Telegram::GroupChat(id));
//    m_backend->getChatInfo(&m_chats.last(), id);
//    endInsertRows();

//    emit chatAdded(id);
}

void CChatInfoModel::onPeerAdded(const Telegram::Peer &peer)
{
    switch (peer.type) {
    case Telegram::Peer::Channel:
    case Telegram::Peer::Chat:
        break;
    default:
        return;
    }

    if (!peer.isValid() || haveChat(peer.id)) {
        return;
    }

    beginInsertRows(QModelIndex(), m_chats.count(), m_chats.count());
    m_peers.append(peer);
    m_chats.append(SGroupChat(peer));
    m_backend->getChatInfo(&m_chats.last(), peer.id);
    const QString token = m_backend->peerPictureToken(peer);
    if (!token.isEmpty()) {
        QPixmap picture;
        if (QPixmapCache::find(token, &picture)) {
            m_chats.last().m_picture = picture;
        } else {
            const quint32 requestId = m_backend->requestPeerPicture(peer);
            if (requestId) {
                m_chatPictureRequests.insert(requestId, peer.id);
            }
        }
    }
    endInsertRows();

    emit chatAdded(peer.id);
}

int CChatInfoModel::indexOfChat(quint32 id) const
{
    for (int i = 0; i < m_chats.count(); ++i) {
        if (m_chats.at(i).id == id) {
            return i;
        }
    }

    return -1;
}

bool CChatInfoModel::haveChat(quint32 id) const
{
    return indexOfChat(id) >= 0;
}

const Telegram::GroupChat *CChatInfoModel::chatById(quint32 id) const
{
    int index = indexOfChat(id);

    if (index < 0) {
        return 0;
    }

    return &m_chats.at(index);
}

Telegram::Peer CChatInfoModel::getPeer(quint32 chatId)
{
    int index = indexOfChat(chatId);

    if (index < 0) {
        return Telegram::Peer();
    }

    return m_peers.at(index);
}

void CChatInfoModel::onChatChanged(quint32 id)
{
    int i = indexOfChat(id);

    if (i < 0) {
        return;
    }

    m_backend->getChatInfo(&m_chats[i], id);
    emit dataChanged(index(i, 0), index(i, ColumnsCount - 1));
    emit chatChanged(id);
}

void CChatInfoModel::onFilePartReceived(quint32 requestId, const QByteArray &data, const QString &mimeType, quint32 offset, quint32 totalSize)
{
    if (!m_chatPictureRequests.contains(requestId)) {
        return;
    }

    const int index = indexOfChat(m_chatPictureRequests.value(requestId));
    if (index < 0) {
        return;
    }
    SGroupChat &chat = m_chats[index];
    chat.m_pictureData += data;
}

void CChatInfoModel::onFileRequestFinished(quint32 requestId, Telegram::RemoteFile requestResult)
{
    Q_UNUSED(requestResult)

    if (!m_chatPictureRequests.contains(requestId)) {
        return;
    }

    const int i = indexOfChat(m_chatPictureRequests.value(requestId));
    if (i < 0) {
        return;
    }
    SGroupChat &chat = m_chats[i];

#ifdef CREATE_MEDIA_FILES
    QDir dir;
    dir.mkdir("peerPictures");

    QFile pictureFile(QString("peerPictures/%1.jpg").arg(peer.id));
    pictureFile.open(QIODevice::WriteOnly);
    pictureFile.write(data);
    pictureFile.close();
#endif

    chat.m_picture = QPixmap::fromImage(QImage::fromData(chat.m_pictureData));

    if (chat.m_picture.isNull()) {
        return;
    }

    chat.m_picture = chat.m_picture.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    const QString token = m_backend->peerPictureToken(chat.m_peer);
    QPixmapCache::insert(token, chat.m_picture);

    emit dataChanged(index(i, 0), index(i, ColumnsCount - 1));
    emit chatChanged(chat.id);
}
