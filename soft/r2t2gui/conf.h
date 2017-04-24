#ifndef _CONF_H_
#define _CONF_H_

#include <stdint.h>
#include <QSettings>

#define MAX_CONFIG  256

class Conf {
	public:
        Conf(QSettings *settings, uint32_t maxIdx);
        int32_t set(uint32_t idx, int32_t val);
        int32_t set(uint32_t idx, int32_t val, int32_t min, int32_t max);
        int32_t get(uint32_t idx);
        int read();
        int save();

    private:
        QSettings *settings;
        int32_t cur[MAX_CONFIG];
        int32_t min[MAX_CONFIG];
        int32_t max[MAX_CONFIG];
        uint32_t maxIdx;
};

#endif
