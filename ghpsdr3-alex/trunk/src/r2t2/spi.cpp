#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define MDIO_BASE 	138   // base addr extracted from /sys/kernel/debug/gpio
#define ADC_SDIO 	(MDIO_BASE + 9)
#define ADC_CSB 	(MDIO_BASE + 10)
#define ADC_SCLK 	(MDIO_BASE + 11)

#define FPGA_GPIO_BASE 106   // base addr extracted from /sys/kernel/debug/gpio
#define DA_SCLK 	(FPGA_GPIO_BASE + 0) 
#define DA_SDIO 	(FPGA_GPIO_BASE + 1) 
#define DA_CS   	(FPGA_GPIO_BASE + 2) 

#define ATT_A_LE   	(FPGA_GPIO_BASE + 3) 
#define ATT_B_LE   	(FPGA_GPIO_BASE + 4) 
#define ATT_CLK   	(FPGA_GPIO_BASE + 5) 
#define ATT_DAT   	(FPGA_GPIO_BASE + 6) 

#define PGA_LCH1   	(FPGA_GPIO_BASE + 7) 
#define PGA_LCH2   	(FPGA_GPIO_BASE + 8) 
#define PGA_SDI   	(FPGA_GPIO_BASE + 9) 
#define PGA_CLK   	(FPGA_GPIO_BASE + 10) 

#define TCLK 	1 		// tclk < 40ns

#define SPI_READ 		0x80
#define SPI_WRITE 		0x00
#define SPI_ONE_BYTE 	0x00
#define SPI_TWO_BYTE 	0x20

#define SPI_ADDR_MASK 	0x1FFF

#define ADC_SPI_PORT		0x0000
#define ADC_CHIP_ID 		0x0001
#define ADC_CHIP_GRADE 		0x0002
#define ADC_DEVICE_INDEX 	0x0005


uint8_t dac_init_data[] =  {
	0x00, 0x20, 	// _Reset
	0x00, 0x00,		// _Reset
	0x02, 0x82,		// enable clock output
	0x04, 0xa0,		// 4mA
	0x05, 0x00,
	0x07, 0xa0,
	0x08, 0x00,
	0x14, 0x00,
};

uint16_t adc_init_data[] =  {
	0x0008, 0x03,	// Reset
	0x0008, 0x00,	// _Reset
	0x0005, 0x33,
	0x000B, 0x00,   // clock divide ratio
	0x000C, 0x02,   // chop mode  (0x04)
	0x000D, 0x00,   // Testmode
	0x0014, 0x01,   // output mode, 
	0x0015, 0x60,   // Termination
	0x0016, 0x03,   // Phase
	0x0019, 0x00,   // TestPattern1hi
	0x001a, 0xf0,   // TestPattern1lo
	0x001b, 0x00,   // TestPattern2hi
	0x001c, 0x00,   // TestPattern2lo
	0x0021, 0x30 	// 16-bit DDR, Two-Lane, 1 x Frame, bytewise Mode

};

typedef struct spi_s {
	int sdio;
	int sclk;
	int cs;
	int addrLen;
} SPI;

static SPI adc,dac,pga1,pga2,att1,att2;

int setPinDir(int pin, bool out) {
	int ret, fd;
	char buf[64];

	sprintf(buf, "/sys/class/gpio/gpio%d/direction", pin);
	fd = open(buf, O_WRONLY);
	if (fd<0) {
		printf("ioctl error: %s\n", strerror(errno));
		return fd;
	}

	if (out) 
		ret = write(fd, "out", 3); 
	else
		ret = write(fd, "in", 2); 

	close(fd);

	if (ret<= 0)
		return -1;
	return 0;
}

