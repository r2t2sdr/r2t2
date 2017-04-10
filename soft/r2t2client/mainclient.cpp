#include <QtCore/QCoreApplication>
#include <QtWidgets/QApplication>
#include <QHostAddress>
#include <QString>
#include <unistd.h>
#include "r2t2client.h"
#include "r2t2qtradiolistener.h"
#include "dsp/ProcessBuffer.h"
#include "lib.h"

static uint32_t clk = 122880000;
QString addr("localhost");
int port = R2T2_PORT;
QString fileName;
ClientProto device = CLIENT_CONSOLE;
uint32_t sampleRate = 16000;
extern int debugLevel; 

void usage(char *prog)
{
    printf("usage: %s [-h] [-t] [-v debugLevel] [-p ip]\n\n", 
	    prog);
    printf("  -v <debugLevel (0..4)> debug output\n");
    printf("  -p <port>              udp port\n");
    printf("  -a <addr>              server address\n");
    printf("  -f <file>              cmd file\n");
    printf("  -d <device>            0: Console, 1: QtRadioServer, 2: dspserver\n");
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
                device = (ClientProto)atoi(optarg);
                break;
            case 'h':
            default:
                usage(argv[0]);
                break;
        }
    }

	QSettings *settings = new QSettings("/root/r2t2client.ini", QSettings::IniFormat);


    qRegisterMetaType<std::shared_ptr<ProcessBuffer>>("std::shared_ptr<ProcessBuffer>");
    QCoreApplication a(argc, argv);
    if (device == CLIENT_QTRADIO)
        new R2T2QtRadioListener(settings, addr, port, 11000, sampleRate);
    else if (device == CLIENT_QTRADIO_DSPSERVER)
        new R2T2QtRadioListener(settings, addr, port, R2T2_PORT, sampleRate);
    else
        new R2T2Client(settings, addr, port, device, NULL, fileName, sampleRate);
    a.exec();

	delete settings;

    return 0;
}
