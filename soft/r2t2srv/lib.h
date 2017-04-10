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

#define arraysize(a) (int)(sizeof(a)/sizeof(a[0]))

void readGlobalSettings(QSettings*);
uint32_t log2fix (uint32_t n);
void PDEBUG  (int Level, const char *Format, ... );
int checkRange(int,int,int);
int getBand(int);
int getSettings(QSettings* settings, QString key, int def);
QString getSettings(QSettings* settings, QString key, QString def);

#endif
