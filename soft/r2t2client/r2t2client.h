#ifndef _R2T2_CLIENT_
#define _R2T2_CLIENT_

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
#include <QSettings>
#include <QFile>
#include <QMutex>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "config.h"
#include "r2t2.pb.h"
#include "dsp/RX.h"
#include "dsp/TCPSink.h"

#define NATIVE_SOCKET 	
#define NATIVE_SOCKET_CLIENT	

typedef enum {
	CLIENT_CONSOLE,
	CLIENT_QTRADIO,
	CLIENT_QTRADIO_DSPSERVER
} ClientProto;

class R2T2Client;
class UdpReader: public QThread {
	Q_OBJECT
	public:
		UdpReader(R2T2Client *parent);
		~UdpReader();
		void run();
    signals:
        void sendAudio(std::shared_ptr<ProcessBuffer>);
        void sendFFT(std::shared_ptr<ProcessBuffer>);
		void sendCmd(int);
	private:
		R2T2Client *parent;
};


class R2T2Client : public QThread {
	Q_OBJECT
	public:
		R2T2Client(QSettings *settings, QString addr, quint16 port, ClientProto proto=CLIENT_CONSOLE, 
				QTcpSocket* tcpSocket=NULL, QString fileName=QString(), uint32_t sampleRate=8000, uint32_t fftRate=192000);
		~R2T2Client();

	public slots:
        void readR2T2ServerUDPData();

	private slots:
        void readClientTCPData();
        void readDspTCPData();
        void readConsole();
        void sendTxToneTestData();
        void sendTxDataRestart();
        void sendAlive();
        void disconnected();
        void sendAudioData(std::shared_ptr<ProcessBuffer>);
        void sendFFTData(std::shared_ptr<ProcessBuffer>);
		void sendCmd(int);
	private:

		void run();
		void sendR2T2Msg();
		void usage();
		void sendClientTcpResp(QString str);
		void handleQtRadioRxData(uint8_t* data, int len);
		void handleQtRadioDspRxData(uint8_t* data, int len);
        void handleQtRadioFFTData(uint8_t* data, int len);
        void handleQtRadioDspFFTData(uint8_t* data, int len);
        void sendQResp(QString q, QString s, QString sep=QString(":"));

#ifdef NATIVE_SOCKET
		int openSocket (const char *host, unsigned short port);
		int getServerPacket(char *buf, int len);

		uint8_t outBuf[64*1024];
		int serverSocket;
		UdpReader *udpReader;
#else
        QUdpSocket *serverSocket;
#endif

#ifdef NATIVE_SOCKET_CLIENT
		int clientSocket;
#else
        QUdpSocket *clientSocket;
#endif
		QSettings *settings;
		QHostAddress dstAddr;
		quint16 port;
		ClientProto proto;
        QTcpSocket *tcpSocket;

		QFile *cmdFile;
		QHostAddress qtClientAddr,qtClientFFTAddr;
		QTextStream *input,*output;
		QTimer *txTimer, *aliveTimer;
		R2T2Proto::R2T2Message *r2t2Msg, *r2t2ServerMsg;
		R2T2Proto::R2T2Message_Command lastCommand;
		RX *rx;
		TCPSink *tcpSink;
		bool connected=false;
		bool r2t2Run = true;
		double p1=0,p2=0;
		float txBuf[256];
		int alive=0;
		int curAntenna=0;
		int32_t txAck,lastAck;
		struct sockaddr_in iq_addr;
		uint16_t qtClientPort=0,qtClientFFTPort=0;
		uint32_t sampleRate,fftRate;
		uint32_t txRate = 48000;
		uint64_t qtFFTSequence=0;
		uint64_t qtSequence=0;
        QMutex mutex;
        float rxBuf[MAX_FFT_SIZE];
        int fftBufPos = 0;
        int rxBufPos = 0;
        int spectrumReady=0;
        int32_t toneTestFreq=0;
        uint8_t fftBuf[MAX_FFT_SIZE*2*sizeof(float)];
};

#endif
