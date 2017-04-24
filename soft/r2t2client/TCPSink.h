#ifndef _TCPSINK_H_
#define _TCPSINK_H_

#include <QTcpSocket>
#include "dsp/ProcessBlock.h"
#include "r2t2gui.pb.h"
#include "types.h"

class TCPSink: public ProcessBlock 
{

	public:
		TCPSink(std::string name, QTcpSocket *socket, int fftRate);
		~TCPSink();
		int receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion);
        void setFFTRate(uint32_t rate);

	private:
        void send();

		uint8_t outBuf[64*1024];
        int processFFT(uint8_t* in0, int cnt);
        int processAudio(int8_t* in0, int cnt);
		R2T2GuiProto::R2T2GuiMessageAnswer *r2t2GuiMsgAnswer;
		QTcpSocket *tcpSocket;
        int fftRate;
        float rxMeter;
};

#endif
