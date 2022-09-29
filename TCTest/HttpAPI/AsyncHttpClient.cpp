#include "AsyncHttpClient.h"

#include <QtNetwork/QNetworkRequest>


AsyncHttpClient::AsyncHttpClient()
{
	QObject::connect(&networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(serviceRequestFinished(QNetworkReply*)));
    //QObject::connect(&networkAccessManager, SIGNAL(sslErrors(QNetworkReply *reply, const QList<QSslError> &errors)), this, SLOT(onSSLErrors(QNetworkReply *reply, const QList<QSslError> &errors)));
}

AsyncHttpClient::~AsyncHttpClient()
{
	networkAccessManager.disconnect();
}

void AsyncHttpClient::get(const QString& url)
{
    QNetworkRequest httpReques = QNetworkRequest(QUrl(url));
    httpReques.setRawHeader("Accept-Language", language.toLatin1());

    networkAccessManager.get(httpReques);
}

void AsyncHttpClient::post(const QString& url, const QByteArray &data)
{
	QNetworkRequest httpReques = QNetworkRequest(QUrl(url));
    httpReques.setRawHeader("Accept-Language", language.toLatin1());
	httpReques.setRawHeader("Content-Type", "application/json");
    
	networkAccessManager.post(httpReques, data);
}


void AsyncHttpClient::serviceRequestFinished(QNetworkReply *reply)
{
	int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (reply->error() == QNetworkReply::NoError) {
		requestFinished(reply, reply->readAll(), statusCode);
    } else {
		requestFinished(reply, "", statusCode);
	}

    reply->deleteLater();
}

void AsyncHttpClient::onSSLErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    reply->ignoreSslErrors();
}
