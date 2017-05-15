#include <QTime>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <stdio.h>
#include "control.h"
#include "config.h"
#include "lib.h"
#include "display_touch.h"
#include "display_lcd.h"
#include "sdrqtradio.h"
#include "sdrr2t2.h"

#define X(a, b) b,
const char *CmdString[] = { CMDS };
#undef X
extern bool touchscreen;
extern bool defaults;

const char *SrcString[] = {"Display","Sdr", "Control", "Key", "DisplayKey"};


Control::Control(char *ip, char* audiodev, char* audiodevMixer, char* mixerVolume, char* mixerMic, int sampleRate, bool qtRadioMode) :qtRadioMode(qtRadioMode)  {

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

    printf("r2t2 SVN: %s\n",SVNREV);

    conf = new Conf(settings, CMD_INIT_END);

    if (!defaults) {
        readSettings();
    }

    conf->set(CMD_SAMPLE_RATE, sampleRate);
    conf->set(CMD_TX_FREQ, conf->get(CMD_RX_FREQ));

    if (touchscreen)
        qApp->setOverrideCursor(QCursor( Qt::BlankCursor ));
    disp = new Display_touch(settings);
    //disp->start();

    sdrqt = new SdrQtRadio(r2t2IP, QT_SERVER_PORT);
    sdrqt->setSampleRate(conf->get(CMD_SAMPLE_RATE));
    sdrqt->setFFT(conf->get(CMD_FFT_TIME), conf->get(CMD_FFT_SIZE));
    sdrqt->start();

    sdrr2t2 = new SdrR2T2(r2t2IP, R2T2_SERVER_PORT);
    sdrr2t2->setSampleRate(conf->get(CMD_SAMPLE_RATE));
    sdrr2t2->setFFT(conf->get(CMD_FFT_TIME), conf->get(CMD_FFT_SIZE));
    sdrr2t2->start();

    if (qtRadioMode)
        sdr = sdrqt; 
    else
        sdr = sdrr2t2; 

    audio = new Audio(audiodev, audiodevMixer, mixerVolume, mixerMic, conf->get(CMD_SAMPLE_RATE));
    //connect(audio, SIGNAL(audioTX(QByteArray)), sdr, SLOT(writeR2T2(QByteArray)), Qt::QueuedConnection);

    serverListUpdateTimer = new QTimer(this);
    serverListUpdateTimer->setSingleShot(true);

#ifdef UNIX
    //keyReader = new KeyReader();

    //connect(keyReader, SIGNAL(controlCommand(int,int,int)), this, SLOT(controlCommand(int,int,int))); 
#endif
    connect(serverListUpdateTimer, SIGNAL(timeout()), this, SLOT(timeout()));
    connect(disp, SIGNAL(command(int, int, int)), this, SLOT(controlCommand(int, int, int)));
    connect(this, SIGNAL(displaySet(int, int, int)), disp, SLOT(displaySet(int, int, int)));
    connect(sdrqt, SIGNAL(audioRX(QByteArray)), audio, SLOT(audioRX(QByteArray)));
    connect(sdrqt, SIGNAL(fftData(QByteArray)), disp, SLOT(fftData(QByteArray)), Qt::QueuedConnection);
    connect(sdrqt, SIGNAL(controlCommand(int,int,int)), this, SLOT(controlCommand(int,int,int)));
    connect(sdrr2t2, SIGNAL(audioRX(QByteArray)), audio, SLOT(audioRX(QByteArray)));
    connect(sdrr2t2, SIGNAL(fftData(QByteArray)), disp, SLOT(fftData(QByteArray)), Qt::QueuedConnection);
    connect(sdrr2t2, SIGNAL(controlCommand(int,int,int)), this, SLOT(controlCommand(int,int,int)));

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

    for (int i=CMD_NONE+1;i<CMD_INIT_END;i++) 
        controlCommand(SRC_CTL, i, conf->get(i), true);
    initReady = true;

    readServer();

    sdr->setTXRate(conf->get(CMD_SAMPLE_RATE));
    sdr->setSampleRate(conf->get(CMD_SAMPLE_RATE));
    sdr->setActive(true);
    sdr->setAudioOff(false);
    sdr->startRX();
    setMode();

    serverListUpdateTimer->start(1000*120); // 2 min reread server
}

Control::~Control() {
    writeSettings();
    audio->terminate();
    audio->wait();
    sdr->terminate();
    sdr->wait();
    delete audio;
    delete sdr;
    delete disp;
    delete serverListUpdateTimer;
#ifdef UNIX
    //keyReader->restore();
    //keyReader->terminate();
    //keyReader->wait();
    //delete keyReader;
#endif
    delete settings;
    delete conf;
}

void Control::cleanup() {
    delete this;
}

void Control::timeout() {
    readServer();
    serverListUpdateTimer->start(1000*120); // 2 min reread server
}

