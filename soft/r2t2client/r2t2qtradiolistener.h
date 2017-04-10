#ifndef _R2T2_QTRADIOLISTENER_
#define _R2T2_QTRADIOLISTENER_

#include <QUdpSocket>
#include <QTcpSocket>
#include <QTcpServer>
#include <QTimer>
#include <QTime>
#include <QBuffer>
#include <QSettings>
#include <QHostAddress>
#include <QTextStream>
#include <QThread>
#include <QFile>
#include <QMutex>
#include <QNetworkAccessManager>
#include "config.h"
#include "r2t2client.h"

#define MAX_RX 	16	

class R2T2QtRadioListener : QObject  {
	Q_OBJECT
	public:
		R2T2QtRadioListener(QSettings *settings, QString addr, quint16 port, quint16 listenPort, uint32_t sampleRate);
		~R2T2QtRadioListener();

	public slots:

	private slots:
        void newConnection();
        void disconnected();
        void postAnswer(QNetworkReply *answer);
        void updatePublic();

	private:
		QSettings *settings;
        QString addr;
        QNetworkAccessManager *networkManager; 
		QTimer *timer;
        quint16 port;
        QTcpServer *tcpServer;
        R2T2Client *r2t2Client;
        quint16 qtRadioPort;
		uint32_t sampleRate;
		int clients=0;
};
#endif
