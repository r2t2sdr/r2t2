#include "smeter.h"

#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <math.h>
#include <assert.h>
#include "lib.h"

SMeter::SMeter(QSettings *settings) : settings(settings) {
	//setFlags(QGraphicsItem::ItemIsMovable);
	setFlags(QGraphicsItem::ItemClipsToShape);
    xSize = 100;
    ySize = 100;
	rssi = 0;
	powerAv = 0;
	powerPeek = 0;
	swr = 1;
	backGroundPixmap = new QPixmap(xSize, ySize);
	setMode(SMETER_RX);
	colorSmeterBackground = QColor(getSettings(settings,"display/colorSmeterBackground","#3c3c3c"));
	colorSmeterBar = QColor(getSettings(settings,"display/colorSmeterBar","#d0d000"));
	colorSmeterBarValue = QColor(getSettings(settings,"display/colorSmeterBarValue","#d08000"));
	colorSmeterBarRed = QColor(getSettings(settings,"display/colorSmeterBarRed","#ff0000"));
	colorSmeterTextLo = QColor(getSettings(settings,"display/colorSmeterTextLo","#ffffff"));
	colorSmeterTextHi = QColor(getSettings(settings,"display/colorSmeterTextHi","#ff0000"));
	colorSmeterText = QColor(getSettings(settings,"display/colorSmeterText","#ffffff"));
	colorPowerPeekBarValue = QColor(getSettings(settings,"display/colorPowerPeekBarValue","#d08000"));
	colorPowerAvBarValue = QColor(getSettings(settings,"display/colorPowerAvBarValue","#f0f000"));

    setCacheMode(QGraphicsItem::ItemCoordinateCache);
}

SMeter::~SMeter() {
	delete backGroundPixmap;
}

void SMeter::setSize(int x, int y) {
    xSize = x;
    ySize = y;
	delete backGroundPixmap;
	backGroundPixmap = new QPixmap(x, y);
	setMode(mode);
}

void SMeter::setOverflow(bool ov) {
	if (ov) {
		colorSmeterBackground = QColor("#f03c3c");
	} else {
		settings =  new QSettings("sdr","r2t2");
		colorSmeterBackground = QColor(getSettings(settings,"display/colorSmeterBackground","#3c3c3c"));
	}
	setMode(mode);
	update();
}

void SMeter::setMode(int m) {
	int i;
	mode = m;
	backGroundPixmap->fill(Qt::black);
	QPainter painter(backGroundPixmap);
	painter.setPen(Qt::black);
	painter.setBrush(colorSmeterBackground);
	painter.drawRect(0,0,xSize-1,ySize-1);
    QFont font("Monospace");
    font.setPixelSize(ySize/5);
	painter.setFont(font);

	switch (mode) {
		case SMETER_RX:
            if (0)
			{
				int xStep = xSize/20;
				int xStep1 = xStep*10/6;

				painter.setPen(colorSmeterTextLo);
				for (i=1;i<10;i++) {
					painter.drawLine(i*xStep,ySize/2,i*xStep,ySize-2);
					if (i&1)
						painter.drawText(i*xStep-1-xStep/2,ySize/2-4,QString("%1").arg(i));
				}
				painter.setPen(colorSmeterTextHi);
				for (i=0;i<6;i++) {
					painter.drawLine(10*xStep+i*xStep1,ySize/2,10*xStep+i*xStep1,ySize-2);
					if (i&1)
						painter.drawText(10*xStep-1+i*xStep1-xStep1/2,ySize/2-4,QString("+%1").arg(i*10+10));

				}
			}
			break;
		case SMETER_POWER:
			{
#if 0
				int xStep = xSize/5;
				double pwr = 0.01;

				painter.setPen(colorSmeterTextLo);
				for (i=1;i<5;i++) {
					painter.drawLine(i*xStep,ySize/2,i*xStep,ySize-2);
					painter.drawText(i*xStep+1-xStep/2,ySize/2-4,QString("%1").arg(pwr));
					pwr *= 10;
				}
#endif
				int xStep = xSize/10;

				painter.setPen(colorSmeterTextLo);
				for (i=1;i<10;i+=2) {
					painter.drawLine(i*xStep,ySize/2,i*xStep,ySize-2);
					painter.drawText(i*xStep+1-xStep/2,ySize/2-4,QString("%1").arg(i*10));
				}
			}
			break;
		case SMETER_LEVEL:
			{
				int xStep = xSize/10;

				painter.setPen(colorSmeterTextLo);
				for (i=1;i<10;i+=2) {
					painter.drawLine(i*xStep,ySize/2,i*xStep,ySize-2);
					painter.drawText(i*xStep+1-xStep/2,ySize/2-4,QString("%1").arg(i*10));
				}
			}
			break;
		case SMETER_SWR:
			break;
		default:
			assert(0);
	}
}

