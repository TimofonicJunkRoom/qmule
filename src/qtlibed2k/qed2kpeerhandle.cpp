#include <QString>

#include "qed2kpeerhandle.h"
#include "qed2ksession.h"
#include "libed2k/error_code.hpp"
#include "transport/session.h"

#define PEER_ACTION(action, data) \
    try                                                                         \
    {                                                                           \
        m_delegate.action(data);                                                \
    }                                                                           \
    catch(libed2k::libed2k_exception& e)                                        \
    {                                                                           \
        try                                                                     \
        {                                                                       \
            libed2k::net_identifier np = m_delegate.get_network_point();        \
            m_delegate = Session::instance()->get_ed2k_session()->getPeer(np);  \
            m_delegate.action(data);                                            \
        }                                                                       \
        catch(...)                                                              \
        {                                                                       \
        }                                                                       \
    }



QED2KPeerHandle::QED2KPeerHandle(const libed2k::peer_connection_handle& pch) : m_delegate(pch)
{
}

QED2KPeerHandle::~QED2KPeerHandle()
{
}

QED2KPeerHandle QED2KPeerHandle::getPeerHandle(const libed2k::net_identifier& np)
{
    return Session::instance()->get_ed2k_session()->getPeer(np);
}

QED2KPeerHandle QED2KPeerHandle::findPeerHandle(const libed2k::net_identifier& np)
{
    return Session::instance()->get_ed2k_session()->findPeer(np);
}

void QED2KPeerHandle::sendMessageToPeer(const QString& strMessage)
{
    PEER_ACTION(send_message, strMessage.toUtf8().constData())
}

bool QED2KPeerHandle::isAllowedSharedFilesView()
{
    return (m_delegate.get_misc_options().m_nNoViewSharedFiles == 0);
}

void QED2KPeerHandle::requestDirs()
{
    PEER_ACTION(get_shared_directories, );
}

void QED2KPeerHandle::requestFiles(QString dirName)
{
    PEER_ACTION(get_shared_directory_files, dirName.toUtf8().constData());
}

void QED2KPeerHandle::requestDirFiles(QString dirHash)
{
    PEER_ACTION(get_ismod_directory, libed2k::md4_hash::fromString(dirHash.toStdString()));
}

QString QED2KPeerHandle::getUserName()
{
    return QString::fromUtf8(m_delegate.get_options().m_strName.c_str());
}

libed2k::md4_hash QED2KPeerHandle::getHash()
{
    libed2k::md4_hash hash;
    try
    {
        hash = m_delegate.get_hash();
    }
    catch(libed2k::libed2k_exception& e) 
    {
    }
    return hash;
}

libed2k::peer_connection_options QED2KPeerHandle::getConnectionOptions()
{
    libed2k::peer_connection_options options;
    try
    {
        options = m_delegate.get_options();
    }
    catch(libed2k::libed2k_exception& e) 
    {
    }
    return options;
}