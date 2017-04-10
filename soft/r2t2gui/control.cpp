#include <QTime>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <stdio.h>
#include "control.h"
#include "config.h"
#include "lib.h"
#include "display_touch.h"
#include "display_lcd.h"

#define X(a, b) b,
const char *CmdString[] = { CMDS };
#undef X
extern bool touchscreen;
extern bool defaults;

const char *SrcString[] = {"Display","Sdr", "Control", "Key", "DisplayKey"};


Control::Control(char *ip, char* audiodev, char* audiodevMixer, char* mixerVolume, char* mixerMic, int sampleRate)  {

	qDebug() << "R2T2 Version " << VERSION;
	initReady = false;

    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

	txChanged = 0;
	settings =  new QSettings("sdr","r2t2");
	readGlobalSettings(settings);
	if (ip)
		r2t2IP = ip;
	else
		r2t2IP    = "192.168.1.99";

	for (int i=0;i<CMD_LAST;i++)
		config[i] = 0;
	printf("r2t2 SVN: %s\n",SVNREV);

	config[CMD_AGC_DEC] = 10000;
	config[CMD_ANT] = 0;
	config[CMD_DISP_MODE] = 1;
	config[CMD_EXIT] = 1;
	config[CMD_FFT_COMPLEX] = 1;
	config[CMD_FFT_SAMPLE_RATE] = 192000;
	config[CMD_FFT_SIZE] = FFT_SIZE;
	config[CMD_FFT_TIME] = 70;
	config[CMD_RX_FILTER_GAIN] = 3;
	config[CMD_TX_FILTER_GAIN] = 3;
	config[CMD_FILTER_RX_CUT] = 120;
	config[CMD_FILTER_RX_HI] = 2700;
	config[CMD_FILTER_RX_LO] = 240;
	config[CMD_FILTER_TX_CUT] = 250;
	config[CMD_FILTER_TX_HI] = 2700;
	config[CMD_FILTER_TX_LO] = 200;
	config[CMD_GAIN] = 0;
	config[CMD_INTERNAL] =  C_INTERNAL_INT;
	config[CMD_MODE] = MODE_LSB;
	config[CMD_PREAMP] = 0;
	config[CMD_PRESEL] = 1;
	config[CMD_RX_FREQ] = 7100000;
	config[CMD_SAMPLE_RATE] = DEFAULT_SAMPLE_RATE;
	config[CMD_STEP] = 100;
	config[CMD_SWR] = 0;
	config[CMD_TX] = 0;
	config[CMD_TX_FREQ] = config[CMD_RX_FREQ];
	config[CMD_TX_POWER] = 100;
	config[CMD_VOLUME] = 30;
	config[CMD_MIC] = 0;
	config[CMD_WATERFALL_MAX] = WATERFALL_MAX;
	config[CMD_WATERFALL_MIN] = WATERFALL_MIN;
	config[CMD_TX_DELAY] = 0;
	config[CMD_NB_LEVEL] = 0xff;
	config[CMD_TX_RX] = 1;
	config[CMD_DISP_COLOR] = 1;
	config[CMD_AUDIO_COMP] = 0;

	if (!defaults)
		readSettings();

	config[CMD_SAMPLE_RATE] = sampleRate;
	config[CMD_TX_FREQ] = config[CMD_RX_FREQ];

	if (touchscreen)
		qApp->setOverrideCursor(QCursor( Qt::BlankCursor ));
	disp = new Display_touch(settings);
	//disp->start();

	sdr = new Sdr(r2t2IP, config[CMD_SAMPLE_RATE]);
	sdr->setSampleRate(config[CMD_SAMPLE_RATE]);
	sdr->setFFT(config[CMD_FFT_TIME], config[CMD_FFT_SIZE]);
	sdr->start();

	audio = new Audio(audiodev, audiodevMixer, mixerVolume, mixerMic, config[CMD_SAMPLE_RATE]);
	//connect(audio, SIGNAL(audioTX(QByteArray)), sdr, SLOT(writeR2T2(QByteArray)), Qt::QueuedConnection);

	timer = new QTimer(this);

#ifdef UNIX
	//keyReader = new KeyReader();

	//connect(keyReader, SIGNAL(controlCommand(int,int,int)), this, SLOT(controlCommand(int,int,int))); 
#endif
	connect(sdr, SIGNAL(audioRX(QByteArray)), audio, SLOT(audioRX(QByteArray)));
	connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
	connect(disp, SIGNAL(command(int, int, int)), this, SLOT(controlCommand(int, int, int)));
	connect(this, SIGNAL(displaySet(int, int, int)), disp, SLOT(displaySet(int, int, int)));
	connect(sdr, SIGNAL(fftData(QByteArray)), disp, SLOT(fftData(QByteArray)), Qt::QueuedConnection);
	connect(sdr, SIGNAL(controlCommand(int,int,int)), this, SLOT(controlCommand(int,int,int)));

#ifdef UNIX
	//keyReader->start();
#endif
	hamLibSocket = new QUdpSocket(this);
	hamLibSocket->bind(13222, QUdpSocket::ShareAddress);
	connect(hamLibSocket, SIGNAL(readyRead()), this, SLOT(readHamLibUDPData()));

	audio->start(QThread::TimeCriticalPriority);

	hpRxBufPos=0;
	sender=0;
	senderPort=0;
	cCnt=0;

	hp_receivers=1;
	memset(hpRxBuf, 0, sizeof(hpRxBuf));

	for (int i=0;i<CMD_INIT_END;i++) 
		controlCommand(SRC_CTL, i, config[i], true);
	initReady = true;

    readServer();
}

