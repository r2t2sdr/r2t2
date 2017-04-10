#include <QDebug>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest> 
#include "r2t2qtradiolistener.h"
#include "lib.h"

R2T2QtRadioListener::R2T2QtRadioListener(QSettings *settings, QString addr, quint16 port, quint16 listenPort, uint32_t sampleRate): 
    settings(settings), addr(addr), port(port), qtRadioPort(listenPort), sampleRate(sampleRate) 
{
	networkManager = new QNetworkAccessManager(this);
	connect(networkManager, SIGNAL(finished(QNetworkReply*)), SLOT(postAnswer(QNetworkReply*)));

	updatePublic();

    tcpServer = new QTcpServer(this);
    if (!tcpServer->listen(QHostAddress::Any, qtRadioPort)) {
        qDebug() << "TCP-Server error" << tcpServer->errorString();
        return;
    }

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(updatePublic()));
	timer->start(240*1000);

    qDebug() << "listening on port " << qtRadioPort;
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

R2T2QtRadioListener::~R2T2QtRadioListener() {
    delete tcpServer;
    delete networkManager;
}

void R2T2QtRadioListener::updatePublic() {

    if (qtRadioPort == 11000)
		return;

    QUrl serviceUrl = QUrl(getSettings(settings, "Common/url", "http://napan.com/qtradio/qtradioreg.pl"));
    QByteArray postData;
    postData.append(QString("call=%1&").arg(getSettings(settings,"Common/call", "unknown")));
    postData.append(QString("location=%1&").arg(getSettings(settings,"Common/location", "unknown")));
    postData.append(QString("band=%1&").arg(getSettings(settings,"Common/band", "160m-6m")));
    postData.append(QString("rig=%1&").arg(getSettings(settings,"Common/rig", "R2T2")));
    postData.append(QString("ant=%1&").arg(getSettings(settings,"Common/ant", "unknown")));
    postData.append(QString("status=%1 client(s)").arg(clients));

	if (getSettings(settings, "Common/public", "False") == "True") { 
		// Call the webservice
		QNetworkRequest networkRequest(serviceUrl);
		networkRequest.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
		networkManager->post(networkRequest,postData);
	}
}

void R2T2QtRadioListener::postAnswer(QNetworkReply * /*answer*/) {
    qDebug() << "registration ok"; 
}


void R2T2QtRadioListener::disconnected() {
	clients--;
	updatePublic();
}

void R2T2QtRadioListener::newConnection() {
	clients++;
    QTcpSocket *tcpSocket = tcpServer->nextPendingConnection();
    qDebug() << "connection from " << tcpSocket->peerAddress().toString() << tcpSocket->peerPort();
	if (clients>MAX_RX) {
		tcpSocket->close();
		clients--;
		return;
	}

	updatePublic();

    if (qtRadioPort == 11000) {
        r2t2Client = new R2T2Client(settings, addr, port, CLIENT_QTRADIO, tcpSocket, QString(), sampleRate);
        connect (tcpSocket, SIGNAL(readyRead()), r2t2Client, SLOT(readClientTCPData()));
    } else {
        r2t2Client = new R2T2Client(settings, addr, port, CLIENT_QTRADIO_DSPSERVER, tcpSocket, QString(), sampleRate);
        connect (tcpSocket, SIGNAL(readyRead()), r2t2Client, SLOT(readDspTCPData()));
    }
    connect (tcpSocket, SIGNAL(disconnected()), r2t2Client, SLOT(disconnected()));
    connect (tcpSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
}

