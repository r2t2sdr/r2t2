#include <QDebug>
#include "stdint.h"
#include "display_lcd.h"
#ifdef LCD
#include "wiringPi.h"
#include "lcd.h"
#endif

Display_lcd::Display_lcd() {
#ifdef LCD
	wiringPiSetup ();
	fd = lcdInit (4, 20, 4, 11,  10, 0,1,2,3,0,0,0,0) ;
#endif
}

Display_lcd::~Display_lcd() {
}

void Display_lcd::displaySet(int /*src*/,int /*cmd*/, int /*f*/) {
}

void Display_lcd::setRXFreq(int f) {
  sprintf(s, "R %8i",f);
#ifdef LCD
  lcdPosition (fd, 0, 0) ; lcdPuts (fd, s) ;
#else
  printf ("%s\n",s);
#endif
}

void Display_lcd::setTXFreq(int f) {
  sprintf(s, "T %8i",f);
#ifdef LCD
  lcdPosition (fd,10, 0) ; lcdPuts (fd, s) ;
#else
  printf ("%s\n",s);
#endif
}

void Display_lcd::start() {
}

void Display_lcd::setMode(int mode) {
	switch (mode) {
		case MODE_LSB: 
			sprintf(s,"LSB");
			break;
		case MODE_USB: 
			sprintf(s,"USB");
			break;
		case MODE_CW: 
			sprintf(s," CW");
			break;
		case MODE_AM: 
			sprintf(s," AM");
			break;
		default:
			sprintf(s,"???");
			break;
	}
#ifdef LCD
	lcdPosition (fd, 0, 1);
	lcdPuts(fd, s);
#else
  printf ("%s\n",s);
#endif
}

void Display_lcd::fftData(QByteArray) {
}

