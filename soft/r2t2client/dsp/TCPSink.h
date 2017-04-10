#ifndef _TCPSINK_H_
#define _TCPSINK_H_

#include <QTcpSocket>
#include "ProcessBlock.h"
#include "types.h"

class TCPSink: public ProcessBlock 
{

	public:
		TCPSink(std::string name, QTcpSocket *socket, int fftRate);
		~TCPSink();
		int receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion);
        void setFFTRate(uint32_t rate);

	private:
        int processFFT(uint8_t* in0, int cnt);
        int processAudio(int8_t* in0, int cnt);
		QTcpSocket *tcpSocket;
        int fftRate;
        int16_t rxMeter;
};

#endif
