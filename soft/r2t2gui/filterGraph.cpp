#include "filterGraph.h"

#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <math.h>
#include "config.h"
#include "lib.h"
#include "fftGraph.h"

#define MIN(a,b)  a>b ? b:a

FilterGraph::FilterGraph(QSettings *settings, int x, int y) : settings(settings), xSize(x), ySize(y) {

	setFlags(QGraphicsItem::ItemClipsToShape);

	fftSize = FFT_SIZE;
	xViewPos = (fftSize-xSize)/2;

	freqStep = 100;

	centerFreq = 7000000;
	sampleRate = 1000000;
	filterFreq = 0;

	dispMode = GRAPH_WATERFALL;

	filterPixmap = QPixmap(xSize/8, ySize);
	settingsChanged();
    setCacheMode(QGraphicsItem::ItemCoordinateCache);
}

FilterGraph::~FilterGraph() {
}

void FilterGraph::setSize(int x, int y) {
	xSize = x;
	ySize = y;
	xViewPos = (fftSize-xSize)/2;
	filterPixmap =  QPixmap(abs(filterLoMarkerPos-filterHiMarkerPos), ySize);
	filterPixmap.fill(colorSpecBackground);

    markerBackgroundPos = QRect(0, 0, xSize, 25);
    markerBackgroundColor = QColor(90, 90, 90, 160);
    markerBackground = QPixmap(xSize, 25);
    markerBackground.fill(markerBackgroundColor);
}

void FilterGraph::settingsChanged() {
	colorSpecFreq = QColor(getSettings(settings,"display/colorFreq","#ffffff"));
	colorSpecGrid = QColor(getSettings(settings,"display/colorGrid","#808080"));
	fft_scale =  2;
	shadow = getSettings(settings,"display/filterShadow",80);
	colorSpecBackground = QColor(getSettings(settings,"display/colorFilterShadowBackground","#ffffff"));
	colorSpecBackground.setAlpha(shadow);
	filterPixmap.fill(colorSpecBackground);
	update();
}

void FilterGraph::setFFTSize(int size) {
    if (size==0) 
        return;
    fftSize = size;
    xViewPos = (fftSize-xSize)/2;
    setSampleRate(sampleRate);
    update();
}

void FilterGraph::setDisplayMode(int m) {
	dispMode = m;
	update();
}

void FilterGraph::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {

    // draw dark, transparent rectangle for better reading of frequency markers
    painter->drawPixmap(markerBackgroundPos, markerBackground);
    painter->setPen(QColor(120, 120, 120));
    painter->drawLine(0, 25, xSize, 25);

    QFont font("Monospace");
    font.setPixelSize(16);
	painter->setFont(font);
	painter->drawPixmap(MIN(filterLoMarkerPos-xViewPos, filterHiMarkerPos-xViewPos),0,filterPixmap);
    painter->setPen(QColor(0,0,80));
    painter->drawText(0,ySize,QString ("SVN %1").arg(SVNREV));

	painter->setPen(colorSpecFreq);
	painter->drawLine(fftFreqMarkerPos-xViewPos, 0, fftFreqMarkerPos-xViewPos, ySize);

	int marker;
	int st = sampleRate/fftSize*200;
	int step=1000; // min 1kHz
	while(step*10 < st) step*=10;
	while(step*5 < st) step*=5;
	while(step*2 < st) step*=2;

	int f = centerFreq +freqOffset;
	int start = (f-sampleRate/2) - (f-sampleRate/2) % step ;
	for (marker=start;marker<f+sampleRate/2;marker+=step) {
		int x = (long long int)(marker - f)*(fftSize/2)/(sampleRate/2) + fftSize/2;
		painter->drawLine(x-xViewPos,0,x-xViewPos,20);
		painter->drawText(x-xViewPos+2,17,QString ("%1").arg(int((marker+500)/1000)));
	}



	switch (dispMode) {
		case GRAPH_FFT:
            fft_scale = 2;
			painter->setPen(colorSpecGrid);
			for (int i=10;i<=160;i+=10) {
				int y = i*fft_scale;
				if (y>ySize)
					break;
				painter->drawLine(0,y, xSize, y);
				painter->drawText(0,y, QString("-%1 dBm").arg(i));
			}
			break;
		case GRAPH_WATERFALL:
			break;
		case GRAPH_DUAL:
		case GRAPH_DUAL2:
            fft_scale = 1;
			painter->setPen(colorSpecGrid);

			for (int i=DUAL_OFFSET;i<=130;i+=20) {
				int y = (i-DUAL_OFFSET)*fft_scale;
				if (y>ySize)
					break;
				painter->drawLine(0,y, xSize, y);
				painter->drawText(0,y, QString("-%1 dBm").arg(i));
			}
			break;
	}
}

