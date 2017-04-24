#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include "numeric.h"
#include "lib.h"

Numeric::Numeric(QSettings *settings, QString /*l*/, int x, int y, int v, int dig, int mi, int ma) : 
	settings(settings), xSize(x), ySize(y), val(v), digits(dig), min(mi), max(ma) {
	//setFlags(QGraphicsItem::ItemIsMovable);
	xStep = xSize/digits;
	xOff = xStep/5;
	dispOffset = 0;
	int n=1;
	for (int i=0;i<digits;i++) {
		steps[digits-i-1] = n; 
		n *= 10;
	}
	colorNumericName = QColor(getSettings(settings,"display/colorNumericName","#0080e0"));
	colorNumericValue = QColor(getSettings(settings,"display/colorNumericValue","#00f000"));
	colorNumericFrame = QColor(getSettings(settings,"display/colorNumericFrame","#000000"));
	colorNumericBackground = QColor(getSettings(settings,"display/colorNumericBackground","#102010"));
}

Numeric::~Numeric() {
}

void Numeric::setSize(int x, int y) {
    xSize = x; 
    ySize = y;
	xStep = xSize/digits;
	xOff = xStep/5-5;
}

void Numeric::setVal(int v) {
	val = v;
	update();
}

int Numeric::getVal() {
	return val;
}

void Numeric::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
	QString s = QString("%1").arg(val+dispOffset,digits);
	painter->setFont(QFont("Monospace", ySize*0.4));
	QFontMetrics fm = painter->fontMetrics();
//	QPoint center = QPoint(( xSize-fm.width(s))/2, ( ySize+fm.height())/2 );

	painter->setPen(colorNumericFrame);
	painter->setBrush(colorNumericBackground);
	painter->drawRect(0,0,xSize, ySize);
	painter->setPen(colorNumericValue);

	int yPos = ySize/2+fm.height()/3;
	for (int i=0;i<s.size();i++) {
		painter->drawText(xOff + i*xStep, yPos, s.mid(i,1));
		if (i>0 && (digits > 6) && (s.size()-i) % 3 == 0) {
			painter->setBrush(Qt::white);
			painter->drawRect(i*xStep+2,yPos, 2,2);
		}
	}
	painter->setFont(QFont("Monospace", 10));
	painter->setPen(colorNumericName);
	painter->drawText(0,ySize/2,label);
}

QRectF Numeric::boundingRect() const {
    return QRectF(0, 0, xSize, ySize);
}

void Numeric::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	int x = (event->pos().x()-xStep/3) / xStep;
	if (event->pos().y()<ySize/2)
		val += steps[x];
	else
		val -= steps[x];
	val = checkRange(val,min,max);
	emit changed(val);
	update();
}

void Numeric::wheelEvent(QGraphicsSceneWheelEvent *event) {
	int x = (event->pos().x()-xStep/3) / xStep;
	if (event->delta()>0)
		val += steps[x];
	else
		val -= steps[x];
	val = checkRange(val,min,max);
	emit changed(val);
	update();
}

void Numeric::mouseReleaseEvent(QGraphicsSceneMouseEvent * /*event*/) {
	update();
}

void Numeric::setOffset(int n) {
	dispOffset = n;
	update();
}


