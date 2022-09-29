#pragma once

#include "global.h"

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

class AsyncHttpClient : public QObject
{
	Q_OBJECT

public:
	AsyncHttpClient();
	~AsyncHttpClient();

    void get(const QString& url);
    void post(const QString& url, const QByteArray &data);

protected:
	virtual void requestFinished(QNetworkReply *reply, const QByteArray data, const int statusCode) = 0;

public slots:
	void serviceRequestFinished(QNetworkReply *reply);
    void onSSLErrors(QNetworkReply *reply, const QList<QSslError> &errors);
private:
	QNetworkAccessManager networkAccessManager;
};