Control::~Control() {
	writeSettings();
	delete timer;
	audio->terminate();
	audio->wait();
	delete audio;
	sdr->terminate();
	sdr->wait();
	delete sdr;
	delete disp;
#ifdef UNIX
	//keyReader->restore();
	//keyReader->terminate();
	//keyReader->wait();
	//delete keyReader;
#endif
	delete settings;
}

void Control::cleanup() {
	delete this;
}

void Control::timeout() {
}

void Control::readServer() {
    if (manager)
        manager->get(QNetworkRequest(QUrl("http://napan.com/qtradio/qtradiolist.pl")));
}

void Control::replyFinished(QNetworkReply *reply) {
    servers.clear();
    while(1) {
        QByteArray a=reply->readLine();
        if (a.size()==0)
            break;
        servers.append(QString(a).split('~'));
    }; 

    QList<QStringList>::iterator i;
    QStringList calls;
    for (i=servers.begin(); i!=servers.end(); ++i)
        calls << (*i)[1];
    disp->setServers(calls);
}

void Control::controlCommand(int src, int cmd, int par, bool initial) {
	PDEBUG(MSG1, "%s -> Control: %s, Par: %i",SrcString[src], CmdString[cmd],par);
	if (!initial && (config[cmd] == par))
		return;
	config[cmd]=par;

	if (src != SRC_DISP)
		emit displaySet(SRC_CTL, cmd, par);

	switch ((CtlCmd)cmd) {
		case CMD_EXIT:	// CTL_C
			sdr->setPtt(false);	
			sdr->setNBLevel(255);	
			QCoreApplication::exit();
			break;
		case CMD_RX_FREQ:
			sdr->setRXFreq(config[CMD_RX_FREQ]);	
			break;
		case CMD_TX_FREQ:
			sdr->setTXFreq(config[CMD_TX_FREQ]);	
			break;
		case CMD_MODE:
			setMode();
			break;
		case CMD_FILTER_RX_LO:
			setMode();
			break;
		case CMD_FILTER_RX_HI:
			setMode();
			break;
		case CMD_TX:
			txChanged = 50;
			if (src != SRC_SDR)
				sdr->setPtt(config[CMD_TX]);	
			audio->setTX(config[CMD_TX]);
			break;
		case CMD_VOLUME:
			audio->setVolume(checkRange(config[CMD_VOLUME],0,255));
			break;
		case CMD_MIC:
			audio->setMic(checkRange(config[CMD_MIC],0,255));
			break;
		case CMD_PREAMP:
			sdr->setAttenuator(config[CMD_PREAMP]);	
			break;
		case CMD_ANT:
			sdr->setAnt(config[CMD_ANT]);	
			break;
		case CMD_AGC_DEC:
			sdr->setAGCDec(1.0/config[CMD_AGC_DEC]);
			break;
		case CMD_TX_POWER:
			sdr->setTXLevel(config[CMD_TX_POWER]);
			break;
		case CMD_GAIN:
			if (config[CMD_GAIN]==0)
				sdr->setGain(0);
			else
				sdr->setGain(exp(config[CMD_GAIN]/5));
			break;
		case CMD_FREQ_STEP_UP:
			config[CMD_RX_FREQ] += config[CMD_STEP];
			if (config[CMD_RX_FREQ] > RX_CLOCK/2)
				config[CMD_RX_FREQ] = RX_CLOCK/2;
			sdr->setRXFreq(config[CMD_RX_FREQ]);	
			break;
		case CMD_FREQ_STEP_DOWN:
			config[CMD_RX_FREQ] -= config[CMD_STEP];
			if (config[CMD_RX_FREQ] < 0)
				config[CMD_RX_FREQ] = 0;
			sdr->setRXFreq(config[CMD_RX_FREQ]);	
			break;
		case CMD_INTERNAL:
			sdr->setTXRate(config[CMD_SAMPLE_RATE]);
			sdr->setSampleRate(config[CMD_SAMPLE_RATE]);
			config[CMD_SAMPLE_RATE]=config[CMD_SAMPLE_RATE];
			switch (config[CMD_INTERNAL]) {
				case C_INTERNAL_INT:
					// restore state
					sdr->setActive(true);
					sdr->setAudioOff(false);
					sdr->startRX();
					setMode();
					break;
				case C_INTERNAL_OFF:
					sdr->setActive(false);
					sdr->setAudioOff(true);
					sdr->stopRX();
					break;
				case C_INTERNAL_EXT:
					sdr->setActive(true);
					sdr->setAudioOff(true);
					sdr->stopRX();
					break;
				case C_INTERNAL_HPSDR:
					sdr->setTXRate(48000);
					sdr->setActive(true);
					sdr->setAudioOff(true);
					sdr->startRX();
					break;
			}
			break;
		case CMD_TWO_TONE_TEST:
			{
				double l1,l2;
				double f1 = getSettings(settings,"Common/testToneFreq1",721);
				double f2 = getSettings(settings,"Common/testToneFreq2",1230);
				double f3 = getSettings(settings,"Common/testToneFreqSingle",1041);
				switch (config[CMD_TWO_TONE_TEST]) {
					case 0:
						l1=0;l2=0;
						break;
					case 1:
						l1=1.0/2/sqrt(10000);l2=l1;
						break;
					case 2:
						l1=1.0/2/sqrt(1000);l2=l1;
						break;
					case 3:
						l1=1.0/2/sqrt(100);l2=l1;
						break;
					case 4:
						l1=1.0/2/sqrt(10);l2=l1;
						break;
					case 5:
						l1=1.0/2;l2=l1;
						break;
					default:
						f1=f3;l1=1;f2=0;l2=0;
				}
				sdr->setToneTest(config[CMD_TWO_TONE_TEST]>0, f1,l1,f2,l2);
			}
			break;
		case CMD_TX_DELAY:
			sdr->setTxDelay(config[CMD_TX_DELAY]);
			break;
		case CMD_NB_LEVEL:
			sdr->setNBLevel(config[CMD_NB_LEVEL]);
			break;
		case CMD_SAMPLE_RATE:
			sdr->setSampleRate(config[CMD_SAMPLE_RATE]);
			break;
		case CMD_NOTCH:
			sdr->setNotch(config[CMD_NOTCH]);
			break;
		case CMD_SQUELCH:
			sdr->setSquelch(config[CMD_SQUELCH]);
			break;
		case CMD_FFT_SIZE:
			sdr->setFFT(config[CMD_FFT_TIME], config[CMD_FFT_SIZE]);
			break;
		case CMD_LAYOUT_CHANGED:
			if (initReady) 
				for (int i=0;i<CMD_INIT_END;i++) 
					controlCommand(SRC_CTL, i, config[i], true);
			break;
		case CMD_AUDIO_COMP:
			sdr->setComp(config[CMD_AUDIO_COMP]);
			break;
		case CMD_PRESEL:
			sdr->selectPresel(config[CMD_PRESEL]);
			break;
		case CMD_IN_LEVEL:
		case CMD_TXPOWER_PEEK_LEVEL:
		case CMD_TXPOWER_AV_LEVEL:
		case CMD_STEP:
		case CMD_RSSI:
		case CMD_SWR:
		case CMD_RECONNECT:
		case CMD_DISP_MODE:
		case CMD_BAND:
		case CMD_ADC_OV:
		case CMD_TX_RX:
		case CMD_DISP_COLOR:
		case CMD_FREQ_OFFSET:
			break;
        case CMD_CONNECT:
            if (servers.size() > par)
                sdr->connectServer(servers[par][7],servers[par][8].toInt());
            break;
		default:
			PDEBUG(ERR1, "Control: unhandled command: %s, par %i",CmdString[cmd],par);
	}
}

