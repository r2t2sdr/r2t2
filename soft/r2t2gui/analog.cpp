#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include "analog.h"
#include "lib.h"

Analog::Analog(QSettings *settings, analog_t *analog) : settings(settings) {
	colorAnalogBackground1 = QColor(getSettings(settings,"display/colorAnalogBackground1","#1e1e1e"));
	colorAnalogBackground2 = QColor(getSettings(settings,"display/colorAnalogBackground2","#414141"));
	colorAnalogSlider = QColor(getSettings(settings,"display/colorAnalogSlider","#00a0a0"));
	colorAnalogName = QColor(getSettings(settings,"display/colorAnalogName","#0080e0"));
	//setFlags(QGraphicsItem::ItemIsMovable);
	setFlags(QGraphicsItem::ItemClipsToShape);
	a = analog;
	analogBackground = new QPixmap(a->xSize, a->ySize);
}

Analog::~Analog() {
	delete analogBackground;
}


void Analog::setVal(int v) {
	a->pos = v;
	update();
}

int Analog::getVal() {
	return a->pos;
}

void Analog::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
	painter->setPen(Qt::black);

	QLinearGradient linearGrad(QPointF(a->xSize/4, a->ySize-(a->pos - a->min)-60), QPointF(a->xSize*3/4, a->ySize-a->pos+60));
	QPainter painterBg(analogBackground);
	linearGrad.setColorAt(0, colorAnalogBackground1);
	linearGrad.setColorAt(1, colorAnalogBackground2);
	painterBg.setBrush(QBrush(linearGrad));
	painterBg.drawRect(analogBackground->rect());
	painter->drawPixmap(0,0, *analogBackground);

	painter->setPen(Qt::black);
	painter->setBrush(colorAnalogSlider);
	painter->drawRect(1, a->ySize-((a->pos - a->min)*a->ySize/(a->max-a->min) + 3), a->xSize-2, 6);

    QFont font("Monospace");
    font.setPixelSize(a->ySize/6);
	painter->setFont(font);
	QFontMetrics fm = painter->fontMetrics();
	painter->setPen(colorAnalogName);
	painter->drawText((a->xSize-fm.width(a->name))/2, a->ySize-8, a->name);
}

QRectF Analog::boundingRect() const {
    return QRectF(0, 0, a->xSize, a->ySize);
}

void Analog::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	a->pos = checkRange((a->ySize - event->pos().y())*(a->max - a->min)/a->ySize + a->min, a->min, a->max);
	emit changed(a->cmd, a->pos);
	update();
}

void Analog::mouseReleaseEvent(QGraphicsSceneMouseEvent * /*event*/) {
	update();
}

void Analog::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
	a->pos = checkRange((a->ySize - event->pos().y())*(a->max - a->min)/a->ySize + a->min, a->min, a->max);
	emit changed(a->cmd, a->pos);
	update();
}
