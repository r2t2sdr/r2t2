#include "fftGraph.h"

#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <math.h>
#include <stdlib.h>
#include "lib.h"

using namespace std;

FFTGraph::FFTGraph(QSettings *settings, int x, int y) : settings(settings), xSize(x), ySize(y) {
	int i;

	setFlags(QGraphicsItem::ItemClipsToShape);

	displayMode = GRAPH_WATERFALL;
	nFFT = FFT_SIZE;
	xViewPos = (nFFT-xSize)/2;
	fftPixmap = new QPixmap(nFFT, ySize);
	fftPixmap->fill(QColor(getSettings(settings,"display/colorFFTBackground","#000000")));
	waterfallPixmap = new QPixmap(nFFT, ySize);
	waterfallPixmap->fill(QColor(getSettings(settings,"display/colorFFTBackground","#000000")));
	mi = -1.5; 
	tmi = mi; 
	ma = -0.5;
	tma = ma;

	settingsChanged(1);

	for(i=0; i < MAX_FFT; i++) {
		fftmax[i]=0;
		fftav[i]=0;
	}

	base = 3;
	scale = 40;
	setAuto = false;
}

FFTGraph::~FFTGraph() {
	delete fftPixmap;
	delete waterfallPixmap;
}

void FFTGraph::setSize(int x, int y) {
	xSize = x; 
	ySize = y;
	xViewPos = (nFFT-xSize)/2;
	delete fftPixmap;
	fftPixmap = new QPixmap(nFFT, ySize);
	fftPixmap->fill(QColor(getSettings(settings,"display/colorFFTBackground","#000000")));
	delete waterfallPixmap;
	waterfallPixmap = new QPixmap(nFFT, ySize);
	waterfallPixmap->fill(QColor(getSettings(settings,"display/colorFFTBackground","#000000")));
}

void FFTGraph::settingsChanged(int mode) {
	double h,s,v;
	average_time_max = getSettings(settings,"display/averageTimeMax",64);
	average_time =     getSettings(settings,"display/averageTime",16);
	colorSpecMax =     QColor(getSettings(settings,"display/colorFFTMax","#d11346"));
	colorSpecAverage = QColor(getSettings(settings,"display/colorFFTAverage","#09f088"));
	colorSpecBack =    QColor(getSettings(settings,"display/colorFFTBackground","#000000"));
	colorSpecMinMax =  QColor(getSettings(settings,"display/colorFFTMinMax","#d0d000"));
	fft_scale =  2; 

	double h_min = 1;
	double h_max = 0;
	double s_min = 1;
	double s_max = 1;
	double v_min = 0;
	double v_max = 1;

	for( int i=0; i<256; i++) {
		switch (mode) {
			case 0:
				colorTab[i].setRgb(i,i,i);
				break;
			case 1:
				if( (i<43) )             colorTab[i].setRgb( 0,0, 255*(i)/43);
				if( (i>=43) && (i<87) )  colorTab[i].setRgb( 0, 255*(i-43)/43, 255);
				if( (i>=87) && (i<120))  colorTab[i].setRgb( 0,255, 255-(255*(i-87)/32));
				if( (i>=120) && (i<154)) colorTab[i].setRgb( (255*(i-120)/33), 255, 0);
				if( (i>=154) && (i<217)) colorTab[i].setRgb( 255, 255 - (255*(i-154)/62), 0);
				if( (i>=217))            colorTab[i].setRgb( 255, 0, 128*(i-217)/38);
				break;
			case 2:
				h = (h_max-h_min)*i/256+h_min;
				if (h>1) h-=1;
				s = (s_max-s_min)*i/256+s_min;
				if (s>1) s-=1;
				v = (v_max-v_min)*i/256+v_min;
				if (v>1) v-=1;
				colorTab[i].setHsvF(h,s,v);
				break;
		}
	}
}


void FFTGraph::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
	switch (displayMode) {
		case GRAPH_WATERFALL:
			painter->drawPixmap(-xViewPos,0,*waterfallPixmap);
			break;
		case GRAPH_FFT:
			painter->drawPixmap(-xViewPos,0,*fftPixmap);
			break;
		case GRAPH_DUAL:
		case GRAPH_DUAL2:
			painter->drawPixmap(-xViewPos,0,*fftPixmap);
			painter->drawPixmap(-xViewPos,130,*waterfallPixmap);
			break;
	}

}

void FFTGraph::setFFTSize(int size) {
	nFFT = size;
	xViewPos = (nFFT-xSize)/2;
	delete fftPixmap;
	fftPixmap = new QPixmap(nFFT, ySize);
	fftPixmap->fill(Qt::black);
	delete waterfallPixmap;
	waterfallPixmap = new QPixmap(nFFT, ySize);
	waterfallPixmap->fill(Qt::black);
	for(int i=0; i < MAX_FFT; i++) {
		fftmax[i]=0;
		fftav[i]=0;
	}
}

QRectF FFTGraph::boundingRect() const {
	return QRectF(0, 0, xSize, ySize);
}

void FFTGraph::setMin(int v) {	// dbm
	tmi = v; 
	if (tmi>=tma) tma = tmi + 0.1;
	base = tmi;
	scale = 256/(tma-tmi);
}

void FFTGraph::setMax(int v) {   // dbm
	tma = v;
	if (tmi>=tma) tmi = tma - 0.1;
	base = tmi;
	scale = 256/(tma-tmi);
}

