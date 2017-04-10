#include <QtCore/QCoreApplication>
#include <QApplication>
#include <unistd.h>
#include "control.h"
#include "lib.h"
#include "math.h"

bool touchscreen = false;
bool defaults = false;
int layOut=0;

void help(char *prog)
{
    printf("usage: %s [-h] [-t] [-v debugLevel] [-p ip] [-a devive] [-m mixer] [-o mixer volume] [-i mixer mic]"
	    "\n\n", 
	    prog);
    printf("  -a <audiodev>          use audiodev\n");
    printf("  -d                     start with defaults\n");
    printf("  -h                     display this help\n");
    printf("  -i <mixer mic>         use mixer for mic\n");
    printf("  -m <mixerdev>          use mixerdev\n");
    printf("  -o <mixer volume>      use mixer for volume\n");
    printf("  -p <ip>                r2t2 ip\n");
    printf("  -s                     use samplerate 48k, default is 8k\n");
    printf("  -l [0|1|2]             use layOut n\n");
    printf("  -t                     touchscreen mode (fullscreen, no mouse pointer)\n");
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
	int sampleRate = DEFAULT_SAMPLE_RATE;

	while ((i = getopt(argc, argv, "+hdstp:a:l:m:i:o:v:")) != EOF) {
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
			case 's':
				sampleRate  = 48000;
				break;
			case 'd':
				defaults = true;
				break;
			case 'h':
			default:
				help(argv[0]);
				break;
		}
	}

//QCoreApplication a(argc, argv);
    QApplication a(argc, argv);
	Control *ctl = new Control(ip, audiodev, mixerdev, mixervol, mixermic, sampleRate);
	QObject::connect(&a, SIGNAL(aboutToQuit()), ctl, SLOT(cleanup()));
	a.exec();
	//delete ctl;

	return 0;
}