void SMeter::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
	char s[64];
	painter->setPen(Qt::black);
	painter->drawPixmap(0,0,*backGroundPixmap);
    QFont font("Monospace");
    font.setPixelSize(ySize/5);
	painter->setFont(font);

	switch (mode) {
		case SMETER_RX:
            if (1)
			{
				int xStep = xSize/20;
				int xStep1 = xStep*10/6;

				painter->setPen(colorSmeterTextLo);
				for (int i=1;i<10;i++) {
					painter->drawLine(i*xStep,ySize/2,i*xStep,ySize-2);
					if (i&1)
						painter->drawText(i*xStep-1-xStep/2,ySize/2-4,QString("%1").arg(i));
				}
				painter->setPen(colorSmeterTextHi);
				for (int i=0;i<6;i++) {
					painter->drawLine(10*xStep+i*xStep1,ySize/2,10*xStep+i*xStep1,ySize-2);
					if (i&1)
						painter->drawText(10*xStep-1+i*xStep1-xStep1/2,ySize/2-4,QString("+%1").arg(i*10+10));

				}
			}
			painter->setPen(colorSmeterText);
			sprintf(s,"%3.0f dBm",rssi);
			painter->drawText(10,ySize/4-1,QString(s));
			painter->setPen(Qt::black);
			painter->setBrush(colorSmeterBar);
			painter->drawRect(1,ySize*3/4,(127+rssi)*xSize/20/6,ySize/4-1);
			break;
		case SMETER_LEVEL:
			painter->setPen(colorSmeterText);
			painter->setPen(Qt::black);
			painter->setBrush(colorSmeterBarValue);
			if (level>=32768)
				painter->setBrush(colorSmeterBarRed);
			painter->drawRect(1,ySize*3/4,level*xSize/0x8000,ySize/4-1);
			break;
		case SMETER_POWER:
			painter->setPen(colorSmeterText);
			painter->setPen(Qt::black);
			painter->setBrush(colorPowerPeekBarValue);
			painter->drawRect(1,ySize*3/4,powerPeek*xSize,ySize/4-1);
			painter->setBrush(colorPowerAvBarValue);
			painter->drawRect(1,ySize*3/5,powerAv*xSize,ySize/4-1);
			break;
		default:
			;
	}
    update();
}

QRectF SMeter::boundingRect() const {
    return QRectF(0, 0, xSize, ySize);
}

void SMeter::setRSSIVal(double v) {
	rssi = v;
	if (mode == SMETER_RX)
		update();
}

void SMeter::setPowerAvVal(double v) {
	powerAv = v;
	if (mode == SMETER_POWER)
		update();
}

void SMeter::setPowerPeekVal(double v) {
	powerPeek = v;
	if (mode == SMETER_POWER)
		update();
}

void SMeter::setSWRVal(double v) {
	swr = v;
	if (mode == SMETER_SWR)
		update();
}

void SMeter::setLevel(int v) {
	level = v;
	if (mode == SMETER_LEVEL)
		update();
}

void SMeter::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	emit pressed(event->pos().x(),event->pos().y());
}




