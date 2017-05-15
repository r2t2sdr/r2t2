#include <QDebug>
#include <QCoreApplication>
#include "watchdog.h"

Watchdog::Watchdog() {
    watch = 10;
}

Watchdog::~Watchdog() {
}

void Watchdog::trigger(int /*id*/) {
    mutex.lock();
    watch = 10;
    mutex.unlock();
}

void Watchdog::run() {
    while (watch>0) {
        mutex.lock();
        watch--;
        mutex.unlock();
        msleep(1000);
    }
    qDebug() << "watchdog timeout, force terminate";
    QCoreApplication::exit();
    std::exit(-1);
}