QRectF FilterGraph::boundingRect() const {
	return QRectF(0, 0, xSize, ySize);
}

void FilterGraph::setFreq(int f) {
	centerFreq = f;
	//update();
}

void FilterGraph::setFilter(int f, int fl, int fh, int m) {
	filterFreq = f;

	switch (m) {
		case MODE_LSB:
			filterLo = -fh;
			filterHi = -fl;
			break;
		case MODE_USB:
			filterLo = fl;
			filterHi = fh;
			break;
		case MODE_AM:
		case MODE_FM:
        case MODE_DSB:
			filterLo = -fh;
			filterHi = fh;
			break;
		case MODE_CW:
			filterLo = -fl/2;
			filterHi = fl/2;
			break;
	}
	fftFreqMarkerPos = (long long int)(filterFreq - centerFreq)*(fftSize/2)/(sampleRate/2) + fftSize/2;
	filterLoMarkerPos = (long long int)(filterFreq + filterLo - centerFreq)*(fftSize/2)/(sampleRate/2) + fftSize/2;
	filterHiMarkerPos = (long long int)(filterFreq + filterHi - centerFreq)*(fftSize/2)/(sampleRate/2) + fftSize/2;
	filterPixmap =  QPixmap(abs(filterLoMarkerPos-filterHiMarkerPos), ySize);
	filterPixmap.fill(colorSpecBackground);
	update();
}

void FilterGraph::setSampleRate(int sf) {
    if (sf==0)
        return;
    sampleRate = sf;
    fftFreqMarkerPos = (long long int)(filterFreq - centerFreq)*(fftSize/2)/(sampleRate/2) + fftSize/2;
    filterLoMarkerPos = (long long int)(filterFreq + filterLo - centerFreq)*(fftSize/2)/(sampleRate/2) + fftSize/2;
    filterHiMarkerPos = (long long int)(filterFreq + filterHi - centerFreq)*(fftSize/2)/(sampleRate/2) + fftSize/2;
    filterPixmap =  QPixmap(abs(filterLoMarkerPos-filterHiMarkerPos), ySize);
    filterPixmap.fill(colorSpecBackground);
    update();
}

void FilterGraph::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	startMovePos = event->pos().x();
    startMoveFreq = centerFreq;

    emit pauseFFTUpdates(true);

//    int freq = centerFreq;
//    int freq;
//    int deltaX = event->pos().x() - xViewPos;
//    if (deltaX < 0) {
//        freq = centerFreq + (xViewPos -deltaX - xSize/2) * (sampleRate/2) / (xSize/2);
//    } else {
//        freq = centerFreq + (xViewPos +deltaX - xSize/2) * (sampleRate/2) / (xSize/2);
//    }
//    qDebug() << "posX:" << xViewPos;
//    qDebug() << "ev X:" << event->pos().x();
//    emit freqChanged(freq);
}

void FilterGraph::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){

    emit pauseFFTUpdates(false);
}

void FilterGraph::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {

    int f;
	//if (event->pos().y()<ySize/3) {
		f = startMoveFreq - (event->pos().x() - startMovePos) * sampleRate / fftSize;
	//} else if (event->pos().y()<ySize*2/3) {
	//	f = startMoveFreq - (event->pos().x() - startMovePos) * 100;
	//} else {
	//	f = startMoveFreq - (event->pos().x() - startMovePos) * 10;
	//}
	centerFreq = checkRange(f,0,RX_CLOCK/2);
	emit freqChanged(centerFreq);
//    qDebug("GUI Freq changed to: %d", centerFreq);
}

void FilterGraph::wheelEvent(QGraphicsSceneWheelEvent *event) {
	centerFreq +=  event->delta()/120*freqStep;
	centerFreq -= centerFreq % freqStep;
	centerFreq = checkRange(centerFreq,0,RX_CLOCK/2);
	emit freqChanged(centerFreq);
}

void FilterGraph::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
    qDebug() << "Hover event";
//	mousePos =  event->pos();
}

void FilterGraph::setFreqStep(int step) {
	freqStep = step;
}

void FilterGraph::setOffset(int v) {
	freqOffset = v;
	update();
}

