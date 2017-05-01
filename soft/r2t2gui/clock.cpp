#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QTimer>
#include <QDateTime>
#include "clock.h"
#include "lib.h"

Clock::Clock(QSettings *settings, int x, int y) : settings(settings), xSize(x), ySize(y) {
	//setFlags(QGraphicsItem::ItemIsMovable);
	digits = 8;
	xStep = xSize/digits;
	xOff = xStep/5+1;
	colorClockName = QColor(getSettings(settings,"display/colorClockName","#0080e0"));
	colorClockValue = QColor(getSettings(settings,"display/colorClockValue","#00f000"));
	colorClockFrame = QColor(getSettings(settings,"display/colorClockFrame","#000000"));
	colorClockBackground = QColor(getSettings(settings,"display/colorClockBackground","#102010"));
	timer = new QTimer();
	connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
	timer->start(1000);
}

Clock::~Clock() {
}

void Clock::setSize(int x, int y) {
    xSize = x; 
    ySize = y;
	xStep = xSize/digits;
	xOff = xStep/5+1;
}

void Clock::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
	QDateTime dt = QDateTime::currentDateTime();
	QString s = QString("%1:%2:%3").arg(dt.time().hour()).arg(dt.time().minute(),2,10,QChar('0')).arg(dt.time().second(),2,10,QChar('0'));
    QFont font("Monospace");
    font.setPixelSize(ySize*4/5);
	painter->setFont(font);
	QFontMetrics fm = painter->fontMetrics();
//	QPoint center = QPoint(( xSize-fm.width(s))/2, ( ySize+fm.height())/2 );

	painter->setPen(colorClockFrame);
	painter->setBrush(colorClockBackground);
	painter->drawRect(0,0,xSize, ySize);
	painter->setPen(colorClockValue);

	int yPos = ySize/2+fm.height()/3;
	for (int i=0;i<s.size();i++) {
		painter->drawText(xOff + i*xStep, yPos, s.mid(i,1));
	}
	painter->setPen(colorClockName);
}

QRectF Clock::boundingRect() const {
    return QRectF(0, 0, xSize, ySize);
}

void Clock::timeout() {
	update();
}
