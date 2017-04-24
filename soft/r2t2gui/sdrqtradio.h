#include <QTcpSocket>
#include <QTimer>
#include <QThread>
#include <QSettings>
#include <stdint.h>
#include "sdr.h"


class SdrQtRadio : public Sdr  {
	Q_OBJECT

	public:
		SdrQtRadio(QString ip, int port);
		~SdrQtRadio();

		void run();
		void setRXFreq(uint32_t f);
		void setTXFreq(uint32_t f);
		void setSampleRate(int rate);
		void setAnt(int);
		void setPresel(int);
		void setAttenuator(int);
		void setTXLevel(int l);
		void setPtt(bool on);
		void setTXRate(int);
		void setFilter(int lo, int hi);
		void setMode(int);
		void setGain(int);
		void setAGC(int);
		void setFFT(int time,int size);
		void setFFTRate(int rate);
		void setVolume(double);
		void setMicGain(double);
		void setToneTest(bool,  double, double, double, double);
		void startRX();
		void stopRX();
		void setActive(bool);
		void setAudioOff(bool);
		void setTxDelay(int);
		void setNBLevel(int);
		void setNotch(int);
		void setSquelch(int);
		void setComp(int);
		void selectPresel(int);
        void setRx(int);

	public slots:
        void terminate();
		void readServerTCPData();
        void setServer(QString ip, uint16_t port);
        void connectServer(bool);

	signals:
		void fftData(QByteArray);
		void audioRX(QByteArray);
        void controlCommand(int,int,int);


    private slots:
        void fftTime();
        void tcpTimeout();
        void error(QAbstractSocket::SocketError /*error*/);
		void connected();
		void disconnected();

    private:
        void sendStartSeq();
        QTcpSocket *tcpSocket;
        QTimer *timer,*tcpTimer;
		QByteArray inBuf;
        QString ip;
        uint16_t port;

		void handleSpectrum(int len);
		void handleAudio(int len);
		void sendCmd(QString cmd);
        bool conn = false;
        bool startRx = false;
        int fftSize = 1024;
        int fftTimeRep = 0;
		int16_t outBuf[81920];
		bool sdrRun = true;
        uint32_t rxFreq = 7100000;
        int antenna = 0;
        int att = 0;
        int pre = 0;
        int mode = 0;
        int filterLo = -3000;
        int filterHi = -300;
        int rx = 0;
};

