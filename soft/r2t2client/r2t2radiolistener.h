#ifndef _R2T2_RADIOLISTENER_
#define _R2T2_RADIOLISTENER_

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
#include "r2t2clientdspqtradio.h"
#include "r2t2clientqtradio.h"
#include "r2t2clientstandard.h"

#define MAX_RX 	16	

class R2T2RadioListener : public QObject  {
	Q_OBJECT
	public:
		R2T2RadioListener(QSettings *settings, QString addr, quint16 port, quint16 listenPort, uint32_t sampleRate);
		~R2T2RadioListener();

	public slots:

    signals:
        void triggerWatchdog(int);


	private slots:
        void newConnection();
        void disconnected();
        void postAnswer(QNetworkReply *answer);
        void updatePublic();
        void sendWatchdog();

	private:
		QSettings *settings;
        QString addr;
        QNetworkAccessManager *networkManager; 
		QTimer *timer, *watchTimer;
        quint16 port;
        QTcpServer *tcpServer;
        R2T2ClientQtRadio *r2t2ClientQtRadio;
        R2T2ClientDSPQtRadio *r2t2ClientDSPQtRadio;
        R2T2ClientStandard *r2t2ClientStandard;
        quint16 qtRadioPort;
		uint32_t sampleRate;
		int clients=0;
};
#endif
