#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <vector>

enum FFTWindow {
    None,
    Hanning,
    BlackmanHerris,
    BlackmanNuttall
};


void calcWindow(FFTWindow win, std::vector<float> &window, int size, bool inverse);

#endif
