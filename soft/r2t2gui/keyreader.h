#ifndef _KEYREADER_H_
#define _KEYREADER_H_

#include <QThread>
#include <termios.h>
#include "config.h"

class KeyReader : public QThread {
	Q_OBJECT
	public: 
		KeyReader();
		~KeyReader();
		void run();
		void restore();

	signals:
		void controlCommand(int,int,int);

	private:
		int kbhit();
		struct termios attr,saveAttr;
	
};

#endif
