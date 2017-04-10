#ifndef SDRGRAPHICSITEM_H
#define SDRGRAPHICSITEM_H

#include <QGraphicsItem>

class SdrGraphicsItem : public QGraphicsItem {
	public:
		SdrGraphicsItem();
		~SdrGraphicsItem();
		virtual void setSize(int,int)=0;

};

#endif
