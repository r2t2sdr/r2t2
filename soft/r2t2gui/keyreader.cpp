#include <QDebug>
#include "keyreader.h"
#include "config.h"

KeyReader::KeyReader() {
	tcgetattr (0, &saveAttr);
	tcgetattr(0, &attr);
//	cfmakeraw(&attr);
	tcsetattr(0, TCSANOW, &attr);
}

KeyReader::~KeyReader() {
}

void KeyReader::restore() {
	tcsetattr(0, TCSANOW, &saveAttr);
}

void KeyReader::run() {
	int key;

	while (true) {
		kbhit();
		key = getchar();
		//qDebug() << "KEY:" << key;
		switch (key) {
			case '1':
				emit controlCommand(SRC_KEY, CMD_FREQ_STEP_UP, 0);
				break;
			case '2':
				emit controlCommand(SRC_KEY, CMD_FREQ_STEP_DOWN, 0);
				break;
		}
	}
}

int KeyReader::kbhit() {
	struct timeval tv = { 1,0 };
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(0, &fds);
	return select(1, &fds, NULL, NULL, &tv);
}

