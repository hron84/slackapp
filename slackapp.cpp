#include <QtGui/QtGui>
#include <QtWebKit/QtWebKit>
#include <QtNetwork/QSslSocket>
#include <QtNetwork/qnetworkreply.h>

#include "filedownloader.h"
#include "slackapp.h"

#define SLACK_URL "https://company.slack.com/messages/"
#define EMAIL_SUFFIX "@company.hu"

SlackApp::SlackApp() : QWidget() {

    form.setupUi(this);

    form.userNameEdit->setFocus();
    connect(form.userNameEdit, SIGNAL(textChanged(QString)), SLOT(adjustLoginButton()));
    connect(form.userNameEdit, SIGNAL(returnPressed()), SLOT(inputPassword()));

    connect(form.passwordEdit, SIGNAL(textChanged(QString)), SLOT(adjustLoginButton()));
    connect(form.passwordEdit, SIGNAL(returnPressed()), SLOT(doLogin()));

    form.loginButton->setEnabled(false);
    connect(form.loginButton, SIGNAL(clicked()), SLOT(doLogin()));

    connect(form.webView, SIGNAL(loadFinished(bool)), SLOT(initialPage(bool)));
    connect(form.webView, SIGNAL(loadProgress(int)),
            form.progressBar, SLOT(setValue(int)));
    connect(form.webView, SIGNAL(titleChanged(const QString &)), SLOT(setAppWindowTitle(const QString &)));

    form.webView->setUrl((QUrl(SLACK_URL)));
    form.webView->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    form.webView->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
//    form.webView->page()->setForwardUnsupportedContent(true);
//    connect(form.webView->page(), SIGNAL(unsupportedContent(QNetworkReply*)),
//            this, SLOT(downloadContent(QNetworkReply *)),
//            Qt::QueuedConnection); // TODO review this is already fixed or not: QTBUG-18718

    connect(form.webView->page(), SIGNAL(downloadRequested(QNetworkRequest)), SLOT(prepareToDownload(QNetworkRequest)));
    //form.webView->setContextMenuPolicy(Qt::PreventContextMenu);

}

void SlackApp::showStatus(const QString &msg) {
    form.statusLabel->setText(msg);
    form.stackedWidget->setCurrentIndex(0);
}

void SlackApp::showError(const QString &msg) {
    form.progressBar->hide();
    showStatus(QString("Error: %1").arg(msg));
}

QWebElement SlackApp::document() const {
    return form.webView->page()->mainFrame()->documentElement();
}

void SlackApp::adjustLoginButton() {
    userName = form.userNameEdit->text();
    password = form.passwordEdit->text();
    bool ok = !userName.isEmpty() && !password.isEmpty();
    form.loginButton->setEnabled(ok);
}

void SlackApp::inputPassword() {
    if (!form.userNameEdit->text().isEmpty())
        form.passwordEdit->setFocus();
}

void SlackApp::doLogin() {
    userName = form.userNameEdit->text();
    password = form.passwordEdit->text();
    bool ok = !userName.isEmpty() && !password.isEmpty();
    if (!ok)
        return;

    form.progressBar->setValue(0);
    form.progressBar->show();
    connect(form.webView, SIGNAL(loadFinished(bool)), SLOT(loginPage(bool)));
    connect(form.webView, SIGNAL(loadProgress(int)),
            form.progressBar, SLOT(setValue(int)));
    showStatus("Logging in...");

    QString userEmail = userName + EMAIL_SUFFIX;

    document().findFirst("#email").setAttribute("value", userEmail);
    document().findFirst("#password").setAttribute("value", password);
    document().findFirst("#signin_form").evaluateJavaScript("this.submit();");
}

void SlackApp::initialPage(bool ok) {
    if (!QSslSocket::supportsSsl()) {
        showError("This example requires SSL support.");
        return;
    }

    if (ok) {
        QWebElement email = document().findFirst("#email");
        QWebElement passwd = document().findFirst("#password");
        QWebElement loginForm = document().findFirst("#signin_form");
        if (!email.isNull() && !passwd.isNull() && !loginForm.isNull()) {
            form.stackedWidget->setCurrentIndex(1);
            form.userNameEdit->setFocus();
            form.webView->disconnect();
            return;
        }
    }

    showError("SERVICE unavailable.");
}

void SlackApp::hideElements()
{
    document().findFirst(".footer-footer").removeFromDocument();
    document().findFirst(".title-bar-bg .title-bar").removeFromDocument();
    QTimer::singleShot(2000, this, SLOT(hideElements()));
}

void SlackApp::loginPage(bool ok) {
    QString location = form.webView->url().toString();
    if (!ok) {
        if (location.indexOf("CheckCookie"))
            return;
        showError("Service unavailable");
    } else {
        // check for any error message

        QWebElement  e = document().findFirst(".alert_error");
        if (e.isNull()) {
            form.stackedWidget->setCurrentIndex(2);
            QTimer::singleShot(500, this, SLOT(hideElements()));
            return;
        }

        QString err = "Unknown login failure.";
        const QString errorMessage = e.toPlainText();
        if (!errorMessage.isEmpty()) {
            err = errorMessage;
            err = err.simplified();
        }
        showError(err);
    }
}

void SlackApp::setAppWindowTitle(const QString &title) {
    this->setWindowTitle(title + " - SlackApp");
}

void SlackApp::prepareToDownload(const QNetworkRequest &request) {
    qDebug() << "Download file selected";
    QString defaultFileName =
            QFileInfo(request.url().toString()).fileName();
    QString filePath =
            QFileDialog::getSaveFileName(this,
                    tr("Save File"),
                    defaultFileName);

    if(filePath.isEmpty()) return;

    QNetworkRequest req = request;
    req.setAttribute(QNetworkRequest::User, filePath);
    QNetworkReply *reply = form.webView->page()->networkAccessManager()->get(req);

    connect(reply, SIGNAL(finished()), SLOT(downloadContent()));

}

//void SlackApp::downloadContent(QNetworkReply *reply) {
void SlackApp::downloadContent() {

//    FileDownloader *downloader = FileDownloader::downloadFile(reply, filePath);
//    downloads << downloader;
//
//    connect(downloader, SIGNAL(downloadFinished( QString )), SLOT(downloadFinished(QString &)));
//    connect(downloader, SIGNAL(downloadAborted( QString , QNetworkReply::NetworkError)), SLOT(downloadFinished(QString &, QNetworkReply::NetworkError)));

    QNetworkReply *reply = ((QNetworkReply*)sender());
    QNetworkRequest request = reply->request();
    QString filePath = request.attribute(QNetworkRequest::User).toString();
    QFile file(filePath);
    if(file.open(QFile::WriteOnly)) {
        file.write(reply->readAll());
    }

    QMessageBox::information(this, "Download finished", filePath + " is downloaded", QMessageBox::Ok);
}

//void SlackApp::downloadFinished(const QString &filePath) {
//    QMessageBox::information(this, "Download finished", filePath + " is downloaded", QMessageBox::Ok);
//}
//
//void SlackApp::downloadAborted(const QString &filePath, const QNetworkReply::NetworkError code) {
//    QMessageBox::critical(this, "Download finished", filePath + " failed to download", QMessageBox::Ok);
//}
