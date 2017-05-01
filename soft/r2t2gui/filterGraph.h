#include <QGraphicsItem>
#include <QWidget>
#include <QSettings>
#include "config.h"
#include "sdrgraphicsitem.h"

class FilterGraph: public QObject, public SdrGraphicsItem  {
	Q_OBJECT
	//Q_INTERFACES(QGraphicsItem)
	public:
	FilterGraph(QSettings *settings, int x, int y);
	~FilterGraph();
	QRectF boundingRect() const;
	void setFreq(int f);
	void setFilter(int f, int fl, int fh, int mode);
	//void setFreq(int cf, int f, int fl, int fh, int m);
	void setFFTSize(int size);
	void setSampleRate (int sampleRate);
	void setDisplayMode(int m);
	void setFreqStep(int step);
	void setViewPosX(int);
	void setViewPosY(int);
	void settingsChanged();
	void setSize(int,int);
	void setOffset(int);
    signals:
        void freqChanged(int freq);
    private:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
        void mousePressEvent(QGraphicsSceneMouseEvent *event);
        void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void wheelEvent( QGraphicsSceneWheelEvent *event);
	void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
	QPixmap *filterPixmap1, *filterPixmap2;
	QPixmap filterPixmap;
	QPointF mousePos;
	QSettings *settings;
	int xViewPos;
	int fftFreqMarkerPos;
	int filterLoMarkerPos, filterHiMarkerPos;

	int xSize, ySize;
	int sampleRate;
	int filterFreq,centerFreq;
	int filterLo,filterHi;
	int fftSize;
	int dispMode;
	int mode;
	int freqStep;
	QColor colorSpecFreq, colorSpecGrid, colorSpecBackground;
	int fft_scale, shadow;
	int startMoveFreq;
	int startMovePos;
	int freqOffset = 0;
};
