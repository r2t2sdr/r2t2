#include <QGraphicsItem>
#include <QSettings>
#include "sdrgraphicsitem.h"

#define SMETER_RX		0
#define SMETER_LEVEL	1
#define SMETER_POWER	2
#define SMETER_SWR		3

class SMeter: public QObject, public SdrGraphicsItem  {
	Q_OBJECT
	//Q_INTERFACES(SdrGraphicsItem)
    public:
	SMeter(QSettings*);
	~SMeter();
	QRectF boundingRect() const;
	void setRSSIVal(double);
	void setPowerPeekVal(double);
	void setPowerAvVal(double);
	void setSWRVal(double);
	void setMode(int);
	void setOverflow(bool);
	void setLevel(int);
	void setSize(int,int);

	signals:
	void pressed(int, int);

	private:
	QPixmap *backGroundPixmap;
	QSettings *settings;
	QColor colorSmeterBackground,colorSmeterBar,colorSmeterBarValue,colorSmeterBarRed;
	QColor colorSmeterTextLo,colorSmeterTextHi,colorSmeterText;
	QColor colorPowerPeekBarValue,colorPowerAvBarValue;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	int xSize,ySize;
	double rssi,level,powerAv,powerPeek,swr;
	int mode;
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
};
