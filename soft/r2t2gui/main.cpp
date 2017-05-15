#include <QtCore/QCoreApplication>
#include <QApplication>
#include <unistd.h>
#include "control.h"
#include "lib.h"
#include "math.h"

bool defaults = false;
bool qtRadioMode = false;
#ifdef ANDROID
int layOut=0;
bool touchscreen = true;
#else
int layOut=1;
bool touchscreen = false;
#endif

void help(char *prog)
{
    printf("usage: %s [-h] [-t] [-v debugLevel] [-q] [-p ip] [-a devive] [-m mixer] [-o mixer volume] [-i mixer mic]"
	    "\n\n", 
	    prog);
    printf("  -a <audiodev>          use audiodev\n");
    printf("  -d                     start with defaults\n");
    printf("  -h                     display this help\n");
    printf("  -i <mixer mic>         use mixer for mic\n");
    printf("  -m <mixerdev>          use mixerdev\n");
    printf("  -o <mixer volume>      use mixer for volume\n");
    printf("  -p <ip>                r2t2 ip\n");
    printf("  -l [0|1]               use layout n\n");
    printf("  -t                     touchscreen mode (fullscreen, no mouse pointer)\n");
    printf("  -q                     qtradio mode\n");
    printf("  -v <debugLevel (0..4)> debug output\n");

    exit(0);
}

int main(int argc, char *argv[])
{
	int i;
	char *audiodev = NULL;
	char *mixerdev = NULL;
	char *mixermic = NULL;
	char *mixervol = NULL;
	char *ip = NULL;
	gettimeofday(&tStart, NULL);

	while ((i = getopt(argc, argv, "+hdtqp:a:l:m:i:o:v:")) != EOF) {
		switch (i) {
			case 'p':
				ip = optarg;
				break;
			case 'a':
				audiodev = optarg;
				break;
			case 'l':
				layOut = atoi(optarg);
				break;
			case 'm':
				mixerdev = optarg;
				break;
			case 'o':
				mixervol = optarg;
				break;
			case 'i':
				mixermic = optarg;
				break;
			case 'v':
				debugLevel = atoi(optarg);
				break;
			case 't':
				touchscreen = true;
				break;
			case 'd':
				defaults = true;
				break;
			case 'q':
				qtRadioMode = true;
				break;
			case 'h':
			default:
				help(argv[0]);
				break;
		}
	}

    QApplication a(argc, argv);
    qRegisterMetaType < uint32_t >("uint32_t");
	Control *ctl = new Control(ip, audiodev, mixerdev, mixervol, mixermic, DEFAULT_SAMPLE_RATE, qtRadioMode);
    QObject::connect(&a, SIGNAL(aboutToQuit()), ctl, SLOT(cleanup()));
	a.exec();

	return 0;
}
