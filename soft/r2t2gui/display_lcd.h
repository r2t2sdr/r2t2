#ifndef _DISPLY_LCD_H_
#define _DISPLAY_LCD_H_

#include "config.h"
#include "display_base.h"

class Display_lcd : public Display_base  {
	Q_OBJECT
	public: 
		Display_lcd();
		~Display_lcd();

public slots:
		void displaySet (int, int,int);
		void fftData(QByteArray);
		void start();
	signals:
		void command(int, int, int);

	private:
		void setRXFreq(int);
		void setTXFreq(int);
		void setMode(int);
		int fd;
		char s[32];
};

#endif