void FFTGraph::setDisplayMode(int m) {
	for(int i=0; i < MAX_FFT; i++) {
		fftmax[i]=-10;
		fftav[i]=0;
	}
	fftPixmap->fill(Qt::black);
	waterfallPixmap->fill(Qt::black);
	displayMode = m;
}

void FFTGraph::fftDataReady(QByteArray &data) {
	int i;
	double v;

	mi = 0;
	ma = -100;

	for(i=0; i < nFFT; i++) {
		v = -(uint8_t)data[i];
		v1[i] = v;
		if (fftmax[i]>v)
			fftmax[i] = v;
		fftmax[i] = ((average_time_max-1)*fftmax[i]+v)/average_time_max;
		if (fftav[i]==0)
			fftav[i] = v;
		else
			fftav[i] = ((average_time-1)*fftav[i]+v)/average_time;

		if (i>nFFT/8 && i<nFFT*7/8)
			mi = min(fftav[i],mi);
		ma = max(fftav[i],ma);
	}

	switch (displayMode) {
		case GRAPH_WATERFALL:
			{
				QPainter painter(waterfallPixmap);
				waterfallPixmap->scroll(0,1,waterfallPixmap->rect());
				for(i=0; i < nFFT; i++) {
					v = (v1[i]-base)*scale;
					if (v>255) v=255;
					if (v<0)   v=0;
					painter.setPen(colorTab[(unsigned char)v]);
					painter.drawPoint(i,0);
				}
				if(setAuto) {
					base = -mi;
					scale = 256/(ma-mi);
					setAuto = false;
				}
			}
			break;
		case GRAPH_FFT:
            fft_scale = 2;
			{
				QPainter painter(fftPixmap);
				fftPixmap->fill(colorSpecBack);
				// setPen kostet rechenzeit, daher jede Schleife einzeln
				painter.setPen(colorSpecMax);
				for(i=1; i < nFFT; i++) 
					painter.drawLine(i-1,-fftmax[i-1]*fft_scale,i,-fftmax[i]*fft_scale);
				painter.setPen(colorSpecAverage);
				for(i=1; i < nFFT; i++) 
					painter.drawLine(i-1,-fftav[i-1]*fft_scale,i,-fftav[i]*fft_scale);
				painter.setPen(colorSpecMinMax);
				painter.drawLine(0,-mi*fft_scale,nFFT,-mi*fft_scale);
				painter.drawLine(0,-ma*fft_scale,nFFT,-ma*fft_scale);
			}
			break;
		case GRAPH_DUAL:
            fft_scale = 1;
			{
				QPainter painter(waterfallPixmap);
				waterfallPixmap->scroll(0,1,waterfallPixmap->rect());
				for(i=0; i < nFFT; i++) {
					v = (v1[i]-base)*scale;
					if (v>255) v=255;
					if (v<0)   v=0;
					painter.setPen(colorTab[(unsigned char)v]);
					painter.drawPoint(i,0);
				}
				if(setAuto) {
					base = -mi;
					scale = 256/(ma-mi);
					setAuto = false;
				}
			}
			{
				QPainter painter(fftPixmap);
				fftPixmap->fill(colorSpecBack);
				painter.setPen(colorSpecMax);
				for(i=1; i < nFFT; i++)  
					painter.drawLine(i-1,-fftmax[i-1]*fft_scale-DUAL_OFFSET*fft_scale,i,-fftmax[i]*fft_scale-DUAL_OFFSET*fft_scale);
				painter.setPen(colorSpecAverage);
				for(i=1; i < nFFT; i++) 
					painter.drawLine(i-1,-fftav[i-1]*fft_scale-DUAL_OFFSET*fft_scale,i,-fftav[i]*fft_scale-DUAL_OFFSET*fft_scale);
			}
			break;
		case GRAPH_DUAL2:
            fft_scale = 1;
			{
				QPainter painter(waterfallPixmap);
				waterfallPixmap->scroll(0,1,waterfallPixmap->rect());
				for(i=0; i < nFFT; i++) {
					v = (v1[i]-base)*scale;
					if (v>255) v=255;
					if (v<0)   v=0;
					painter.setPen(colorTab[(unsigned char)v]);
					painter.drawPoint(i,0);
				}
				if(setAuto) {
					base = -mi;
					scale = 256/(ma-mi);
					setAuto = false;
				}
			}
			{
				QPainter painter(fftPixmap);
				fftPixmap->fill(colorSpecBack);
				painter.setPen(colorSpecMax);
				for(i=1; i < nFFT; i++) { 
					v = (v1[i]-base)*scale;
					if (v>255) v=255;
					if (v<0)   v=0;
					painter.setPen(colorTab[(unsigned char)v]);
					painter.drawLine(i,-fftmax[i]*fft_scale-DUAL_OFFSET*fft_scale,i,300);
				}
				painter.setPen(colorSpecMinMax);
			}
			break;
		default:
			;
	}
	update();
}


void FFTGraph::setAutomaticCB() {
	setAuto = false;
}

int FFTGraph::getMin() {
	return mi;
}

int FFTGraph::getMax() {
	return ma;
}

void FFTGraph::scrollVert(int diff) {
	waterfallPixmap->scroll(diff,0,waterfallPixmap->rect());
	QPainter painter(waterfallPixmap);
	painter.setPen(colorSpecBack);
	painter.setBrush(colorSpecBack);
	if (diff>0)
		painter.drawRect(0,0,diff,ySize);
	else
		painter.drawRect(nFFT+diff,0,nFFT,ySize);
}
