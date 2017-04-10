#include <QDebug>
#include <QSettings>
#include "lib.h"

#ifdef DEBUG_TIME
#define MAX_GLOB_TIME   32
static uint32_t globTime[MAX_GLOB_TIME];
static char globTimeStr[32][MAX_GLOB_TIME];
static int globTimeCnt;
#endif


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

#ifdef DEBUG_TIME

static uint32_t getTime() {
	timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_usec + (tv.tv_sec & 0xfff) *1000000;
}

void startGlobTime() {
	memset (globTime, 0, sizeof(globTime));
	globTimeCnt=0;
	globTime[globTimeCnt]=getTime();

}
void setGlobTime(const char *s) {
	if (globTimeCnt >= MAX_GLOB_TIME)
		return;
	globTime[globTimeCnt]=getTime();
	strcpy(globTimeStr[globTimeCnt], s);
	globTimeCnt++;
}

void outGlobTime(const char *comment) {
	uint32_t lastTime = 0;
	printf("# %s\n", comment);
	for (int i=0;i<MAX_GLOB_TIME;i++)
		if (globTime[i]!=0) {
			uint32_t curTime = globTime[i]- globTime[0];
			printf("# %20s %5i %5i\n", globTimeStr[i], curTime, curTime - lastTime);
			lastTime = curTime;
		}
}
#else
void startGlobTime(){};
void setGlobTime(const char *s) {
	ARG_UNUSED(s);
};
void outGlobTime(const char *s) {
	ARG_UNUSED(s);
};
#endif



