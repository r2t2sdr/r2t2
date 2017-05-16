#include <QDebug>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest> 
#include "r2t2radiolistener.h"
#include "lib.h"

R2T2RadioListener::R2T2RadioListener(QSettings *settings, QString addr, quint16 port, quint16 listenPort, uint32_t sampleRate): 
    settings(settings), addr(addr), port(port), qtRadioPort(listenPort), sampleRate(sampleRate) 
{
	networkManager = new QNetworkAccessManager(this);
	connect(networkManager, SIGNAL(finished(QNetworkReply*)), SLOT(postAnswer(QNetworkReply*)));
    r2t2ClientStandard = NULL;
    r2t2ClientQtRadio = NULL;
    r2t2ClientDSPQtRadio = NULL;

	updatePublic();

    tcpServer = new QTcpServer(this);
    if (!tcpServer->listen(QHostAddress::Any, qtRadioPort)) {
        qDebug() << "TCP-Server error" << tcpServer->errorString();
        return;
    }

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(updatePublic()));
	timer->start(240*1000);

	watchTimer = new QTimer(this);
	connect(watchTimer, SIGNAL(timeout()), this, SLOT(sendWatchdog()));
	watchTimer->start(5*1000);

    qDebug() << "listening on port " << qtRadioPort;
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

R2T2RadioListener::~R2T2RadioListener() {
    delete tcpServer;
    delete networkManager;
    if (r2t2ClientStandard)
        delete r2t2ClientStandard;
    if (r2t2ClientQtRadio)
        delete r2t2ClientQtRadio;
    if (r2t2ClientDSPQtRadio)
        delete r2t2ClientDSPQtRadio;
}

void R2T2RadioListener::updatePublic() {

    if (qtRadioPort == QTRADIO_SERVER_PORT)
        return;

    if (getSettings(settings, "Common/public", "False") == "True") {
        if (qtRadioPort == QTRADIO_DSP_PORT) {
			QString regUrl = getSettings(settings, "Common/url", "http://napan.com/qtradio/qtradioreg.pl");
			qDebug() << "register: " << clients << " clients  at" << regUrl ;
            QUrl serviceUrl = QUrl(regUrl);
            QNetworkRequest networkRequest(serviceUrl);
            networkRequest.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");

            QByteArray postData;
            postData.append(QString("call=%1&").arg(getSettings(settings,"Common/call", "unknown")));
            postData.append(QString("location=%1&").arg(getSettings(settings,"Common/location", "unknown")));
            postData.append(QString("band=%1&").arg(getSettings(settings,"Common/band", "160m-6m")));
            postData.append(QString("rig=%1&").arg(getSettings(settings,"Common/rig", "R2T2")));
            postData.append(QString("ant=%1&").arg(getSettings(settings,"Common/ant", "unknown")));
            postData.append(QString("status=%1 client(s)").arg(clients));

            networkManager->post(networkRequest,postData);
        }

        if (qtRadioPort == R2T2_PORT) {
			QString regUrl = getSettings(settings, "Common/urlr2t2", "http://dl2stg.de/cgi-bin/r2t2.py");
			qDebug() << "register: " << clients << " clients  at" << regUrl ;
            QUrl serviceUrl = QUrl(regUrl);
            QNetworkRequest networkRequest(serviceUrl);
            networkRequest.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");

            QByteArray postData;
            postData.append(QString("call=%1&").arg(getSettings(settings,"Common/call", "unknown")));
            postData.append(QString("location=%1&").arg(getSettings(settings,"Common/location", "unknown")));
            postData.append(QString("band=%1&").arg(getSettings(settings,"Common/band", "160m-6m")));
            postData.append(QString("rig=%1&").arg(getSettings(settings,"Common/rig", "R2T2")));
            postData.append(QString("ant=%1&").arg(getSettings(settings,"Common/ant", "unknown")));
            postData.append(QString("status=%1 client(s)").arg(clients));

            networkManager->post(networkRequest,postData);
        }
    }
}

void R2T2RadioListener::postAnswer(QNetworkReply * /*answer*/) {
    qDebug() << "registration ok"; 
}


void R2T2RadioListener::disconnected() {
	clients--;
	updatePublic();
}

void R2T2RadioListener::newConnection() {
	clients++;
    QTcpSocket *tcpSocket = tcpServer->nextPendingConnection();
    qDebug() << "connection from " << tcpSocket->peerAddress().toString() << tcpSocket->peerPort();
	if (clients>MAX_RX) {
		tcpSocket->close();
		clients--;
		return;
	}

	updatePublic();

    if (qtRadioPort == QTRADIO_SERVER_PORT) {
        r2t2ClientQtRadio = new R2T2ClientQtRadio(settings, addr, port, tcpSocket, QString(), sampleRate);
        connect (tcpSocket, SIGNAL(readyRead()), r2t2ClientQtRadio, SLOT(readClientTCPData()));
        connect (tcpSocket, SIGNAL(disconnected()), r2t2ClientQtRadio, SLOT(disconnected()));
    } else if (qtRadioPort == QTRADIO_DSP_PORT) {
        r2t2ClientDSPQtRadio = new R2T2ClientDSPQtRadio(settings, addr, port, tcpSocket, QString(), sampleRate);
        connect (tcpSocket, SIGNAL(readyRead()), r2t2ClientDSPQtRadio, SLOT(readDspTCPData()));
        connect (tcpSocket, SIGNAL(disconnected()), r2t2ClientDSPQtRadio, SLOT(disconnected()));
    } else {
        r2t2ClientStandard = new R2T2ClientStandard(settings, addr, port, tcpSocket, QString(), sampleRate);
        connect (tcpSocket, SIGNAL(readyRead()), r2t2ClientStandard, SLOT(readGuiTCPData()));
        connect (tcpSocket, SIGNAL(disconnected()), r2t2ClientStandard, SLOT(disconnected()));
    }
    connect (tcpSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
}

void R2T2RadioListener::sendWatchdog() {
    emit triggerWatchdog(0);
}
