
#ifndef __SESSION_H__
#define __SESSION_H__

#include <QScopedPointer>

#include "transport/transfer.h"
#include "qtlibtorrent/qbtsession.h"
#include "qtlibed2k/qed2ksession.h"
#include "torrentspeedmonitor.h"

typedef libtorrent::session_status SessionStatus;

/**
 * Generic data transfer session
 */
class Session: public QObject, public SessionBase
{
    Q_OBJECT
    Q_DISABLE_COPY(Session)

public:
    static Session* instance();
    static void drop();

    virtual ~Session();

    QBtSession* get_torrent_session();
    QED2KSession* get_ed2k_session();

    Transfer getTransfer(const QString& hash) const;
    std::vector<Transfer> getTransfers() const;
    qlonglong getETA(const QString& hash) const;
    qreal getGlobalMaxRatio() const;
    qreal getMaxRatioPerTransfer(const QString& hash, bool* use_global) const;
    bool isFilePreviewPossible(const QString& hash) const;
    QStringList getConsoleMessages() const;
    QStringList getPeerBanMessages() const;
    SessionStatus getSessionStatus() const;
    void changeLabelInSavePath(const Transfer& t, const QString& old_label, const QString& new_label);
    QTorrentHandle addTorrent(const QString& path, bool fromScanDir = false,
                              QString from_url = QString(), bool resumed = false);
    QTorrentHandle addMagnetUri(const QString& url, bool resumed=false);
    QED2KHandle addTransfer(const libed2k::add_transfer_params& params);
    void downloadFromUrl(const QString& url);
    void processDownloadedFile(const QString& url, const QString& path);
    void pauseTransfer(const QString& hash);
    void resumeTransfer(const QString& hash);
    void deleteTransfer(const QString& hash, bool delete_files);
    void recheckTransfer(const QString& hash);
    void setDownloadLimit(const QString& hash, long limit);
    void setUploadLimit(const QString& hash, long limit);
    void setMaxRatioPerTransfer(const QString& hash, qreal ratio);
    void removeRatioPerTransfer(const QString& hash);
    void useAlternativeSpeedsLimit(bool alternative);
    void addConsoleMessage(const QString& msg, QColor color=
                           QApplication::palette().color(QPalette::WindowText));
    void banIP(QString ip);
    QHash<QString, TrackerInfos> getTrackersInfo(const QString &hash) const;
    void setDownloadRateLimit(long rate);
    void setUploadRateLimit(long rate);
    bool hasActiveTransfers() const;

    bool useTemporaryFolder() const;
    bool isDHTEnabled() const;
    bool isPexEnabled() const;
    bool isLSDEnabled() const;
    bool isQueueingEnabled() const;
    bool isListening() const;

public slots:
	void startUpTransfers();
	void configureSession();
	void enableIPFilter(const QString &filter_path, bool force=false);

signals:
    void addedTransfer(Transfer t);
    void deletedTransfer(QString hash);
    void pausedTransfer(Transfer t);
    void resumedTransfer(Transfer t);
    void finishedTransfer(Transfer t);
    void metadataReceived(Transfer t);
    void fullDiskError(Transfer t, QString msg);
    void transferAboutToBeRemoved(Transfer t);
    void transferFinishedChecking(Transfer t);
    void trackerAuthenticationRequired(Transfer t);
    void newDownloadedTransfer(QString path, QString url);
    void downloadFromUrlFailure(QString url, QString reason);
    void alternativeSpeedsModeChanged(bool alternative);
    void recursiveDownloadPossible(QTorrentHandle t);
    void savePathChanged(Transfer t);
    void newConsoleMessage(QString msg);
    void newBanMessage(QString msg);

private slots:
    void on_addedTorrent(const QTorrentHandle& h);
    void on_pausedTorrent(const QTorrentHandle& h);
    void on_resumedTorrent(const QTorrentHandle& h);
    void on_finishedTorrent(const QTorrentHandle& h);
    void on_metadataReceived(const QTorrentHandle& h);
    void on_fullDiskError(const QTorrentHandle& h, QString msg);
    void on_torrentAboutToBeRemoved(const QTorrentHandle& h);
    void on_torrentFinishedChecking(const QTorrentHandle& h);
    void on_trackerAuthenticationRequired(const QTorrentHandle& h);
    void on_savePathChanged(const QTorrentHandle& h);
    void saveTempFastResumeData();
    void readAlerts();

private:
    Session();
    SessionBase* delegate(const QString& hash) const;
    SessionBase* delegate(const Transfer& t) const;
    std::vector<SessionBase*> delegates() const;

    static Session* m_instance;

    QBtSession m_btSession;
    QED2KSession m_edSession;

    QScopedPointer<TorrentSpeedMonitor> m_speedMonitor;
    QScopedPointer<QTimer>  m_periodic_resume;
    QScopedPointer<QTimer>  m_alerts_reading;
};

#endif
