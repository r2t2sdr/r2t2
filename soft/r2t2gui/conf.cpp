#include <algorithm> 
#include "assert.h"
#include "conf.h"
#include "config.h"


Conf::Conf (QSettings *settings, uint32_t maxIdx): settings(settings), maxIdx(maxIdx) {
    if (maxIdx >= MAX_CONFIG) { 
        assert(0);
        maxIdx = MAX_CONFIG-1;
    }
    for (uint32_t i=0;i<maxIdx;i++) {
        cur[i] = 0;
        min[i] = 0;
        max[i] = 1;
    }

    set(CMD_AGC, 2, 0, 4);
    set(CMD_ANT, 0, 0, 1);
    set(CMD_AUDIO_COMP, 0, 0, 4);
    set(CMD_DISP_COLOR, 1, 0, 4);
    set(CMD_DISP_MODE, 2, 0, 4);
    set(CMD_FFT_SAMPLE_RATE, 192000, 8000, 192000);
    set(CMD_FFT_SIZE, FFT_SIZE, 256, 4192);
    set(CMD_FFT_TIME, 100, 20, 9999);
    set(CMD_FILTER_RX_HI, 3000, 1500, 3900);
    set(CMD_FILTER_RX_LO, 300, 50, 1000);
    set(CMD_FILTER_TX_HI, 3000, 1500, 3900);
    set(CMD_FILTER_TX_LO, 300, 50, 1000);
    set(CMD_GAIN, 0, 0, 255);
    set(CMD_MIC, 0, 0, 255);
    set(CMD_MODE, MODE_LSB, 0, MODE_LAST);
    set(CMD_NB_LEVEL, 255, 0, 255);
    set(CMD_PREAMP, 0, -30, 30);
    set(CMD_PRESEL, 0, 0, 10);
    set(CMD_QTRADIO_RX, 0, 0, 7);
    set(CMD_RSSI, -100, -256, 256);
    set(CMD_NOISE_FILTER, 0, 0,255);
    set(CMD_RX_FREQ, 7100000, 0, 61440000);
    set(CMD_SAMPLE_RATE, DEFAULT_SAMPLE_RATE, 8000, 16000);
    set(CMD_STEP, 100, 1, 1000000);
    set(CMD_SWR, 0, -256, 0);
    set(CMD_TX, 0);
    set(CMD_TX_DELAY, 0, 0, 1000);
    set(CMD_TX_FREQ, get(CMD_RX_FREQ), 0, 61440000);
    set(CMD_TX_POWER, 100, 0, 100);
    set(CMD_TX_RX, 1, 0, 1);
    set(CMD_VOLUME, 30, 0, 255);
    set(CMD_WATERFALL_MAX, WATERFALL_MAX, -256, 256);
    set(CMD_WATERFALL_MIN, WATERFALL_MIN, -256, 256);
}

int32_t Conf::set(uint32_t idx, int32_t val) {
    if (idx > maxIdx) {
        return 0;
    }
    assert(val<=max[idx]);
    assert(val>=min[idx]);
    val = std::max(min[idx], val);
    val = std::min(max[idx], val);
    cur[idx] = val;
    // printf("conf set: %s, par %i\n", CmdString[idx], val);
    return val;
}

int32_t Conf::set(uint32_t idx, int32_t val, int32_t mi, int32_t ma) {
    if (idx > maxIdx) {
        return 0;
    }
    min[idx] = mi;
    max[idx] = ma;
    val = std::max(min[idx], val);
    val = std::min(max[idx], val);
    cur[idx] = val;
    return val;
}

int32_t Conf::get(uint32_t idx) {
    if (idx >= maxIdx) {
        assert(0);
        return 0;
    }
    // printf("conf get: %s, par %i\n", CmdString[idx], cur[idx]);
    return cur[idx];
}

int Conf::read() {
    settings->beginGroup("Control");
    settings->beginReadArray("Config");
    for (uint32_t i=0;i<maxIdx;i++) {
        settings->setArrayIndex(i);
        for(uint32_t j=0;j<maxIdx;j++) {
            if (CmdString[j] == settings->value("name").toString())
                this->set(j, settings->value("val",this->get(j)).toInt());
        }
    }
    settings->endArray();
    settings->endGroup();
    return 0;
}

int Conf::save() {
    settings->beginGroup("Control");
    settings->beginWriteArray("Config");
    for (uint32_t i=0;i<maxIdx;i++) {
        settings->setArrayIndex(i);
        settings->setValue("name", CmdString[i]);
        settings->setValue("val", this->get(i));
    }
    settings->endArray();
    settings->endGroup();
    return 0;
}
