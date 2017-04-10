#include <QGraphicsItem>
#include <QSettings>

class Analog;

struct analog_t {
	Analog *analog;
	QString name;
	int min;
	int max;
	int pos;
	int cmd;
	int xPos;
	int yPos;
	int xSize;
	int ySize;
//	int id;
};


class Analog: public QObject, public QGraphicsItem  {
		Q_OBJECT
		Q_INTERFACES(QGraphicsItem)
	public:
		Analog(QSettings *settings, analog_t*);
		~Analog();
		QRectF boundingRect() const;
		void setVal(int);
		int getVal();

    signals:
		void changed(int, int);

	private:
		QSettings *settings;
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
		QPixmap *analogBackground;
		analog_t *a;
		QColor colorAnalogBackground1;
		QColor colorAnalogBackground2;
		QColor colorAnalogSlider;
		QColor colorAnalogName;
		
		void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
		void mousePressEvent(QGraphicsSceneMouseEvent *event);
		void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
};
