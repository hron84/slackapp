#include <QtCore>
#include <QNetworkReply>

#include "filedownloader.h"

void FileDownloader::doDownload() {
    QByteArray buf = m_reply->read(m_reply->readBufferSize());
    if(buf.size() > 0) {

        qDebug() << "Processing " << buf.size() << " bytes";
        (*outStream) << buf.data();
    }
}

void FileDownloader::abortDownload(QNetworkReply::NetworkError error) {
    m_file->close();

    if(error != QNetworkReply::NoError) {
        m_file->remove();
        setAborted(true);
        qDebug() << "Download aborted due error code " << error;

        emit downloadFailed(m_file->fileName(), error);
    }
}

void FileDownloader::finishDownload() {
    if(isAborted()) return;

    m_file->close();
    qDebug() << "Download finished";

    emit downloadFinished(m_file->fileName());
}