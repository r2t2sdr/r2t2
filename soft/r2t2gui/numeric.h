#include <QGraphicsItem>
#include <QSettings>
#include "sdrgraphicsitem.h"

class Numeric: public QObject, public SdrGraphicsItem  {
		Q_OBJECT
		//Q_INTERFACES(QGraphicsItem)
	public:
		Numeric(QSettings *settings, QString, int,int,int,int, int, int);
		~Numeric();
		QRectF boundingRect() const;
		void setVal(int);
		int getVal();
		void setSize(int, int);
		void setOffset(int);

    signals:
		void changed(int);

	private:
		QSettings *settings;
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
		int xSize,ySize;
		int val;	
		int digits;
		int xStep,xOff;
		int steps[32];
		int min,max;
		int dispOffset;
		QString label;
		QColor colorNumericName, colorNumericValue;
		QColor colorNumericFrame, colorNumericBackground;
		
		void mousePressEvent(QGraphicsSceneMouseEvent *event);
		void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
		void wheelEvent(QGraphicsSceneWheelEvent *event);
};