void Control::readServer() {
    QString serverUrl;

    if (!manager)
        return;

    if (qtRadioMode)
        serverUrl = getSettings(settings, "General/serverUrl", "http://napan.com/qtradio/qtradiolist.pl");
    else
        serverUrl = getSettings(settings, "General/serverUrlQtRadio", "http://dl2stg.de/r2t2reg/r2t2.txt");
    qDebug() << "read r2t2 from " << serverUrl;
    manager->get(QNetworkRequest(QUrl(serverUrl)));
}

void Control::replyFinished(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << reply->error(); 
        return;
    }

    servers.clear();
    while(1) {
        QByteArray a=reply->readLine();
        if (a.size()==0)
            break;
        servers.append(QString(a).split('~'));
    }; 

    QList<QStringList>::iterator i;
    QStringList calls;
    QStringList serverInfo;
    calls << "local"; 
    serverInfo << QString("local ip: ") + r2t2IP;
    for (i=servers.begin(); i!=servers.end(); ++i) {
        // qDebug() << *i;
        // qDebug() << "CALL:" << calls;
        calls << (*i)[0].left(1) + ":" + (*i)[1];
        serverInfo << QString("%1 | Call: %2 | Location: %3 | Band: %4 | Rig: %5 | Ant: %6").arg((*i)[0]).arg((*i)[1]).arg((*i)[2]).arg((*i)[3]).arg((*i)[4]).arg((*i)[5]);
    }
    //qDebug() << calls;
    //qDebug() << serverInfo;
    disp->setServers(calls, serverInfo);
}

