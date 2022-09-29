#pragma once

#include "global.h"

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <QEventLoop>

class BaseHttpClient : public QObject
{
    Q_OBJECT

public:
	BaseHttpClient();
	~BaseHttpClient();
    const QByteArray get(const QString& url, const int timeout);
    const QByteArray post(const QString& url, const QByteArray &data, const int timeout);

public slots:
    void onSSLErrors(QNetworkReply *reply, const QList<QSslError> &errors);

private:
    QNetworkAccessManager networkAccessManager;
    QEventLoop eventLoop;
};
