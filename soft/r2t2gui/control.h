#include <QUdpSocket>
#include <QTimer>
#include <QBuffer>
#include <QSettings>
#include <QNetworkAccessManager>
#include "config.h"
#include "sdr.h"
#include "display_base.h"
#include "audioQt.h"
#include "conf.h"

class Control : public QObject  {
	Q_OBJECT
	public:
		Control(char*, char*, char*, char* ,char*, int, bool);
		~Control();


	public slots:
		void controlCommand(int, int, int, bool inital=false);
		void readHamLibUDPData();
		void cleanup();

	signals:
		void displaySet(int,int,int);

	private slots:
		void timeout();
		void readSettings();
		void writeSettings();
        void readServer();
        void replyFinished(QNetworkReply*);
        void smtrVal(int16_t);

	private:
		void setMode();

		QTimer *timer;
		QString r2t2IP;
		QUdpSocket *hamLibSocket;
        QNetworkAccessManager *manager;
		Sdr *sdr, *sdrqt, *sdrr2t2;
#ifdef UNIX
		//KeyReader *keyReader;
#endif
		Audio *audio;
		Display_base *disp;
		QSettings *settings;
        Conf *conf;
		QHostAddress sender;
		quint16 senderPort;
		uint8_t hpRxBuf[1200];
		int hpRxBufPos;
		uint8_t c[6];
		int txChanged;
        QList<QStringList> servers;

		int hp_receivers;
		int cCnt;
		bool initReady;
        bool qtRadioMode;
};
