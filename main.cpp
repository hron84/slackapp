#include <QtGui>
#include <QtNetwork/QNetworkProxyFactory>
#include "slackapp.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QNetworkProxyFactory::setUseSystemConfiguration(true);

    app.setWindowIcon(QIcon(":/favicon.png"));

    SlackApp slackApp;
    slackApp.show();
    return app.exec();
}