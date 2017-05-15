#include <QGraphicsItem>
#include <QTimer>
#include <QWheelEvent>
#include <QSettings>
#include "config.h"
#include "sdrgraphicsitem.h"

#define GRAPH_WATERFALL	0
#define GRAPH_FFT		1
#define GRAPH_DUAL		2
#define GRAPH_DUAL2		3
#define GRAPH_NONE		4

class FFTGraph: public QObject, public SdrGraphicsItem  {
    Q_OBJECT
	//Q_INTERFACES(QGraphicsItem)
    public:
	FFTGraph(QSettings *settings, int x,int y);
	~FFTGraph();
	QRectF boundingRect() const;
    public slots:
	void fftDataReady(QByteArray &data); 
	void setMin(int v);
	void setMax(int v);
	int getMin();
	int getMax();
	void setDisplayMode(int m);
	void setFFTSize(int size);
	void setAutomaticCB();
	void settingsChanged(int);
	void scrollVert(int);
	void setSize(int,int);

    private:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	void updateLine();

	double fftmax[MAX_FFT];
	double fftav[MAX_FFT];
	double v1[MAX_FFT];
	QColor colorTab[256];
	QPixmap *fftPixmap;
    QPixmap *waterfallPixmap;
    QSettings *settings;
	int xViewPos;

	int nFFT;
	int xSize, ySize;
	double base,scale;
	int displayMode;
	double mi,ma;
	double tmi,tma;
	bool setAuto;
	int average_time_max,average_time;
	QColor colorSpecMinMax,colorSpecMax,colorSpecAverage,colorSpecBack;
	int fft_scale;
	double specCal;
};
