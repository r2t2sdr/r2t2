#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include "textbutton.h"
#include "display_touch.h"
#include "lib.h"

TextButton::TextButton(QSettings *settings, text_button_t *button) : settings(settings) {
	// setFlags(QGraphicsItem::ItemIsMovable);
	b = button;
	isPressed = false;
    isActiv = false;

	buttonBackground = new QPixmap(b->xSize, b->ySize);
	QPainter painter(buttonBackground);
	QLinearGradient linearGrad(QPointF(b->xSize/2, b->ySize/4), QPointF(b->xSize/1, b->ySize*3/4));
	linearGrad.setColorAt(0, QColor(getSettings(settings,"display/colorButtonBackground1","#282828")));
	linearGrad.setColorAt(1, QColor(getSettings(settings,"display/colorButtonBackground2","#4b4b4b")));
	painter.setBrush(QBrush(linearGrad));
	painter.drawRect(buttonBackground->rect());

	buttonBackgroundP = new QPixmap(b->xSize, b->ySize);
	QPainter painterP(buttonBackgroundP);
	QLinearGradient linearGradP(QPointF(b->xSize/2, b->ySize/4), QPointF(b->xSize/1, b->ySize*3/4));
	linearGradP.setColorAt(0, QColor(getSettings(settings,"display/colorButtonPressedBackground1","#4b4baf")));
	linearGradP.setColorAt(1, QColor(getSettings(settings,"display/colorButtonPressedBackground2","#282828")));
	painterP.setBrush(QBrush(linearGradP));
	painterP.drawRect(buttonBackgroundP->rect());

	buttonBackgroundA = new QPixmap(b->xSize, b->ySize);
	QPainter painterA(buttonBackgroundA);
	QLinearGradient linearGradA(QPointF(b->xSize/2, b->ySize/4), QPointF(b->xSize/1, b->ySize*3/4));
	linearGradA.setColorAt(0, QColor(getSettings(settings,"display/colorButtonActive1","#2828af")));
	linearGradA.setColorAt(1, QColor(getSettings(settings,"display/colorButtonActive2","#4b4b4b")));
	painterA.setBrush(QBrush(linearGradA));
	painterA.drawRect(buttonBackgroundA->rect());

	colorButtonFrame = QColor(getSettings(settings,"display/colorButtonFrame","#000000"));
	colorButtonText  = QColor(getSettings(settings,"display/colorButtonText","#ffffff"));
	colorButtonName  = QColor(getSettings(settings,"display/colorButtonName","#0080e0"));
	colorButtonLedOn = QColor(getSettings(settings,"display/colorButtonLedOn","#00e000"));
	colorButtonLedOff= QColor(getSettings(settings,"display/colorButtonLedOff","#000000"));
	colorButtonPosOn = QColor(getSettings(settings,"display/colorButtonPosOn","#00e000"));
	colorButtonPosOff= QColor(getSettings(settings,"display/colorButtonPosOff","#000000"));

	for (int i=0;i<MAX_ENTRY;i++) {
		if (entry[b->cmd][i].name[0] == 0)
			break;
		b->buttonText.append(entry[b->cmd][i].name);
		b->buttonValue[i] = entry[b->cmd][i].par;
	}
}

TextButton::~TextButton() {
	delete buttonBackground;
	delete buttonBackgroundP;
	delete buttonBackgroundA;
}

void TextButton::setCmd(int cmd, QString name) {
	b->name = name;
	b->cmd = cmd;
	b->buttonText.clear();
	for (int i=0;i<MAX_ENTRY;i++) {
		if (entry[b->cmd][i].name == NULL)
			break;
		b->buttonText.append(entry[b->cmd][i].name);
		b->buttonValue[i] = entry[b->cmd][i].par;
	}
	update();
}

void TextButton::setVal(int v) {
	if (b->buttonText.size() >0 && b->buttonText.at(0)==QString("ANALOG")) {
		b->pos = v;
		update();
	} else {
		for (int i=0;i<b->buttonText.size();i++) {
			if (b->buttonValue[i] == v) {
				b->pos = i;
				update();
				return;
			}
		}
	}
}

int TextButton::getVal() {
	if (b->buttonText.at(0)==QString("ANALOG"))
		return b->pos;
	return b->buttonValue[b->pos];
}

