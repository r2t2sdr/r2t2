#ifndef _SDR_H_
#define _SDR_H_

#include <QThread>
#include <stdint.h>

class Sdr : public QObject  {
	Q_OBJECT

	public:
		// Sdr(QString ip, int port);
		//Sdr();
        virtual ~Sdr() {}

    public slots:
        virtual void init()=0;
		virtual void setRXFreq(uint32_t f)=0;
		virtual void setTXFreq(uint32_t f)=0;
		virtual void setSampleRate(int rate)=0;
		virtual void setAnt(int)=0;
		virtual void setPresel(int)=0;
		virtual void setAttenuator(int)=0;
		virtual void setTXLevel(int l)=0;
		virtual void setPtt(bool on)=0;
		virtual void setTXRate(int)=0;
		virtual void setFilter(int lo, int hi)=0;
		virtual void setMode(int)=0;
		virtual void setGain(int)=0;
		virtual void setAGC(int)=0;
		virtual void setFFT(int time,int size)=0;
		virtual void setFFTRate(int rate)=0;
		virtual void setVolume(double)=0;
		virtual void setMicGain(double)=0;
		virtual void setToneTest(bool,  double, double, double, double)=0;
		virtual void startRX()=0;
		virtual void stopRX()=0;
		virtual void setActive(bool)=0;
		virtual void setAudioOff(bool)=0;
		virtual void setTxDelay(int)=0;
		virtual void setNBLevel(int)=0;
		virtual void setNotch(int)=0;
		virtual void setSquelch(int)=0;
		virtual void setComp(int)=0;
		virtual void selectPresel(int)=0;
        virtual void setRx(int)=0;
        virtual void setNoiseFilter(int)=0;

		virtual void readServerTCPData()=0;
        virtual void setServer(QString ip, uint16_t port)=0;
        virtual void connectServer(bool)=0;

	signals:
		//void fftData(QByteArray)=0;
		//void audioRX(QByteArray)=0;
        //void controlCommand(int,int,int)=0;
};

#endif
