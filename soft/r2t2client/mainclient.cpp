#include <QtCore/QCoreApplication>
#include <QtWidgets/QApplication>
#include <QHostAddress>
#include <QString>
#include <unistd.h>
#include "r2t2clientqtradio.h"
#include "r2t2clientdspqtradio.h"
#include "r2t2clientconsole.h"
#include "r2t2clientstandard.h"
#include "r2t2radiolistener.h"
#include "dsp/ProcessBuffer.h"
#include "lib.h"
#include "watchdog.h"

static uint32_t clk = 122880000;
QString addr("localhost");
int port = R2T2_SERVER_PORT;
QString fileName;
int32_t device = 0;
uint32_t sampleRate = 16000;
extern int debugLevel; 

void usage(char *prog)
{
    printf("usage: %s [-h] [-t] [-v debugLevel] [-p port] [-a addr]\n\n", 
	    prog);
    printf("  -v <debugLevel (0..4)> debug output\n");
    printf("  -p <port>              udp port\n");
    printf("  -a <addr>              server address\n");
    printf("  -f <file>              cmd file\n");
    printf("  -d <device>            0: Standard, 1: QtRadioServer, 2: dspserver, 3: Console\n");
    printf("  -s <sample rate>       QtRadio iq sample rate (default 16000)\n");

    exit(0);
}

int main(int argc, char *argv[])
{
    int i;


    while ((i = getopt(argc, argv, "+hp:v:c:p:a:f:d:s:")) != EOF) {
        switch (i) {
            case 'i':
                break;
            case 'v':
                debugLevel = atoi(optarg);
                break;
            case 'c':
                clk = atoi(optarg);
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 's':
                sampleRate = atoi(optarg);
                break;
            case 'a':
                addr = QString(optarg);
                break;
            case 'f':
                fileName = QString(optarg);
                break;
            case 'd':
                device = atoi(optarg);
                break;
            case 'h':
            default:
                usage(argv[0]);
                break;
        }
    }
    R2T2RadioListener *radioListener = NULL; 
    Watchdog *watchdog = new Watchdog();
	QSettings *settings = new QSettings("/root/r2t2client.ini", QSettings::IniFormat);

    qRegisterMetaType<std::shared_ptr<ProcessBuffer>>("std::shared_ptr<ProcessBuffer>");
    QCoreApplication a(argc, argv);
    if (device == 1) {
		qDebug() << "start r2t2client version " << VERSION << "in qtradio mode";
        radioListener = new R2T2RadioListener(settings, addr, port, QTRADIO_SERVER_PORT, sampleRate);
	} else if (device == 2) {
		qDebug() << "start r2t2client version " << VERSION << "in qtradio dsp mode";
        radioListener = new R2T2RadioListener(settings, addr, port, QTRADIO_DSP_PORT, sampleRate);
	} else if (device == 3) {
		qDebug() << "start r2t2client version " << VERSION << "in console mode";
        new R2T2ClientConsole(settings, addr, port, NULL, fileName, sampleRate);
	} else {
		qDebug() << "start r2t2client version " << VERSION << "in r2t2 mode";
        radioListener = new R2T2RadioListener(settings, addr, port, R2T2_PORT, sampleRate);
	}

    if (radioListener)
        QObject::connect(radioListener, SIGNAL(triggerWatchdog(int)), watchdog, SLOT(trigger(int)));
    watchdog->start();

    a.exec();

	delete settings;
    delete watchdog;
    if (radioListener)
        delete radioListener;

    return 0;
}
