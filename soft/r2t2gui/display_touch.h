#ifndef DISPLAY_TOUCH_H
#define DISPLAY_TOUCH_H

#include <QtGui>
#include <QSettings>
#include "display_base.h"
#include "sdrgraphicsitem.h"
#include "smeter.h"
#include "textbutton.h"
#include "numeric.h"
#include "fftGraph.h"
#include "filterGraph.h"
#include "analog.h"
#include "label.h"
#include "clock.h"

#define ITEM_SMTR			0
#define ITEM_RXFREQ			1
#define ITEM_TXFREQ			2
#define ITEM_INPUT			3
#define ITEM_FILTERGRAPH	4
#define ITEM_FFTGRAPH		5
#define ITEM_CLOCK			6
#define ITEM_MAX			7

typedef struct {
	QString name;
	int mode;
	int rxFreq;
	int txFreq;
	int filterLo;
	int filterHi;
	int preamp;
	int ant;
} MemSet;

typedef struct {
	char name[128];
	int par;
} Entry;

extern Entry entry[CMD_LAST][MAX_ENTRY];

typedef struct MenuEntry {
	Entry *entry;
	int cmd;
	MenuEntry (*next)[MAX_MENU];
} MenuEntry;

typedef struct  {
	SdrGraphicsItem *it;
	int xSize;
	int ySize;
	int xPos;
	int yPos;
} Items;

namespace Ui {
class Display_touch;
}

class Display_touch : public Display_base
{
	Q_OBJECT

	public:
		explicit Display_touch(QSettings *settings, QWidget *parent=0);
		~Display_touch();

	public slots:
		void displaySet(int, int, int);
        void setServers(QStringList, QStringList);
		void fftData(QByteArray);
		void start();
	signals:
		void command (int,int,int);

	private slots:
		void analogChanged(int,int);
		void buttonPressed(int,int);
		void freqChanged(int);
		void readSettings();
		void setBand(int);
		void setFFTSize(int);
		void setMemory(int);
		void smtrPressed(int,int);
		void storeBand();
		void storeMemory(int);
		void updateOn();
		void writeSettings();
		void setFreq(int);
		void setTXFreq(int);
		void keyPressEvent(QKeyEvent *event); 
		void resizeEvent(QResizeEvent* event);
		void resizeDisp(QSize size);
	private:
		void setLayout(int);

		QSettings *settings;
		FFTGraph *fftGraph;
		FilterGraph *filterGraph;
		MemSet memBand[MAX_BAND];
		MemSet mem[MAX_MEM];
		Numeric *rxfreq, *txfreq;
		Numeric *input;
		Clock *clock;
		QGraphicsScene *scene;
		QTimer *timer;
		SMeter *smtr;
		Ui::Display_touch *ui;
		analog_t analog[NBUTTON];
		label_t label[NBUTTON];
		text_button_t button[NBUTTON];
		int cmdStore[CMD_LAST];
		bool update;
		double buf[MAX_FFT];
		double cicCorr[MAX_FFT+1];
		int fftSampRate; 
		int fftSize;
		int getVal(int cmd);
		void setVal(int cmd, int val);
		int rxFreq;
		int inputVal;
		bool inputMode;
		bool txrx;
		int step;
		int txFreq;
		std::vector<double> window;
		void sendCmd(int,int);
		int menuStartIdx;
		MenuEntry (*curMenu)[MAX_MENU];
		MenuEntry (*lastMenu)[MAX_MENU];
		Items items[ITEM_MAX];
		int div1,div2;
		int grid;
		double fftNearOffset,fftWidthOffset;
		int ptt;
		int fullScreen;
		bool initReady;
		int volume;
		int encSens, encDelta;
        bool connected;
};

#endif