int setPin(int pin, int on) {
	int ret, fd;
	char buf[64];

	sprintf(buf, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(buf, O_WRONLY);
	if (fd<0) {
		printf("ioctl error: %s\n", strerror(errno));
		return  fd;
	}

	ret = write(fd, on>0 ? "1" : "0", 1); 
	close(fd);
	return ret;
}

int getPin (int pin) {
	int fd;
	char val='x';
	char buf[64];

	sprintf(buf, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(buf, O_RDONLY);
	if (fd<0) {
		printf("ioctl error: %s\n", strerror(errno));
		return fd;
	}

	if (read(fd, &val, 1)!=1) {
		printf("ioctl error: %s\n", strerror(errno));
		return  -1;
	};

	if(val == '0')
		return 0;
	if(val == '1')
		return 1;
	assert(0);
	return -1;
}

int initPin(int pin, bool on) {
	char buf[8];
	int fd,ret = 0;

	if (on)
		fd = open("/sys/class/gpio/export", O_WRONLY);
	else
		fd = open("/sys/class/gpio/unexport", O_WRONLY);

	if (fd<0) {
		printf("ioctl error: %s\n", strerror(errno));
		return  ret;
	}

	sprintf(buf, "%d", pin);
	if (write(fd, buf, strlen(buf)) != (int)strlen(buf))
		ret = -1;

	close(fd);
	return ret;
}

int writeByte(SPI spi, uint8_t out) {
	int i;
	uint8_t bit;

	setPinDir(spi.sdio, true);
	for (i=0;i<8;i++) {
		bit = out & 0x80;
		out <<= 1;
		setPin(spi.sclk, 0);
		setPin(spi.sdio, bit);
		usleep(TCLK);
		setPin(spi.sclk, 1);
		usleep(TCLK);
	}
	setPinDir(spi.sdio, false);
	return 0;
}

uint8_t readByte(SPI spi) {
	int i;
	uint8_t byte=0;
	int bit;

	setPinDir(spi.sdio, false);
	for (i=0;i<8;i++) {
		setPin(spi.sclk, 0);
		usleep(TCLK);
		byte <<= 1;
		bit = getPin(spi.sdio);
		if (bit>=0)
			byte |= bit;
		else {
			assert(0);
			return -1;
		}

		setPin(spi.sclk, 1);
		usleep(TCLK);
	}
	return byte;
}

int writeOneByte(SPI spi, uint8_t out) {
    setPin(spi.sclk, 0);
	setPin(spi.cs, 0);
	writeByte(spi, out);
	setPin(spi.cs, 1);
	return 0;
}


uint8_t readReg(SPI spi, uint16_t reg) {
	uint8_t val;

	setPin(spi.cs, 0);
	if (spi.addrLen == 16) {
		writeByte(spi, SPI_READ | SPI_ONE_BYTE | reg>>8);
		writeByte(spi, reg & 0xff);
	} else {
		writeByte(spi, SPI_READ | SPI_ONE_BYTE | reg);
	}
	val =  readByte(spi);
	setPin(spi.cs, 1);
	return val;
}

uint8_t writeReg(SPI spi, uint16_t reg, uint8_t val) {

	if ((reg & ~SPI_ADDR_MASK) != 0) {
		printf("addr to long %04x\n", reg);
		return 0;
	}

	setPin(spi.cs, 0);
	if (spi.addrLen == 16) {
		writeByte(spi, SPI_WRITE | SPI_ONE_BYTE | reg>>8);
		writeByte(spi, reg & 0xff);
	} else {
		writeByte(spi, SPI_WRITE | SPI_ONE_BYTE | reg);
	}
	writeByte(spi, val);
	setPin(spi.cs, 1);
	return 0;
}


int initSPI(SPI spi) {
	initPin(spi.sdio, true);
	setPinDir(spi.sdio, false);

	initPin(spi.sclk, true);
	setPinDir(spi.sclk, true);
	setPin(spi.sclk, 1);

	initPin(spi.cs, true);
	setPinDir(spi.cs, true);
	setPin(spi.cs, 1);
	return 0;
}

unsigned char reverse(unsigned char b) {
	b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
	b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
	b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
	return b;
}

void setRxGain(int gain, int rx) {
	uint8_t g;
	if (gain<-10 || gain>32)
		g=0;
	else
		g = gain+10;
	g = reverse (g<<2);

	if (rx==0)
		writeOneByte(pga1, 0x80 | g);
	else
		writeOneByte(pga2, 0x80 | g);
};

void setRxAtt(int att, int rx) {
	if (att>31) 
		att=31;
	att = att << 1;
	if (rx==0)
		writeOneByte(att1, att);
	else
		writeOneByte(att2, att);
};

void initSpi(void) {

	adc.sclk = ADC_SCLK;
	adc.sdio = ADC_SDIO;
	adc.cs   = ADC_CSB;
	adc.addrLen = 16;

	dac.sclk = DA_SCLK;
	dac.sdio = DA_SDIO;
	dac.cs   = DA_CS;
	dac.addrLen = 8;

	pga1.sclk = PGA_CLK;
	pga1.sdio = PGA_SDI;
	pga1.cs   = PGA_LCH1;
	pga1.addrLen = 8;
	
	pga2.sclk = PGA_CLK;
	pga2.sdio = PGA_SDI;
	pga2.cs   = PGA_LCH2;
	pga2.addrLen = 8;

	att1.sclk = ATT_CLK;
	att1.sdio = ATT_DAT;
	att1.cs   = ATT_A_LE;
	att1.addrLen = 8;

	att2.sclk = ATT_CLK;
	att2.sdio = ATT_DAT;
	att2.cs   = ATT_B_LE;
	att2.addrLen = 8;

	initSPI(adc);
	initSPI(dac);
	initSPI(pga1);
	initSPI(pga2);
	initSPI(att1);
	initSPI(att2);
}


void initADC() {
	size_t i;

	if (readReg(adc, ADC_CHIP_ID)!=0x8B) {
		printf ("AD9645 not found, exiting");
		return;
	};

	switch (readReg(adc, ADC_CHIP_GRADE) & 0x70) {
		case 0x40:
			printf ("detect AD9645 with 80 msps\n");
			break;
		case 0x50:
			printf ("detect AD9645 with 105 msps\n");
			break;
		case 0x60:
			printf ("detect AD9645 with 125 msps\n");
			break;
		default:
			printf ("detect AD9645 with unknown chipgrade\n");
	};

	for (i=0;i<sizeof(adc_init_data)/sizeof(adc_init_data[0]/2);i++) {
		int ret = writeReg(adc, adc_init_data[i*2], adc_init_data[i*2+1]);
		if (i==0)
			usleep(100000);
		if (ret) {
			printf("error write reg :  %04x %i\n", adc_init_data[i*2],ret);
			return;
		}
	}
}

void initDAC() {
	size_t i;

	printf("AD9717 Version %02x\n",readReg(dac, 31));

	for (i=0; i<sizeof(dac_init_data)/sizeof(dac_init_data[0])/2;i++) {
		int ret = writeReg(dac, dac_init_data[i*2], dac_init_data[i*2+1]);
		if (i==0)
			usleep(100000);
		if (ret) {
			printf("error write reg :  %04x %i\n", dac_init_data[i*2],ret);
			return;
		}
	}
}
