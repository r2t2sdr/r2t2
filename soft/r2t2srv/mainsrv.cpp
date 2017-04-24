#include <QtCore/QCoreApplication>
#include <QtWidgets/QApplication>
#include <unistd.h>
#include "r2t2srv.h"

static uint32_t clk = 122880000;
static int port = R2T2_PORT;
extern int debugLevel;

void usage(char *prog)
{
    printf("usage: %s [-h] [-t] [-v debugLevel] [-p ip]\n\n", 
	    prog);
    printf("  -v <debugLevel (0..4)> debug output\n");
    printf("  -p <port>              udp port\n");

    exit(0);
}

int main(int argc, char *argv[])
{
	int i;

	qDebug() << "starting r2t2srv version " << VERSION;

	if (geteuid () != 0) {
		printf ("run with root permissions !\n");
		return -1;
	}

	while ((i = getopt(argc, argv, "+hp:v:c:p:f")) != EOF) {
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
			case 'h':
			default:
				usage(argv[0]);
				break;
		}
	}

QCoreApplication a(argc, argv);
	new R2T2Srv(clk, port);
	a.exec();
	return 0;
}