void TextButton::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
	int pos = b->pos;
	QString text="";

	if ((b->buttonText.size()>0) && b->buttonText.at(0)==QString("ANALOG")) {
		text = QString("%1").arg(pos);
	} else {
		if (b->buttonText.size() > pos)
			text = b->buttonText.at(pos);
		else
			text = QString("%1").arg(pos);
	}
    QFont font("Monospace");
    font.setPixelSize(b->ySize*2/5);
	painter->setFont(font);
	QFontMetrics fm = painter->fontMetrics();
	QPoint center = QPoint(( b->xSize-fm.width(text))/2, ( b->ySize+fm.height())/2-5 );

	painter->setPen(Qt::white);
	painter->setBrush(colorButtonFrame);

	if (isPressed)
		painter->drawPixmap(0,0, *buttonBackgroundP);
    else if (isActiv)
		painter->drawPixmap(0,0, *buttonBackgroundA);
	else
		painter->drawPixmap(0,0, *buttonBackground);
	painter->setPen(colorButtonText);
	painter->drawText(center,text);
	painter->setPen(colorButtonName);
	painter->drawText(b->xSize/2-(fm.width(b->name))/2, b->ySize-2, b->name);

	if (b->buttonText.size() > 0 && b->buttonText.at(0)==QString("ANALOG")) {
		painter->setBrush(colorButtonPosOff);
		painter->setPen(colorButtonPosOff);
		if (b->buttonValue[0]-1 > b->pos) {
			painter->setBrush(colorButtonPosOn);
			painter->setPen(colorButtonPosOn);
		}
		painter->drawRect(b->xSize*7/8,b->ySize/2,2,2);

		painter->setBrush(colorButtonPosOff);
		painter->setPen(colorButtonPosOff);
		if (b->pos > b->buttonValue[1]) {
			painter->setBrush(colorButtonPosOn);
			painter->setPen(colorButtonPosOn);
		}
		painter->drawRect(b->xSize/8,b->ySize/2,2,2);
	} else {
		if (b->buttonText.size()>2) {
			painter->setBrush(colorButtonPosOff);
			painter->setPen(colorButtonPosOff);
			if (b->buttonText.size()-1 > b->pos) {
				painter->setBrush(colorButtonPosOn);
				painter->setPen(colorButtonPosOn);
			}
			painter->drawRect(b->xSize*7/8,b->ySize/2,2,2);

			painter->setBrush(colorButtonPosOff);
			painter->setPen(colorButtonPosOff);
			if (b->pos>0) {
				painter->setBrush(colorButtonPosOn);
				painter->setPen(colorButtonPosOn);
			}
			painter->drawRect(b->xSize/8,b->ySize/2,2,2);
		} else if (b->buttonText.size()==2){
			painter->setBrush(colorButtonLedOff);
			painter->setPen(colorButtonLedOff);
			if (b->pos==1) {
				painter->setBrush(colorButtonLedOn);
			}
			painter->drawRect(b->xSize*3/8,b->ySize/8,b->xSize*2/8,b->ySize/8);
		} else {

			// nothing
		}
	}
}

QRectF TextButton::boundingRect() const {
    return QRectF(0, 0, b->xSize, b->ySize);
}

void TextButton::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	bool left = (event->pos().x() <  b->xSize/2);
	pressed(left,false);
}

void TextButton::pressed(bool left, bool autoUp) {
	if (b->buttonText.size()>0 && b->buttonText.at(0)==QString("ANALOG")) {
		if (left && b->pos > b->buttonValue[1]) 
			b->pos--;
		if (!left && b->pos+1 < b->buttonValue[0])
			b->pos++;
		isPressed = !autoUp;
		emit pressed(b->cmd, b->pos);
		update();
		return;
	} 

	if (b->buttonText.size()>2) {
		if (left && b->pos>0) 
			b->pos--;
		if (!left && b->pos+1 < b->buttonText.size())
			b->pos++;
	} else if (b->buttonText.size()==2) {
		if (b->pos>0)
			b->pos=0;
		else 
			b->pos=1;
	}
	isPressed = !autoUp;
	emit pressed(b->cmd, b->buttonValue[b->pos]);
	update();
}

void TextButton::mouseReleaseEvent(QGraphicsSceneMouseEvent * /*event*/) {
	isPressed = false;
	update();
}

void TextButton::doUpdate() {
	update();
}

void TextButton::setActive(bool activ) {
    isActiv = activ;
}
