#include <QGraphicsItem>
#include <QSettings>
#include <assert.h>
#include "config.h"

#ifndef  __TEXT_BUTTON_H__
#define  __TEXT_BUTTON_H__
class TextButton;

struct text_button_t {
	TextButton *button;
	QStringList buttonText;
	int buttonValue[MAX_ENTRY];
	QString name;
	int pos;
	int cmd;
	int xPos;
	int yPos;
	int xSize;
	int ySize;
};


class TextButton: public QObject, public QGraphicsItem  {
		Q_OBJECT
		Q_INTERFACES(QGraphicsItem)
	public:
		TextButton(QSettings *settings, text_button_t*);
		~TextButton();
		QRectF boundingRect() const;
		void setVal(int);
		int getVal();
		void doUpdate();
		void setCmd(int cmd,QString name=QString());
		void pressed(bool left, bool autoUp);

    signals:
		void pressed(int, int);

	private:
		QSettings *settings;
		QPixmap *buttonBackground;
		QPixmap *buttonBackgroundP;
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
		text_button_t *b;
		bool isPressed;
		QColor colorButtonFrame, colorButtonText, colorButtonName, colorButtonLedOn, colorButtonLedOff;
		QColor colorButtonPosOn, colorButtonPosOff;
		
		void mousePressEvent(QGraphicsSceneMouseEvent *event);
		void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
};
#endif
