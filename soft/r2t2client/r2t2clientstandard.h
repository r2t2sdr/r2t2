#ifndef _R2T2_CLIENT_STANDARD_
#define _R2T2_CLIENT_STANDARD_

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
#include "r2t2gui.pb.h"
#include "dsp/RX.h"
#include "TCPSink.h"

#define NATIVE_SOCKET 	
#define NATIVE_SOCKET_CLIENT	

class R2T2ClientStandard;
class UdpReaderR2T2ClientStandard : public QThread {
	Q_OBJECT
	public:
		UdpReaderR2T2ClientStandard (R2T2ClientStandard *parent);
		~UdpReaderR2T2ClientStandard ();
		void run();
    signals:
        void sendAudio(std::shared_ptr<ProcessBuffer>);
        void sendFFT(std::shared_ptr<ProcessBuffer>);
		void sendCmd(int);
	private:
		R2T2ClientStandard *parent;
};


class R2T2ClientStandard : public QThread {
	Q_OBJECT
	public:
		R2T2ClientStandard(QSettings *settings, QString addr, quint16 port,
				QTcpSocket* tcpSocket=NULL, QString fileName=QString(), uint32_t sampleRate=8000, uint32_t fftRate=192000);
		~R2T2ClientStandard();

	public slots:
        void readR2T2ServerUDPData();

	private slots:
        void readGuiTCPData();
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
		void sendR2T2GuiMsgAnswer();
		void handleRadioRxData(uint8_t* data, int len);
        void handleRadioFFTData(uint8_t* data, int len);

#ifdef NATIVE_SOCKET
		int openSocket (const char *host, unsigned short port);
		int getServerPacket(char *buf, int len);

		uint8_t outBuf[64*1024];
		uint8_t outBufGui[64*1024];
		int serverSocket;
		UdpReaderR2T2ClientStandard  *udpReader;
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
        QTcpSocket *tcpSocket;

		QFile *cmdFile;
		QHostAddress qtClientAddr,qtClientFFTAddr;
		QTextStream *input,*output;
		QTimer *txTimer, *aliveTimer;
		R2T2GuiProto::R2T2GuiMessage *r2t2GuiMsg;
		R2T2GuiProto::R2T2GuiMessageAnswer *r2t2GuiMsgAnswer;
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
		int gain[2];
		int32_t txAck,lastAck;
		struct sockaddr_in iq_addr;
		uint16_t qtClientPort=0,qtClientFFTPort=0;
		uint32_t sampleRate,fftRate;
		uint32_t txRate = 48000;
		uint64_t qtFFTSequence=0;
		uint64_t qtSequence=0;
        int filterLo = 300;
        int filterHi = 3000;
        QMutex mutex;
        float rxBuf[MAX_FFT_SIZE];
        int fftBufPos = 0;
        int rxBufPos = 0;
        int spectrumReady=0;
        int32_t toneTestFreq=0;
        uint8_t fftBuf[MAX_FFT_SIZE*2*sizeof(float)];
};

#endif
