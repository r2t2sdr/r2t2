#include <google/protobuf/text_format.h>
#include "TCPSink.h"
#include "types.h"
#include "config.h"
#include "lib.h"

extern int debugLevel;

TCPSink::TCPSink(std::string name, QTcpSocket *tcpSocket, int fftRate) : ProcessBlock(name, 3, 0), tcpSocket(tcpSocket), fftRate(fftRate) {
    r2t2GuiMsgAnswer = new R2T2GuiProto::R2T2GuiMessageAnswer();

    setInputType(0, typeid(int8_t)); // audio
    setInputType(1, typeid(uint8_t)); // fft 
    setInputType(2, typeid(float)); // S-Meter 
}

TCPSink::~TCPSink() {
    delete r2t2GuiMsgAnswer;
}

void TCPSink::send() {
    std::ostringstream out;
    r2t2GuiMsgAnswer->SerializeToOstream(&out);

	if (debugLevel == 5) {
		std::string formated;
		google::protobuf::TextFormat::PrintToString(*r2t2GuiMsgAnswer, &formated);
		qDebug() << "send message to gui from rx\n" << formated.data();
	}

    strcpy((char*)outBuf, "R2");
    outBuf[2] = out.str().size() & 0xff;
    outBuf[3] = (out.str().size() >> 8) & 0xff;

    memcpy(outBuf+4, out.str().data(), out.str().size());
    tcpSocket->write((const char*)outBuf, out.str().size()+4);

    r2t2GuiMsgAnswer->Clear();
}

int TCPSink::processAudio(int8_t* in0, int cnt) {
    r2t2GuiMsgAnswer->set_rxdata(in0, cnt);
    send();
    return cnt;
}

int TCPSink::processFFT(uint8_t* in0, int cnt) {
    r2t2GuiMsgAnswer->set_fftdata(in0, cnt);
    r2t2GuiMsgAnswer->set_rssi(rxMeter);
    r2t2GuiMsgAnswer->set_fftrate(fftRate);
    send();
    return cnt;
}

void TCPSink::setFFTRate(uint32_t rate) {
    fftRate = rate;
}

int TCPSink::receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion) {
	preReceive(buf, input, recursion);

    if (input == 0) {
        int nElements = processAudio((int8_t*)**(inBuf[0]), inBuf[0]->size());
        inBuf[0]->processed(nElements);
    }
    if (input == 1) {
        processFFT((uint8_t*)**(inBuf[1]), inBuf[1]->size());
        inBuf[1].reset();
    }
    if (input == 2) {
        rxMeter = *(float*)**(inBuf[2]);
        inBuf[2].reset();
    }
	return 0;
}
