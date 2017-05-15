#include "types.h"
#include "window.h"

void calcWindow(FFTWindow win, std::vector<float> &window, int size, bool inverse) {
    window.clear();
    switch (win) {
        case Hanning:
            for (int i=0;i<size;i++) 
                window.push_back(0.5  - 0.5  * cos (2 * M_PI / size * i));
            break;
        case BlackmanHerris:
            for (int i=0;i<size;i++) 
                window.push_back(0.35875  - 0.48829*cos(2*M_PI/size*i) + 0.14128*cos(4*M_PI/size*i) - 0.01168*cos(6*M_PI/size*i));
            break;
        case BlackmanNuttall:
            for (int i=0;i<size;i++) 
                window.push_back(0.3625819  - 0.4891775*cos(2*M_PI/size*i) + 0.1365995*cos(4*M_PI/size*i) - 0.0106411*cos(6*M_PI/size*i));
            break;
        case None:
        default:
            for (int i=0;i<size;i++) 
                window.push_back(1.0);
            break;
    }

    if (inverse) {
        for (int i=0;i<size;i++) {
            if (window[i]!=0) 
                window[i] = 1.0/window[i];
        }
    }
}
