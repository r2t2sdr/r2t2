#include <QUdpSocket>
#include <QTimer>
#include <QTime>
#include <QBuffer>
#include <QSettings>
#include <QThread>
#include <QHostAddress>
#include <QMutex>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "config.h"
#include "r2t2.pb.h"
#include "r2t2.h"

#define MAX_RX_CLIENT  MAX_RX
#define MAX_TX_CLIENT  MAX_TX

#define NATIVE_SOCKET 	

#define TYPE_AUDIO 		0
#define TYPE_FFT 		1

typedef enum Usage {RX, TX} Usage;

class R2T2Srv;

typedef struct Client_s {
    Usage usage;
    QHostAddress addr;
    quint16 port;
	int timeout;
	bool work;
	uint32_t fft;
	uint32_t fftSize;
	uint32_t curFFTSize;
    uint32_t seq;
	uint32_t rxBuf[2][512];
	uint32_t rxBufPos[2];
	uint32_t curInput;
} Client;


class UdpReader: public QThread {
	Q_OBJECT
	public:
		UdpReader(R2T2Srv *parent);
		void run();
	private:
		R2T2Srv *parent;
};

class R2T2Srv : public QThread {
	Q_OBJECT
	public:
		R2T2Srv(uint32_t clk, quint16 port);
		~R2T2Srv();


	public slots:
		void readClientUDPData();
	private slots:
		void checkCon();

	private:
		void run();
		void sendR2T2Msg(QHostAddress addr, quint16 port);
		void sendAck(bool, QHostAddress &addr, quint16 port);
		void int2float(void* buf, int cnt, int rx);
		int getServerPacket(char *buf, int len, QHostAddress *addr, quint16 *port);
		void handleFFTData(int32_t *out, int len);
		void handleAudioData(int32_t *out, int len);

        R2T2 *r2t2;
#ifdef NATIVE_SOCKET
		int clientSocket;
		UdpReader *udpReader;
#else
        QUdpSocket *clientSocket;
#endif
		QMutex mutex;
        Client client[MAX_RX_CLIENT + MAX_TX_CLIENT];
		quint16 port;
		R2T2Proto::R2T2Message *r2t2Msg, *r2t2ClientMsg;
		uint8_t outBuf[64*1024];
		int allocClient(QHostAddress, quint16, Usage);
		int freeClient(QHostAddress, quint16);
		int getClient(QHostAddress, quint16);
		void clearClient(uint32_t n);

		QTimer *conTimer;
		struct sockaddr_in iq_addr;
		float gain[2];
		float att[2];
};


