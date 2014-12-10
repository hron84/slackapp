#ifndef SLACKAPP_FILEDOWNLOADER_H
#define SLACKAPP_FILEDOWNLOADER_H 1

#include <QtCore/qobjectdefs.h>
#include <QNetworkReply>
#include <QFile>

class FileDownloader : public QObject {
    Q_OBJECT;

public:
    FileDownloader(QNetworkReply *reply, QString filePath) : m_reply(reply), aborted(false) {
        m_file = new QFile(filePath);
        m_file->open(QIODevice::WriteOnly);
        outStream = new QDataStream(m_file);

        connect(m_reply, SIGNAL(readyRead()), SLOT(doDownload()));
        connect(m_reply, SIGNAL(finished()), SLOT(finishDownload()));
        connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError )), SLOT(abortDownload(QNetworkReply::NetworkError )));
    }

    bool isAborted() { return aborted; }

    static FileDownloader *downloadFile(QNetworkReply *reply, QString filePath) {
        return new FileDownloader(reply, filePath);
    }
private:
    QNetworkReply *m_reply;
    QFile *m_file;
    QDataStream *outStream;
    bool aborted;
    void setAborted(bool state) { aborted = state; }
signals:
    void downloadFinished(QString filePath);
    void downloadFailed(QString filePath, QNetworkReply::NetworkError code);
private slots:

    void doDownload();
    void finishDownload();
    void abortDownload(QNetworkReply::NetworkError error);

};

#endif