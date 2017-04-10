#include <QGraphicsItem>
#include <QSettings>
#include "sdrgraphicsitem.h"

class Clock: public QObject, public SdrGraphicsItem  {
		Q_OBJECT
		//Q_INTERFACES(QGraphicsItem)
	public:
		Clock(QSettings*, int, int);
		~Clock();
		QRectF boundingRect() const;
		void setSize(int, int);

	private slots:
		void timeout();
	private:
		QSettings *settings;
		QTimer *timer;
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
		int xSize,ySize;
		int digits;
		int xStep,xOff;
		QString label;
		QColor colorClockName, colorClockValue;
		QColor colorClockFrame, colorClockBackground;
		
};
