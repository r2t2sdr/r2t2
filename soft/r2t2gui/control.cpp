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
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)), Qt::QueuedConnection);

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

    audioThread = new QThread;

    audio = new Audio(audiodev, audiodevMixer, mixerVolume, mixerMic, conf->get(CMD_SAMPLE_RATE));
    audio->moveToThread(audioThread);
    connect(audioThread, SIGNAL(started()), audio, SLOT(init()));
    audioThread->start(QThread::TimeCriticalPriority);

    radioThread = new QThread;

    sdrqt = new SdrQtRadio(r2t2IP, QT_SERVER_PORT, 0);
    sdrqt->setSampleRate(conf->get(CMD_SAMPLE_RATE));
    sdrqt->moveToThread(radioThread);
    connect(radioThread, SIGNAL(started()), sdrqt, SLOT(init()));

    sdrr2t2 = new SdrR2T2(r2t2IP, R2T2_SERVER_PORT, 0);
    sdrr2t2->setSampleRate(conf->get(CMD_SAMPLE_RATE));
    sdrr2t2->moveToThread(radioThread);
    connect(radioThread, SIGNAL(started()), sdrr2t2, SLOT(init()));
    radioThread->start();

    if (qtRadioMode)
        sdr = sdrqt; 
    else
        sdr = sdrr2t2; 

    if (touchscreen)
        qApp->setOverrideCursor(QCursor( Qt::BlankCursor ));
    disp = new Display_touch(settings);
    disp->start();

    //connect(audio, SIGNAL(audioTX(QByteArray)), sdr, SLOT(writeR2T2(QByteArray)), Qt::QueuedConnection);

    serverListUpdateTimer = new QTimer(this);
    serverListUpdateTimer->setSingleShot(true);

#ifdef UNIX
    //keyReader = new KeyReader();

    //connect(keyReader, SIGNAL(controlCommand(int,int,int)), this, SLOT(controlCommand(int,int,int))); 
