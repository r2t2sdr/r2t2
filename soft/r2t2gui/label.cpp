#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include "label.h"
#include "display_touch.h"
#include "lib.h"

Label::Label(QSettings *settings, label_t *label) : settings(settings) {
	//setFlags(QGraphicsItem::ItemIsMovable);
	//setFlags(QGraphicsItem::ItemClipsToShape);
	l = label;
	buttonBackground = new QPixmap(l->xSize, l->ySize);
	QPainter painter(buttonBackground);
	QLinearGradient linearGrad(QPointF(l->xSize/2, l->ySize/4), QPointF(l->xSize/1, l->ySize*3/4));
	linearGrad.setColorAt(0, QColor(getSettings(settings,"display/colorLabelBackground1","#282828")));
	linearGrad.setColorAt(1, QColor(getSettings(settings,"display/colorLabelBackground2","#4b4b4b")));
	colorLabelText = QColor(getSettings(settings,"display/colorLabelText","#e0e0ff"));
	colorLabelName = QColor(getSettings(settings,"display/colorLabelName","#ffffff"));
	painter.setBrush(QBrush(linearGrad));
	painter.drawRect(buttonBackground->rect());
}

Label::~Label() {
	delete buttonBackground;
}

void Label::setVal(int v) {
	l->val = v;
	update();
}

void Label::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
	painter->setFont(QFont("Monospace", l->ySize*3/4-1));
	QFontMetrics fm = painter->fontMetrics();
	// QPoint center = QPoint(( l->xSize-fm.width(l->name))/2, (l->ySize+fm.height())/2-5 );

	painter->setPen(Qt::white);
	painter->setBrush(QColor(0,0,0));
	painter->drawPixmap(0,0, *buttonBackground);
	painter->setPen(colorLabelName);
	painter->drawText(2,l->ySize-2,l->name+":");
	painter->setPen(colorLabelText);
	if (l->val < MAX_MENU && entry[l->cmd][l->val].name != NULL)
		painter->drawText(fm.width(l->name+":  "), l->ySize-2, QString("%1").arg(entry[l->cmd][l->val].name));
	else
		painter->drawText(l->xSize/2-4, l->ySize-2, QString("%1").arg(l->val));
}

QRectF Label::boundingRect() const {
    return QRectF(0, 0, l->xSize, l->ySize);
}


void Label::doUpdate() {
	update();
}
