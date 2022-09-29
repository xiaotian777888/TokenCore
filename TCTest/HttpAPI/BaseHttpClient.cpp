#include "BaseHttpClient.h"

#include <QTimer>

BaseHttpClient::BaseHttpClient()
{
    QObject::connect(&networkAccessManager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    //QObject::connect(&networkAccessManager, SIGNAL(sslErrors(QNetworkReply *reply, const QList<QSslError> &errors)), this, SLOT(onSSLErrors(QNetworkReply *reply, const QList<QSslError> &errors)));
}

BaseHttpClient::~BaseHttpClient()
{
    networkAccessManager.disconnect();
}

static const QByteArray emptyByteArray("");

const QByteArray BaseHttpClient::get(const QString& url, const int timeout)
{
	QTimer timer;
	timer.setInterval(timeout);
	timer.setSingleShot(true);

    QNetworkRequest httpReques = QNetworkRequest(QUrl(url));
    httpReques.setRawHeader("Accept-Language", language.toLatin1());

    QNetworkReply* reply = networkAccessManager.get(httpReques);

	timer.start();

    connect(&timer, SIGNAL(timeout()), &eventLoop, SLOT(quit()));

	eventLoop.exec(QEventLoop::ExcludeUserInputEvents);

	if (timer.isActive()) {
		timer.stop();
		reply->deleteLater();
        return reply->readAll();
	} else {
		disconnect(&networkAccessManager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
		reply->abort();
		reply->deleteLater();
		return emptyByteArray;
	}
}

const QByteArray BaseHttpClient::post(const QString& url,const QByteArray &data, const int timeout)
{
	QTimer timer;
	timer.setInterval(timeout);
	timer.setSingleShot(true);

    QNetworkRequest httpReques = QNetworkRequest(QUrl(url));
    httpReques.setRawHeader("Content-Type", "application/json");
    httpReques.setRawHeader("Accept-Language", language.toLatin1());
    
    QNetworkReply* reply = networkAccessManager.post(httpReques, data);

	timer.start();

    connect(&timer, SIGNAL(timeout()), &eventLoop, SLOT(quit()));

	eventLoop.exec(QEventLoop::ExcludeUserInputEvents);

	if (timer.isActive()) {
		timer.stop();
		reply->deleteLater();
        return reply->readAll();
	} else {
		disconnect(&networkAccessManager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
		reply->abort();
		reply->deleteLater();
		return emptyByteArray;
	}
}

void BaseHttpClient::onSSLErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    reply->ignoreSslErrors();
}