#endif
    connect(serverListUpdateTimer, SIGNAL(timeout()), this, SLOT(timeout()), Qt::QueuedConnection);
    connect(disp, SIGNAL(command(int, int, int)), this, SLOT(controlCommand(int, int, int)), Qt::QueuedConnection);
    connect(this, SIGNAL(displaySet(int, int, int)), disp, SLOT(displaySet(int, int, int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setTX(bool)), audio, SLOT(setTX(bool)), Qt::QueuedConnection);
    connect(this, SIGNAL(setMic(int)), audio, SLOT(setMic(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setVolume(int)), audio, SLOT(setVolume(int)), Qt::QueuedConnection);

    connect(sdrr2t2, SIGNAL(audioRX(QByteArray)), audio, SLOT(audioRX(QByteArray)), Qt::QueuedConnection);
    connect(sdrr2t2, SIGNAL(fftData(QByteArray)), disp, SLOT(fftData(QByteArray)), Qt::QueuedConnection);
    connect(sdrr2t2, SIGNAL(controlCommand(int,int,int)), this, SLOT(controlCommand(int,int,int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setRXFreq(uint32_t)), sdrr2t2, SLOT(setRXFreq(uint32_t)), Qt::QueuedConnection);
    connect(this, SIGNAL(setTXFreq(uint32_t)), sdrr2t2, SLOT(setTXFreq(uint32_t)), Qt::QueuedConnection);
    connect(this, SIGNAL(setSampleRate(int)), sdrr2t2, SLOT(setSampleRate(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setAnt(int)), sdrr2t2, SLOT(setAnt(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setPresel(int)), sdrr2t2, SLOT(setPresel(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setAttenuator(int)), sdrr2t2, SLOT(setAttenuator(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setTXLevel(int)), sdrr2t2, SLOT(setTXLevel(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setPtt(bool)), sdrr2t2, SLOT(setPtt(bool)), Qt::QueuedConnection);
    connect(this, SIGNAL(setTXRate(int)), sdrr2t2, SLOT(setTXRate(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setFilter(int,int)), sdrr2t2, SLOT(setFilter(int,int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setMode(int)), sdrr2t2, SLOT(setMode(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setGain(int)), sdrr2t2, SLOT(setGain(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setAGC(int)), sdrr2t2, SLOT(setAGC(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setFFT(int,int)), sdrr2t2, SLOT(setFFT(int,int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setFFTRate(int)), sdrr2t2, SLOT(setFFTRate(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setVolume(double)), sdrr2t2, SLOT(setVolume(double)), Qt::QueuedConnection);
    connect(this, SIGNAL(setMicGain(double)), sdrr2t2, SLOT(setMicGain(double)), Qt::QueuedConnection);
    connect(this, SIGNAL(setToneTest(bool,double,double,double,double)), sdrr2t2, SLOT(setToneTest(bool,double,double,double,double)), Qt::QueuedConnection);
//    connect(this, SIGNAL(startRX()), sdrr2t2, SLOT(startRX()), Qt::QueuedConnection);
//    connect(this, SIGNAL(stopRX()), sdrr2t2, SLOT(stopRX()), Qt::QueuedConnection);
    connect(this, SIGNAL(setActive(bool)), sdrr2t2, SLOT(setActive(bool)), Qt::QueuedConnection);
    connect(this, SIGNAL(setAudioOff(bool)), sdrr2t2, SLOT(setAudioOff(bool)), Qt::QueuedConnection);
    connect(this, SIGNAL(setTxDelay(int)), sdrr2t2, SLOT(setTxDelay(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setSquelch(int)), sdrr2t2, SLOT(setSquelch(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setComp(int)), sdrr2t2, SLOT(setComp(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setPresel(int)), sdrr2t2, SLOT(setPresel(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setRx(int)), sdrr2t2, SLOT(setRx(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setTX(bool)), audio, SLOT(setTX(bool)), Qt::QueuedConnection);
    connect(this, SIGNAL(setMic(int)), audio, SLOT(setMic(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setVolume(int)), audio, SLOT(setVolume(int)), Qt::QueuedConnection);

    connect(sdrqt, SIGNAL(audioRX(QByteArray)), audio, SLOT(audioRX(QByteArray)), Qt::QueuedConnection);
    connect(sdrqt, SIGNAL(fftData(QByteArray)), disp, SLOT(fftData(QByteArray)), Qt::QueuedConnection);
    connect(sdrqt, SIGNAL(controlCommand(int,int,int)), this, SLOT(controlCommand(int,int,int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setRXFreq(uint32_t)), sdrqt, SLOT(setRXFreq(uint32_t)), Qt::QueuedConnection);
    connect(this, SIGNAL(setTXFreq(uint32_t)), sdrqt, SLOT(setTXFreq(uint32_t)), Qt::QueuedConnection);
    connect(this, SIGNAL(setSampleRate(int)), sdrqt, SLOT(setSampleRate(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setAnt(int)), sdrqt, SLOT(setAnt(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setPresel(int)), sdrqt, SLOT(setPresel(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setAttenuator(int)), sdrqt, SLOT(setAttenuator(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setTXLevel(int)), sdrqt, SLOT(setTXLevel(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setPtt(bool)), sdrqt, SLOT(setPtt(bool)), Qt::QueuedConnection);
    connect(this, SIGNAL(setTXRate(int)), sdrqt, SLOT(setTXRate(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setFilter(int,int)), sdrqt, SLOT(setFilter(int,int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setMode(int)), sdrqt, SLOT(setMode(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setGain(int)), sdrqt, SLOT(setGain(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setAGC(int)), sdrqt, SLOT(setAGC(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setFFT(int,int)), sdrqt, SLOT(setFFT(int,int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setFFTRate(int)), sdrqt, SLOT(setFFTRate(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setVolume(double)), sdrqt, SLOT(setVolume(double)), Qt::QueuedConnection);
    connect(this, SIGNAL(setMicGain(double)), sdrqt, SLOT(setMicGain(double)), Qt::QueuedConnection);
    connect(this, SIGNAL(setToneTest(bool,double,double,double,double)), sdrqt, SLOT(setToneTest(bool,double,double,double,double)), Qt::QueuedConnection);
//    connect(this, SIGNAL(startRX()), sdrqt, SLOT(startRX()), Qt::QueuedConnection);
//    connect(this, SIGNAL(stopRX()), sdrqt, SLOT(stopRX()), Qt::QueuedConnection);
    connect(this, SIGNAL(setActive(bool)), sdrqt, SLOT(setActive(bool)), Qt::QueuedConnection);
    connect(this, SIGNAL(setAudioOff(bool)), sdrqt, SLOT(setAudioOff(bool)), Qt::QueuedConnection);
    connect(this, SIGNAL(setTxDelay(int)), sdrqt, SLOT(setTxDelay(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setSquelch(int)), sdrqt, SLOT(setSquelch(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setComp(int)), sdrqt, SLOT(setComp(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setPresel(int)), sdrqt, SLOT(setPresel(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setRx(int)), sdrqt, SLOT(setRx(int)), Qt::QueuedConnection);




#ifdef UNIX
    //keyReader->start();
#endif
    hamLibSocket = new QUdpSocket(this);
    hamLibSocket->bind(13222, QUdpSocket::ShareAddress);
    connect(hamLibSocket, SIGNAL(readyRead()), this, SLOT(readHamLibUDPData()));

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

    emit setTXRate(conf->get(CMD_SAMPLE_RATE));
    emit setSampleRate(conf->get(CMD_SAMPLE_RATE));
    emit setActive(true);
    emit setAudioOff(false);
    setMode();

    serverListUpdateTimer->start(1000*120); // 2 min reread server
}

Control::~Control() {
    writeSettings();
    audioThread->quit();
    radioThread->quit();
    radioThread->wait(200);
    audioThread->wait(200);
    delete audioThread;
    delete radioThread;
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
    deleteLater();
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
            emit setPtt(false);
            emit setNBLevel(255);
            QCoreApplication::exit();
            break;
        case CMD_RX_FREQ:
            emit setRXFreq(conf->get(CMD_RX_FREQ));
            break;
        case CMD_TX_FREQ:
            emit setTXFreq(conf->get(CMD_TX_FREQ));
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
                emit setPtt(conf->get(CMD_TX));
            emit setTX(conf->get(CMD_TX));
            break;
        case CMD_VOLUME:
            emit setVolume(conf->get(CMD_VOLUME));
            break;
        case CMD_MIC:
            emit setMic(conf->get(CMD_MIC));
            break;
        case CMD_PREAMP:
            if (src != SRC_SDR) 
                emit setAttenuator(conf->get(CMD_PREAMP));
            break;
        case CMD_ANT:
            emit setAnt(conf->get(CMD_ANT));
            break;
        case CMD_AGC:
            emit setAGC(conf->get(CMD_AGC));
            break;
        case CMD_TX_POWER:
            emit setTXLevel(conf->get(CMD_TX_POWER));
            break;
        case CMD_FFT_SAMPLE_RATE:
            emit setFFTRate(conf->get(CMD_FFT_SAMPLE_RATE));
            break;
        case CMD_GAIN:
            if (conf->get(CMD_GAIN)==0)
                emit setGain(0);
            else
                emit setGain(exp(conf->get(CMD_GAIN)/5));
            break;
        case CMD_FREQ_STEP_UP:
            conf->set(CMD_RX_FREQ, conf->get(CMD_RX_FREQ) + conf->get(CMD_STEP));
            emit setRXFreq(conf->get(CMD_RX_FREQ));
            break;
        case CMD_FREQ_STEP_DOWN:
            conf->set(CMD_RX_FREQ, conf->get(CMD_RX_FREQ) - conf->get(CMD_STEP));
            emit setRXFreq(conf->get(CMD_RX_FREQ));
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
                emit setToneTest(conf->get(CMD_TWO_TONE_TEST)>0, f1,l1,f2,l2);
            }
            break;
        case CMD_TX_DELAY:
            emit setTxDelay(conf->get(CMD_TX_DELAY));
            break;
        case CMD_NB_LEVEL:
            emit setNBLevel(conf->get(CMD_NB_LEVEL));
            break;
        case CMD_SAMPLE_RATE:
            emit setSampleRate(conf->get(CMD_SAMPLE_RATE));
            break;
        case CMD_NOTCH:
            emit setNotch(conf->get(CMD_NOTCH));
            break;
        case CMD_SQUELCH:
            emit setSquelch(conf->get(CMD_SQUELCH));
            break;
        case CMD_FFT_SIZE:
        case CMD_FFT_TIME:
            emit setFFT(conf->get(CMD_FFT_TIME), conf->get(CMD_FFT_SIZE));
            break;
        case CMD_LAYOUT_CHANGED:
            if (initReady) 
                for (int i=CMD_NONE+1;i<CMD_INIT_END;i++) 
                    controlCommand(SRC_CTL, i, conf->get(i), true);
            break;
        case CMD_AUDIO_COMP:
            emit setComp(conf->get(CMD_AUDIO_COMP));
            break;
        case CMD_PRESEL:
            emit selectPresel(conf->get(CMD_PRESEL));
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
                QMetaObject::invokeMethod(sdr, "connectServer", Qt::QueuedConnection, Q_ARG(bool, par>0));
            break;
        case CMD_QTRADIO_MODE:
            if (par == qtRadioMode)
                return;
            qtRadioMode = par;
            QMetaObject::invokeMethod(sdr, "connectServer", Qt::QueuedConnection, Q_ARG(bool, false));
            if (qtRadioMode)
                sdr = sdrqt; 
            else
                sdr = sdrr2t2;
            for (int i=CMD_NONE+1;i<CMD_INIT_END;i++) 
                controlCommand(SRC_CTL, i, conf->get(i), true);
            readServer();
            break;
        case CMD_QTRADIO_RX:
            emit setRx(par);
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
    emit setMode(conf->get(CMD_MODE));
    switch(conf->get(CMD_MODE)) {
        case MODE_LSB:
            emit setRXFreq(conf->get(CMD_RX_FREQ));
            emit setFilter(-conf->get(CMD_FILTER_RX_HI), -conf->get(CMD_FILTER_RX_LO));
            break;
        case MODE_USB:
            emit setRXFreq(conf->get(CMD_RX_FREQ));
            emit setFilter(conf->get(CMD_FILTER_RX_LO), conf->get(CMD_FILTER_RX_HI));
            break;
        case MODE_CW:
            emit setRXFreq(conf->get(CMD_RX_FREQ));
            emit setFilter(-conf->get(CMD_FILTER_RX_LO)/2, conf->get(CMD_FILTER_RX_LO)/2);
            break;
        case MODE_DSB:
        case MODE_AM:
        case MODE_FM:
            emit setRXFreq(conf->get(CMD_RX_FREQ));
            emit setFilter(-conf->get(CMD_FILTER_RX_HI), conf->get(CMD_FILTER_RX_HI));
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
