#include <QDebug>
#ifdef ANDROID
#include <QtAndroidExtras>
#include <QAndroidJniObject>
#endif
#include <QSettings>
#include "lib.h"


int bandMin[] =     {  0, 1810000, 3500000, 7000000, 10100000, 14000000, 18068000, 21000000, 24890000, 28000000, 50000000, 472000, 135700};
int bandMax[] =     {  0, 2000000, 3800000, 7200000, 10150000, 14350000, 18168000, 21450000, 24990000, 29700000, 51000000, 479000, 137800};
int bandTxLevel[] = {100,     100,     100,     100,      100,      100,      100,      100,      100,      100,      100,    100,    100};


void readGlobalSettings(QSettings *settings) {

	settings->beginGroup("Common");
	settings->beginReadArray("HamBand");
	for (int i=0;i<arraysize(bandMin);i++) {
		settings->setArrayIndex(i);
		bandMin[i] = settings->value("bandMin",bandMin[i]).toInt();
		settings->setValue("bandMin", bandMin[i]);

		bandMax[i] = settings->value("bandMax",bandMax[i]).toInt();
		settings->setValue("bandMax", bandMax[i]);

		bandTxLevel[i] = settings->value("bandTxLevel",bandTxLevel[i]).toInt();
		settings->setValue("bandTxLevel", bandTxLevel[i]);
	}
	settings->endArray();
	settings->endGroup();
}


// input: uint32, output: uint16.uint16
uint32_t log2fix(uint32_t n) {
    static const uint16_t lut[16] = { // log2(1.0)...log2(2.0) in 1/16 interval
        0, 5732, 11136, 16248, 21098, 25711, 30109, 34312, 38336, 42196, 45904, 49472, 52911, 56229, 59434, 62534 };

	uint32_t p=31;

	if (n<=0)
		return 0;

    while (!(n & 0x80000000)) {
		p--;
		n <<= 1;
    }
	return (p << 16) + lut[(n >> 27) & 0x0f];
}

int debugLevel = 0;
timeval tStart;
static char s[1024];
static char t[32];

void PDEBUG (int Level, const char *Format, ... ) {
	if (debugLevel >= Level) {
		va_list ap;
		timeval tv;
		int32_t elapsedTime;
		int32_t elapsedTimeUsec;
		va_start(ap, Format);
		gettimeofday(&tv, NULL);
		elapsedTime = tv.tv_sec - tStart.tv_sec; 
		elapsedTimeUsec = tv.tv_usec - tStart.tv_usec;
		if (elapsedTimeUsec <= 0) {
			elapsedTime--;
			elapsedTimeUsec += 1000000;
		}
		vsnprintf(s,sizeof(s),Format,ap);
		va_end(ap);
		sprintf(t, "%4i.%06i", elapsedTime, elapsedTimeUsec);
		qDebug() << t <<  s;
	}
}


int checkRange(int val, int min, int max) {
	if (val>max)
		return max;
	if (val<min)
		return min;
	return val;
}

double checkRange(double val, double min, double max) {
	if (val>max)
		return max;
	if (val<min)
		return min;
	return val;
}

int getBand(int f) {
	for (int i=0;i<arraysize(bandMin);i++) 
		if (f >= bandMin[i] && f <= bandMax[i])
			return i;
	return 0;
}
				
int getSettings(QSettings* settings, QString key, int def) {
	int n = settings->value(key,def).toInt();
	settings->setValue(key,n);
	return n;
}

QString getSettings(QSettings* settings, QString key, QString def) {
	QString s = settings->value(key,def).toString();
	settings->setValue(key,s);
	return s;
}

void keepScreenOn(bool on) {
#ifdef ANDROID
  QtAndroid::runOnAndroidThread([on]{
    QAndroidJniObject activity = QtAndroid::androidActivity();
    if (activity.isValid()) {
      QAndroidJniObject window =
          activity.callObjectMethod("getWindow", "()Landroid/view/Window;");

      if (window.isValid()) {
        const int FLAG_KEEP_SCREEN_ON = 128;
        if (on) {
          window.callMethod<void>("addFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
        } else {
          window.callMethod<void>("clearFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
        }
      }
    }
    QAndroidJniEnvironment env;
    if (env->ExceptionCheck()) {
      env->ExceptionClear();
    }
  });
#else 
  ARG_UNUSED(on);
#endif
}