void Control::setMode() {
	sdr->setMode(config[CMD_MODE]);
	switch(config[CMD_MODE]) {
		case MODE_LSB:
			sdr->setRXFreq(config[CMD_RX_FREQ]);	
            sdr->setFilter(-config[CMD_FILTER_RX_HI], -config[CMD_FILTER_RX_LO]);
			break;
		case MODE_USB:
			sdr->setRXFreq(config[CMD_RX_FREQ]);	
            sdr->setFilter(config[CMD_FILTER_RX_LO], config[CMD_FILTER_RX_HI]);
			break;
		case MODE_CW:
			sdr->setRXFreq(config[CMD_RX_FREQ]);	
            sdr->setFilter(-config[CMD_FILTER_RX_LO]/2, config[CMD_FILTER_RX_LO]/2);
			break;
		case MODE_DSB:
		case MODE_AM:
		case MODE_FM:
			sdr->setRXFreq(config[CMD_RX_FREQ]);	
            sdr->setFilter(-config[CMD_FILTER_RX_HI], config[CMD_FILTER_RX_HI]);
			break;
	}
}

void Control::readSettings() {
	settings->beginGroup("Version");
	int version = settings->value("verint",0).toInt();
	settings->endGroup();

	if (version<VERSION_INT) {
		qDebug() << "old settings version is to old (" << version << "), skip";
		return;
	}

	settings->beginGroup("Control");
	settings->beginReadArray("Config");
	for (int i=0;i<CMD_INIT_END;i++) {
		settings->setArrayIndex(i);
		for(int j=0;j<CMD_INIT_END;j++) {
			if (CmdString[j] == settings->value("name").toString())
				config[j] = settings->value("val",config[j]).toInt();
		}
	}
	settings->endArray();
	settings->endGroup();
}

