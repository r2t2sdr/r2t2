#include <QGraphicsItem>
#include <QSettings>
#include <assert.h>

#ifndef  __LABEL_H__
#define  __LABEL_H__

class Label;

struct label_t {
	Label *label;
	QString name;
	int cmd;
	int val;
	int xPos;
	int yPos;
	int xSize;
	int ySize;
};

class Label: public QObject, public QGraphicsItem  {
		Q_OBJECT
		Q_INTERFACES(QGraphicsItem)
	public:
		Label(QSettings *settings, label_t*);
		~Label();
		QRectF boundingRect() const;
		void setVal(int);
		void doUpdate();

    signals:
		void pressed(int, int);

	private:
		label_t *l;
		QSettings *settings;
		QPixmap *buttonBackground;
		QColor colorLabelText,colorLabelName;
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};
#endif
