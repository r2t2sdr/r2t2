#include <QThread>
#include <QMutex>

#include "config.h"

class Watchdog : public QThread {
	Q_OBJECT
	public:
		Watchdog();
		~Watchdog();
		void run();

    public slots:
        void trigger(int);
    private:
        int watch;
        QMutex mutex;
};