void Control::controlCommand(int src, int cmd, int par, bool initial) {
    PDEBUG(MSG1, "%s -> Control: %s, Par: %i",SrcString[src], CmdString[cmd],par);
    if (!initial && (cmd < CMD_INIT_END) &&  (conf->get(cmd) == par))
        return;

    if (cmd < CMD_INIT_END)
        conf->set(cmd, par);

    if (src != SRC_DISP)
        emit displaySet(SRC_CTL, cmd, par);

    switch ((CtlCmd)cmd) {
        case CMD_EXIT:	// CTL_C
            sdr->setPtt(false);	
            sdr->setNBLevel(255);	
            QCoreApplication::exit();
            break;
        case CMD_RX_FREQ:
            sdr->setRXFreq(conf->get(CMD_RX_FREQ));	
            break;
        case CMD_TX_FREQ:
            sdr->setTXFreq(conf->get(CMD_TX_FREQ));	
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
                sdr->setPtt(conf->get(CMD_TX));	
            audio->setTX(conf->get(CMD_TX));
            break;
        case CMD_VOLUME:
            audio->setVolume(conf->get(CMD_VOLUME));
            break;
        case CMD_MIC:
            audio->setMic(conf->get(CMD_MIC));
            break;
        case CMD_PREAMP:
            if (src != SRC_SDR) 
                sdr->setAttenuator(conf->get(CMD_PREAMP));	
            break;
        case CMD_ANT:
            sdr->setAnt(conf->get(CMD_ANT));	
            break;
        case CMD_AGC:
            sdr->setAGC(conf->get(CMD_AGC));
            break;
        case CMD_TX_POWER:
            sdr->setTXLevel(conf->get(CMD_TX_POWER));
            break;
        case CMD_FFT_SAMPLE_RATE:
            sdr->setFFTRate(conf->get(CMD_FFT_SAMPLE_RATE));
            break;
        case CMD_GAIN:
            if (conf->get(CMD_GAIN)==0)
                sdr->setGain(0);
            else
                sdr->setGain(exp(conf->get(CMD_GAIN)/5));
            break;
        case CMD_FREQ_STEP_UP:
            conf->set(CMD_RX_FREQ, conf->get(CMD_RX_FREQ) + conf->get(CMD_STEP));
            sdr->setRXFreq(conf->get(CMD_RX_FREQ));	
            break;
        case CMD_FREQ_STEP_DOWN:
            conf->set(CMD_RX_FREQ, conf->get(CMD_RX_FREQ) - conf->get(CMD_STEP));
            sdr->setRXFreq(conf->get(CMD_RX_FREQ));	
            break;
        case CMD_TWO_TONE_TEST:
            {
                double l1,l2;
                double f1 = getSettings(settings,"Common/testToneFreq1",721);
                double f2 = getSettings(settings,"Common/testToneFreq2",1230);
                double f3 = getSettings(settings,"Common/testToneFreqSingle",1041);
                switch (conf->get(CMD_TWO_TONE_TEST)) {
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
                sdr->setToneTest(conf->get(CMD_TWO_TONE_TEST)>0, f1,l1,f2,l2);
            }
            break;
        case CMD_TX_DELAY:
            sdr->setTxDelay(conf->get(CMD_TX_DELAY));
            break;
        case CMD_NB_LEVEL:
            sdr->setNBLevel(conf->get(CMD_NB_LEVEL));
            break;
        case CMD_SAMPLE_RATE:
            sdr->setSampleRate(conf->get(CMD_SAMPLE_RATE));
            break;
        case CMD_NOTCH:
            sdr->setNotch(conf->get(CMD_NOTCH));
            break;
        case CMD_SQUELCH:
            sdr->setSquelch(conf->get(CMD_SQUELCH));
            break;
        case CMD_FFT_SIZE:
        case CMD_FFT_TIME:
            sdr->setFFT(conf->get(CMD_FFT_TIME), conf->get(CMD_FFT_SIZE));
            break;
        case CMD_LAYOUT_CHANGED:
            if (initReady) 
                for (int i=CMD_NONE+1;i<CMD_INIT_END;i++) 
                    controlCommand(SRC_CTL, i, conf->get(i), true);
            break;
        case CMD_AUDIO_COMP:
            sdr->setComp(conf->get(CMD_AUDIO_COMP));
            break;
        case CMD_PRESEL:
            sdr->selectPresel(conf->get(CMD_PRESEL));
            break;
        case CMD_CONNECT_SERVER:
            if (par < 0)
                break;
            if (par == 0) {
                sdr->setServer(r2t2IP, qtRadioMode ? QT_SERVER_PORT : R2T2_SERVER_PORT);
                break;
            }
            par -= 1;
            if (servers.size() > par) {
                sdr->setServer(servers[par][7],servers[par][8].toInt());
            } else {
                sdr->setServer("0.0.0.0",0);
            }
            break;
        case CMD_CONNECT:
            if (src == SRC_SDR) 
                serverListUpdateTimer->start(2000); // read server afer 2s
            else
                sdr->connectServer(par>0);
            break;
        case CMD_QTRADIO_MODE:
            if (par == qtRadioMode)
                return;
            qtRadioMode = par;
            sdr->connectServer(0);
            if (qtRadioMode) 
                sdr = sdrqt; 
            else
                sdr = sdrr2t2;
            for (int i=CMD_NONE+1;i<CMD_INIT_END;i++) 
                controlCommand(SRC_CTL, i, conf->get(i), true);
            readServer();
            break;
        case CMD_QTRADIO_RX:
            sdr->setRx(par);
            break;
        case CMD_NOISE_FILTER:
            sdr->setNoiseFilter(par);
            break;
        case CMD_IN_LEVEL:
        case CMD_TXPOWER_PEEK_LEVEL:
        case CMD_TXPOWER_AV_LEVEL:
        case CMD_STEP:
        case CMD_SWR:
        case CMD_DISP_MODE:
        case CMD_BAND:
        case CMD_ADC_OV:
        case CMD_TX_RX:
        case CMD_DISP_COLOR:
        case CMD_CONNECT_INFO:
        case CMD_MENU:
        case CMD_RSSI:
        case CMD_FILTER_TX_LO:
        case CMD_FILTER_TX_HI:
        case CMD_SMETER_MODE:
        case CMD_WATERFALL_MAX:
        case CMD_WATERFALL_MIN:
        case CMD_INPUT_FREQ:
        case CMD_NUMBER:
            break;
        default:
            assert(0);
            PDEBUG(ERR1, "Control: unhandled command: %s, par %i",CmdString[cmd],par);
    }
}

void Control::setMode() {
    sdr->setMode(conf->get(CMD_MODE));
    switch(conf->get(CMD_MODE)) {
        case MODE_LSB:
            sdr->setRXFreq(conf->get(CMD_RX_FREQ));	
            sdr->setFilter(-conf->get(CMD_FILTER_RX_HI), -conf->get(CMD_FILTER_RX_LO));
            break;
        case MODE_USB:
            sdr->setRXFreq(conf->get(CMD_RX_FREQ));	
            sdr->setFilter(conf->get(CMD_FILTER_RX_LO), conf->get(CMD_FILTER_RX_HI));
            break;
        case MODE_CW:
            sdr->setRXFreq(conf->get(CMD_RX_FREQ));	
            sdr->setFilter(-conf->get(CMD_FILTER_RX_LO)/2, conf->get(CMD_FILTER_RX_LO)/2);
            break;
        case MODE_DSB:
        case MODE_AM:
        case MODE_FM:
            sdr->setRXFreq(conf->get(CMD_RX_FREQ));	
            sdr->setFilter(-conf->get(CMD_FILTER_RX_HI), conf->get(CMD_FILTER_RX_HI));
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
    conf->read();

}

void Control::writeSettings() {
    settings->beginGroup("Version");
    settings->setValue("ver", VERSION);
    settings->setValue("verint", VERSION_INT);
    settings->endGroup();
    conf->save();
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
                datagram.append(QString("f %1\n").arg(conf->get(CMD_RX_FREQ),12));
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
