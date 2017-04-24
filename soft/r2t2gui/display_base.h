#ifndef DISPLAY_BASE_H
#define DISPLAY_BASE_H

#include <QMainWindow>

class Display_base : public QMainWindow  {
	Q_OBJECT
    
public:
    // Display_base();
    virtual ~Display_base() {};


public slots:
	virtual void displaySet(int, int, int)=0;
	virtual void fftData(QByteArray)=0;
	virtual void setServers(QStringList, QStringList)=0;
	virtual void start()=0;

signals: 
	virtual void command(int, int, int)=0;
    
};

#endif
