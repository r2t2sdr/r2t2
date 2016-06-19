
#ifndef ADV7511_H_
#define ADV7511_H_

#include "xiicps.h"

#define IIC_SCLK_RATE			100000
#define ADV7511_ADDR			0x39

XIicPs Iic;		/**< Instance of the IIC Device */
int adv7511_init(short DeviceID, char Bus, char ChipAddr);

#endif /* ADV7511_H_ */
