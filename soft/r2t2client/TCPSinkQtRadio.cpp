#include "TCPSinkQtRadio.h"
#include "types.h"
#include "config.h"
#include "lib.h"

TCPSinkQtRadio::TCPSinkQtRadio(std::string name, QTcpSocket *tcpSocket, int fftRate) : 
	ProcessBlock(name, 3, 0), tcpSocket(tcpSocket), fftRate(fftRate) {
    setInputType(0, typeid(int8_t)); // audio
    setInputType(1, typeid(uint8_t)); // fft 
    setInputType(2, typeid(float)); // S-Meter 
}

TCPSinkQtRadio::~TCPSinkQtRadio() {
}

int TCPSinkQtRadio::processAudio(int8_t* in0, int cnt) {
	uint8_t audioHeader[5];
	outGlobTime("audio");

	if (cnt<1000)
		return 0;

    audioHeader[0] = AUDIO_BUFFER;
    audioHeader[1] = HEADER_VERSION;
    audioHeader[2] = HEADER_SUBVERSION;
    audioHeader[3] = (cnt >> 8)&0xFF;
    audioHeader[4] = cnt & 0xFF;

	tcpSocket->write((char*)&audioHeader[0], sizeof(audioHeader));
	tcpSocket->write((char*)in0, cnt);
    return cnt;
}

int TCPSinkQtRadio::processFFT(uint8_t* in0, int cnt) {
    int subrxMeter = rxMeter;
    int LO_offset = 0;
    uint8_t spectrumHeader[15];

	outGlobTime("fft");

    spectrumHeader[0]=SPECTRUM_BUFFER;
    spectrumHeader[1]=HEADER_VERSION;
    spectrumHeader[2]=HEADER_SUBVERSION;
    spectrumHeader[3]=((cnt)>>8) & 0xff;
    spectrumHeader[4]=(cnt) & 0xff;
    spectrumHeader[5]=((int)rxMeter>>8)&0xFF; // sub rx meter
    spectrumHeader[6]=(int)rxMeter&0xFF;
    spectrumHeader[7]=((int)subrxMeter>>8)&0xFF; // sub rx meter
    spectrumHeader[8]=(int)subrxMeter&0xFF;
    spectrumHeader[9] =(fftRate>>24)&0xFF; // sample rate
    spectrumHeader[10]=(fftRate>>16)&0xFF;
    spectrumHeader[11]=(fftRate>>8) &0xFF;
    spectrumHeader[12]= fftRate     &0xFF;
    spectrumHeader[13]=((int)LO_offset>>8)&0xFF; // IF
    spectrumHeader[14]=(int)LO_offset&0xFF;

	tcpSocket->write((char*)spectrumHeader, BUFFER_HEADER_SIZE);
	tcpSocket->write((char*)in0, cnt);
    return 0;
}

void TCPSinkQtRadio::setFFTRate(uint32_t rate) {
    fftRate = rate;
}

int TCPSinkQtRadio::receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion) {
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
        rxMeter = (int16_t)(*(float*)**(inBuf[2]));
        inBuf[2].reset();
    }
	return 0;
}