void Control::writeSettings() {
	settings->beginGroup("Version");
	settings->setValue("ver", VERSION);
	settings->setValue("verint", VERSION_INT);
	settings->endGroup();

	settings->beginGroup("Control");
	settings->beginWriteArray("Config");
	for (int i=0;i<CMD_INIT_END;i++) {
		settings->setArrayIndex(i);
		settings->setValue("name", CmdString[i]);
		settings->setValue("val", config[i]);
	}
	settings->endArray();
	settings->endGroup();
}

void Control::readHamLibUDPData() {
	while (hamLibSocket->hasPendingDatagrams()) {
		QByteArray datagram;
		datagram.resize(hamLibSocket->pendingDatagramSize());
		QHostAddress sender;
		quint16 senderPort;
		QRegExp rx("(\\S) *(\\S*)");

		hamLibSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
		if (rx.indexIn(datagram)!=-1) {

			QString tag = rx.cap(1);
			QString arg = rx.cap(2);
			qint64 iarg = arg.toLong();

			if (tag == "f") {
				controlCommand(SRC_CTL, CMD_RX_FREQ, iarg, false);
				controlCommand(SRC_CTL, CMD_TX_FREQ, iarg, false);
			}
			if (tag == "F") {
				datagram.clear();
				datagram.append(QString("f %1\n").arg(config[CMD_RX_FREQ],12));
				hamLibSocket->writeDatagram(datagram, sender, senderPort);
			}
			if (tag == "t") {
				controlCommand(SRC_CTL, CMD_TX, iarg>0, false);
			}
		}
	}
}

void Control::smtrVal(int16_t val) {
    emit displaySet(SRC_CTL, CMD_RSSI, val);
}
