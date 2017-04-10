#ifndef _LIB_H_
#define _LIB_H_

#include <QSettings>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include "sys/time.h"

#define ERR1 1
#define MSG1 2
#define MSG2 3
#define MSG3 4
#define MSG4 5

#define ARG_UNUSED(par)       if (par) do { ; } while (0)
#define arraysize(a) (int)(sizeof(a)/sizeof(a[0]))


template <typename T> void cmul(T &a, const T b) {
	T t;
	t.re = a.re * b.re - a.im * b.im;
	a.im = a.re * b.im + a.im * b.re;
	a.re = t.re;
}

void PDEBUG  (int Level, const char *Format, ... );
int checkRange(int,int,int);
int getSettings(QSettings* settings, QString key, int def);
QString getSettings(QSettings* settings, QString key, QString def);

void startGlobTime();
void setGlobTime(const char *s);
void outGlobTime(const char *s);

#endif
