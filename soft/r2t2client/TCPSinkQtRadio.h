#ifndef _TCPSINK_QTRADIO_H_
#define _TCPSINK_QTRADIO_H_

#include <QTcpSocket>
#include "dsp/ProcessBlock.h"
#include "types.h"

class TCPSinkQtRadio: public ProcessBlock 
{

	public:
		TCPSinkQtRadio(std::string name, QTcpSocket *socket, int fftRate);
		~TCPSinkQtRadio();
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
