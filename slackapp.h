#include <QWidget>
#include <QtWebKit/QWebElement>
#include <QNetworkReply>
#include "ui_form.h"

class FileDownloader;

class SlackApp : public QWidget {
Q_OBJECT

public:
    SlackApp();

protected:
    void showStatus(const QString &msg);
    void showError(const QString &msg);

    QWebElement document() const ;

private slots:

    void adjustLoginButton();
    void inputPassword();
    void doLogin();
    void initialPage(bool ok);
    void loginPage(bool ok);
    void hideElements();
    void setAppWindowTitle(const QString &title);
    void prepareToDownload(const QNetworkRequest &request);
//    void downloadContent(QNetworkReply *reply);
    void downloadContent();
//    void downloadFinished(const QString &filePath);
//    void downloadAborted(const QString &filepath, const QNetworkReply::NetworkError code);

private:
    Ui::Form form;
    QString userName;
    QString password;
    QList<FileDownloader *> downloads;
};

