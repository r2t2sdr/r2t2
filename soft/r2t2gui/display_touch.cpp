#include <QDebug>
#include <math.h>
#include <assert.h>
#include "config.h"
#include "display_touch.h"
#include "ui_display_touch.h"
#include "lib.h"

#define DOWN	yp += ys + GRID_SIZE;
#define UP  	yp -= ys + GRID_SIZE;
#define RIGHT 	xp += xs + GRID_SIZE;
#define LEFT  	xp -= xs + GRID_SIZE;

#define DOWN2	yp += ys + GRID_SIZE/2;
#define UP2  	yp -= ys + GRID_SIZE/2;
#define RIGHT2 	xp += xs + GRID_SIZE/2;
#define LEFT2  	xp -= xs + GRID_SIZE/2;

#define MAX_SHORT 		0x8000
#define NEW_BUTTON(...) button[bIdx++] = {NULL, QStringList(), {},  __VA_ARGS__ ,xp,yp,xs,ys}
#define NEW_ANALOG(...) analog[aIdx++] = {NULL, __VA_ARGS__ ,xp,yp,xs,ys}
#define NEW_LABEL(...)  label[lIdx++] =  {NULL, __VA_ARGS__ ,0,xp,yp,xs,ys}

static int tvFreq = 28000000; // Transverter Freq

extern bool touchscreen;
extern bool defaults;
extern int layOut;

extern MenuEntry menuMain[MAX_MENU];
extern MenuEntry menuMain1[MAX_MENU];
extern MenuEntry menuAnt[MAX_MENU];
extern MenuEntry menuFreq[MAX_MENU];
extern MenuEntry menuMode[MAX_MENU];
extern MenuEntry menuDisplay[MAX_MENU];
extern MenuEntry menuAtt[MAX_MENU];
extern MenuEntry menuStep[MAX_MENU];
extern MenuEntry menuFilterRxLo[MAX_MENU];
extern MenuEntry menuFilterRxHi[MAX_MENU];
extern MenuEntry menuAGC[MAX_MENU];
extern MenuEntry menuRecallMemory[MAX_MENU];
extern MenuEntry menuStoreMemory[MAX_MENU];
extern MenuEntry menuSettings[MAX_MENU];

Entry entry[CMD_LAST][MAX_ENTRY] = {
	/* "NONE" */				{{"<-",0}},
	/* "AGC_DEC" */				{{"fixed",0},{"long",1},{"slow",2},{"medium",3}, {"fast",4}},
	/* "ANT" */					{{"1",0},{"2",1}},
	/* "DISP_MODE" */			{{"Water",0},{"FFT",1},{"Dual",2},{"Dual2",3}},
	/* "FFT_COMPLEX" */			{},
	/* "FFT_SAMPLE_RATE" */		{},
	/* "FFT_SIZE" */			{{"256",256},{"512",512},{"1024",1024},{"2048",2048},{"4096",4096}},
	/* "FFT_TIME" */			{},
	/* "RX_FILTER_GAIN" */		{},
	/* "TX_FILTER_GAIN" */		{},
	/* "FILTER_RX_CUT" */		{},
	/* "FILTER_RX_HI" */		{{"1500",1500},{"1800",1800},{"2100",2100},{"2400",2400},{"2700",2700},{"3000",3000},{"3300",3300},{"3600",3600},{"3900",3900}},
	/* "FILTER_RX_LO" */		{{"10",10},{"50",50},{"100",100},{"200",200},{"300",300},{"500",500},{"700",700},{"1000",1000}},
	/* "FILTER_TX_CUT" */		{},
	/* "FILTER_TX_HI" */		{},
	/* "FILTER_TX_LO" */		{},
	/* "GAIN" */				{{"ANALOG",101},{"",0}},
	/* "MODE" */				{{"LSB",0},{"USB",1},{"DSB",2},{"CW",3},{"AM",6},{"FM",5}},
	/* "PRESEL" */				{{"off",0,},{"auto",1},{"160 m",2},{"80 m",3},{"40 m",4},{"30 m",5},{"20 m",6},{"17 m",7},{"15 m",8},{"12 m",9},{"10 m",10},{"6 m",11},{"Transverter",12},{"-",13}},
	/* "PREAMP" */				{{"-30 dB",-30},{"-20 dB",-20},{"-10 dB",-10},{"-0 dB",0},{"10dB", 10},{"20dB",20}},
	/* "RSSI" */				{},
	/* "RX_FREQ" */				{},
	/* "SAMPLE_RATE */			{},
	/* "SMETER_MODE */			{{"rssi",0},{"level",1},{"power",2},{"swr",3}},
	/* "STEP */					{{"1 Hz",1},{"10 Hz",10},{"50 Hz",50},{"100 Hz",100},{"500 Hz",500},{"1 kHz",1000},{"5 kHz",5000},{"9 kHz",9000},{"10 kHz",10000},{"12.5 kHz",12500},{"20 kHz",20000},{"25 kHz",25000}},
	/* "SWR */					{},
	/* "TX */					{{"off",0},{"on",1}},
	/* "TX_RX */				{{"",0},{"",1}},
	/* "TX_FREQ */				{},
	/* "TX_POWER */				{{"ANALOG",256},{"",0}},
	/* "VOLUME */				{{"ANALOG",256},{"",0}},
	/* "MIC */					{{"ANALOG",256},{"",0}},
	/* "WATERFALL_MAX */		{{"ANALOG",WATERFALL_MAX},{"",-120}},
	/* "WATERFALL_MIN */		{{"ANALOG",WATERFALL_MIN},{"",-50}},
	/* "DIV1*/					{},
	/* "DIV2*/					{},
	/* "TWO_TONE_TEST */		{{"Audio",0},{"T2 -40 dB",1},{"T2 -30 dB",2},{"T2 -20 dB",3},{"T2 -10 dB",4},{"T2 0 dB",5},{"T1 0 dB",6}},
	/* "TX_DELAY */				{{"0ms",0},{"50ms",1},{"100ms",2},{"150ms",3},{"200ms",4},{"250ms",5},{"300ms",6},{"400ms",8},{"500ms",10},{"700ms",12},{"900ms",15}},
	/* "FULL_DUPLEX */			{{"OFF",0},{"HF",1},{"0s",2},{"1s",3},{"5s",4}},
	/* "NB_LEVEL */				{{"ANALOG",256},{"",0}},
	/* "INTERNAL */				{{"int",0},{"ext",1},{"off",2},{"HPSDR",3}},
	/* "NOTCH */				{{"off",0},{"1",1},{"2",2}},
	/* "SQUELCH */				{{"ANALOG",256},{"",0}},
	/* "DISP_COLOR */			{{"sw",0},{"color1",1},{"color2",2}},
	/* "AUDIO_COMP */			{{"off",0},{"6-6 dB",1},{"12-12 dB",2},{"20-20 dB",3},{"6-12 dB",4},{"12-20 dB",5}},
	/* "FREQ_OFFSET */			{{"off",0},{"144Mhz",144000000-tvFreq},{"148Mhz",148000000-tvFreq},{"430Mhz",430000000-tvFreq},{"432Mhz",432000000-tvFreq},{"434Mhz",434000000-tvFreq},{"436Mhz",436000000-tvFreq},{"438Mhz",438000000-tvFreq}},
	/* "INIT_END */				{},
	/* "EXIT */					{{"exit",0}},
	/* "MENU */					{},
	/* "NUMBER */				{{"0",0},{"1",1},{"2",2},{"3",3},{"4",4},{"5",5},{"6",6},{"7",7},{"8",8},{"9",9}},
	/* "STORE_MEMORY */ 		{{"0",0},{"1",1},{"2",2},{"3",3},{"4",4},{"5",5},{"6",6},{"7",7},{"8",8},{"9",9},{"10",10},{"11",11}},
	/* "RECONNECT */			{},
	/* "FREQ_STEP_DOWN */		{{"down",0}},
	/* "FREQ_STEP_UP */			{{"up",0}},
	/* "RECALL_MEMORY */		{{"0",0},{"1",1},{"2",2},{"3",3},{"4",4},{"5",5},{"6",6},{"7",7},{"8",8},{"9",9},{"10",10},{"11",11}},
	/* "BAND */					{{"no band",0},{"160 m",1},{"80 m",2},{"40 m",3},{"30 m",4},{"20 m",5},{"17 m",6},{"15 m",7},{"12 m",8},{"10 m",9},{"6 m",10}},
	/* "INPUT_FREQ */			{{"Hz",0},{"kHz",1},{"Mhz",2}},
	/* "ADC_OV */				{},
	/* "IN_LEVEL */				{},
	/* "TX_POWER_PEEK_LEVEL */  {},
	/* "TX_POWER_AV_LEVEL   */  {},
	/* "LAYOUT CHANGED */		{},
    /* "CONNECT" */             {},
	/* "LAST */				
};

MenuEntry menuAGC[MAX_MENU] = {
	{&entry[CMD_NONE][0],      CMD_NONE,    &menuMain},
	{&entry[CMD_AGC_DEC][0],   CMD_AGC_DEC, &menuMain},
	{&entry[CMD_AGC_DEC][1],   CMD_AGC_DEC, &menuMain},
	{&entry[CMD_AGC_DEC][2],   CMD_AGC_DEC, &menuMain},
};

MenuEntry menuFilterRxLo[MAX_MENU] = {
	{&entry[CMD_NONE][0],     CMD_NONE, &menuMain},
	{&entry[CMD_FILTER_RX_LO][0], CMD_FILTER_RX_LO, &menuMain},
	{&entry[CMD_FILTER_RX_LO][1], CMD_FILTER_RX_LO, &menuMain},
	{&entry[CMD_FILTER_RX_LO][2], CMD_FILTER_RX_LO, &menuMain},
	{&entry[CMD_FILTER_RX_LO][3], CMD_FILTER_RX_LO, &menuMain},
	{&entry[CMD_FILTER_RX_LO][4], CMD_FILTER_RX_LO, &menuMain},
	{&entry[CMD_FILTER_RX_LO][5], CMD_FILTER_RX_LO, &menuMain},
	{&entry[CMD_FILTER_RX_LO][6], CMD_FILTER_RX_LO, &menuMain},
};

MenuEntry menuFilterRxHi[MAX_MENU] = {
	{&entry[CMD_NONE][0],     CMD_NONE, &menuMain},
	{&entry[CMD_FILTER_RX_HI][0], CMD_FILTER_RX_HI, &menuMain},
	{&entry[CMD_FILTER_RX_HI][1], CMD_FILTER_RX_HI, &menuMain},
	{&entry[CMD_FILTER_RX_HI][2], CMD_FILTER_RX_HI, &menuMain},
	{&entry[CMD_FILTER_RX_HI][3], CMD_FILTER_RX_HI, &menuMain},
	{&entry[CMD_FILTER_RX_HI][4], CMD_FILTER_RX_HI, &menuMain},
	{&entry[CMD_FILTER_RX_HI][5], CMD_FILTER_RX_HI, &menuMain},
	{&entry[CMD_FILTER_RX_HI][6], CMD_FILTER_RX_HI, &menuMain},
	{&entry[CMD_FILTER_RX_HI][7], CMD_FILTER_RX_HI, &menuMain},
	{&entry[CMD_FILTER_RX_HI][8], CMD_FILTER_RX_HI, &menuMain},
};

MenuEntry menuStep[MAX_MENU] = {
	{&entry[CMD_NONE][0],     CMD_NONE, &menuMain},
	{&entry[CMD_STEP][0], CMD_STEP, &menuMain},
	{&entry[CMD_STEP][1], CMD_STEP, &menuMain},
	{&entry[CMD_STEP][2], CMD_STEP, &menuMain},
	{&entry[CMD_STEP][3], CMD_STEP, &menuMain},
	{&entry[CMD_STEP][4], CMD_STEP, &menuMain},
	{&entry[CMD_STEP][5], CMD_STEP, &menuMain},
	{&entry[CMD_STEP][6], CMD_STEP, &menuMain},
	{&entry[CMD_STEP][7], CMD_STEP, &menuMain},
	{&entry[CMD_STEP][8], CMD_STEP, &menuMain},
	{&entry[CMD_STEP][9], CMD_STEP, &menuMain},
	{&entry[CMD_STEP][10], CMD_STEP, &menuMain},
};

MenuEntry menuConnect1[MAX_MENU] = {
	{&entry[CMD_NONE][0],     CMD_NONE, &menuMain},
	{&entry[CMD_CONNECT][10], CMD_CONNECT, &menuMain},
	{&entry[CMD_CONNECT][11], CMD_CONNECT, &menuMain},
	{&entry[CMD_CONNECT][12], CMD_CONNECT, &menuMain},
	{&entry[CMD_CONNECT][13], CMD_CONNECT, &menuMain},
	{&entry[CMD_CONNECT][14], CMD_CONNECT, &menuMain},
	{&entry[CMD_CONNECT][15], CMD_CONNECT, &menuMain},
	{&entry[CMD_CONNECT][16], CMD_CONNECT, &menuMain},
	{&entry[CMD_CONNECT][17], CMD_CONNECT, &menuMain},
	{&entry[CMD_CONNECT][18], CMD_CONNECT, &menuMain},
	{&entry[CMD_CONNECT][19], CMD_CONNECT, &menuMain},
	{&entry[CMD_CONNECT][20], CMD_CONNECT, &menuMain},
};

Entry entryMore = {"more", 0};
MenuEntry menuConnect[MAX_MENU] = {
	{&entry[CMD_NONE][0],     CMD_NONE, &menuMain},
	{&entry[CMD_CONNECT][0], CMD_CONNECT, &menuMain},
	{&entry[CMD_CONNECT][1], CMD_CONNECT, &menuMain},
	{&entry[CMD_CONNECT][2], CMD_CONNECT, &menuMain},
	{&entry[CMD_CONNECT][3], CMD_CONNECT, &menuMain},
	{&entry[CMD_CONNECT][4], CMD_CONNECT, &menuMain},
	{&entry[CMD_CONNECT][5], CMD_CONNECT, &menuMain},
	{&entry[CMD_CONNECT][6], CMD_CONNECT, &menuMain},
	{&entry[CMD_CONNECT][7], CMD_CONNECT, &menuMain},
	{&entry[CMD_CONNECT][8], CMD_CONNECT, &menuMain},
	{&entry[CMD_CONNECT][9], CMD_CONNECT, &menuMain},
	{&entryMore, CMD_CONNECT, &menuConnect1},
};

MenuEntry menuAtt[MAX_MENU] = {
	{&entry[CMD_NONE][0],     CMD_NONE, &menuMain},
	{&entry[CMD_PREAMP][0], CMD_PREAMP, &menuMain},
	{&entry[CMD_PREAMP][1], CMD_PREAMP, &menuMain},
	{&entry[CMD_PREAMP][2], CMD_PREAMP, &menuMain},
	{&entry[CMD_PREAMP][3], CMD_PREAMP, &menuMain},
	{&entry[CMD_PREAMP][4], CMD_PREAMP, &menuMain},
	{&entry[CMD_PREAMP][5], CMD_PREAMP, &menuMain},
	{&entry[CMD_PREAMP][6], CMD_PREAMP, &menuMain},
};

MenuEntry menuDisplay[MAX_MENU] = {
	{&entry[CMD_NONE][0],     CMD_NONE, &menuMain},
	{&entry[CMD_DISP_MODE][0], CMD_DISP_MODE, &menuMain},
	{&entry[CMD_DISP_MODE][1], CMD_DISP_MODE, &menuMain},
};

MenuEntry menuAudioComp[MAX_MENU] = {
	{&entry[CMD_NONE][0],     CMD_NONE, &menuMain},
	{&entry[CMD_AUDIO_COMP][1],CMD_AUDIO_COMP, &menuMain},
	{&entry[CMD_AUDIO_COMP][2],CMD_AUDIO_COMP, &menuMain},
	{&entry[CMD_AUDIO_COMP][3],CMD_AUDIO_COMP, &menuMain},
};

MenuEntry menuMode[MAX_MENU] = {
	{&entry[CMD_NONE][0],     CMD_NONE, &menuMain},
	{&entry[CMD_MODE][0], CMD_MODE, &menuMain},
	{&entry[CMD_MODE][1], CMD_MODE, &menuMain},
	{&entry[CMD_MODE][2], CMD_MODE, &menuMain},
	{&entry[CMD_MODE][3], CMD_MODE, &menuMain},
	{&entry[CMD_MODE][4], CMD_MODE, &menuMain},
};

MenuEntry menuFreq[MAX_MENU] = {
	{&entry[CMD_NONE][0],     CMD_NONE, &menuMain},
	{&entry[CMD_NUMBER][0],  CMD_NUMBER, &menuFreq},
	{&entry[CMD_NUMBER][1],  CMD_NUMBER, &menuFreq},
	{&entry[CMD_NUMBER][2],  CMD_NUMBER, &menuFreq},
	{&entry[CMD_NUMBER][3],  CMD_NUMBER, &menuFreq},
	{&entry[CMD_NUMBER][4],  CMD_NUMBER, &menuFreq},
	{&entry[CMD_NUMBER][5],  CMD_NUMBER, &menuFreq},
	{&entry[CMD_NUMBER][6],  CMD_NUMBER, &menuFreq},
	{&entry[CMD_NUMBER][7],  CMD_NUMBER, &menuFreq},
	{&entry[CMD_NUMBER][8],  CMD_NUMBER, &menuFreq},
	{&entry[CMD_NUMBER][9],  CMD_NUMBER, &menuFreq},
	{&entry[CMD_INPUT_FREQ][1], CMD_INPUT_FREQ, &menuMain},
};

MenuEntry menuFreqOffset[MAX_MENU] = {
	{&entry[CMD_NONE][0],     CMD_NONE, &menuMain},
	{&entry[CMD_FREQ_OFFSET][0],  CMD_FREQ_OFFSET, &menuMain},
	{&entry[CMD_FREQ_OFFSET][1],  CMD_FREQ_OFFSET, &menuMain},
	{&entry[CMD_FREQ_OFFSET][2],  CMD_FREQ_OFFSET, &menuMain},
	{&entry[CMD_FREQ_OFFSET][3],  CMD_FREQ_OFFSET, &menuMain},
	{&entry[CMD_FREQ_OFFSET][4],  CMD_FREQ_OFFSET, &menuMain},
	{&entry[CMD_FREQ_OFFSET][5],  CMD_FREQ_OFFSET, &menuMain},
	{&entry[CMD_FREQ_OFFSET][6],  CMD_FREQ_OFFSET, &menuMain},
};

MenuEntry menuPresel[MAX_MENU] = {
	{&entry[CMD_NONE][0],     CMD_NONE, &menuMain},
	{&entry[CMD_PRESEL][0],  CMD_PRESEL, &menuMain},
	{&entry[CMD_PRESEL][1],  CMD_PRESEL, &menuMain},
	{&entry[CMD_PRESEL][2],  CMD_PRESEL, &menuMain},
	{&entry[CMD_PRESEL][3],  CMD_PRESEL, &menuMain},
	{&entry[CMD_PRESEL][4],  CMD_PRESEL, &menuMain},
	{&entry[CMD_PRESEL][5],  CMD_PRESEL, &menuMain},
	{&entry[CMD_PRESEL][6],  CMD_PRESEL, &menuMain},
	{&entry[CMD_PRESEL][7],  CMD_PRESEL, &menuMain},
	{&entry[CMD_PRESEL][8],  CMD_PRESEL, &menuMain},
	{&entry[CMD_PRESEL][9],  CMD_PRESEL, &menuMain},
	{&entry[CMD_PRESEL][10],  CMD_PRESEL, &menuMain},
};

MenuEntry menuRecallMemory[MAX_MENU] = {
	{&entry[CMD_NONE][0],     CMD_NONE, &menuMain},
	{&entry[CMD_RECALL_MEMORY][1],  CMD_RECALL_MEMORY, &menuMain},
	{&entry[CMD_RECALL_MEMORY][2],  CMD_RECALL_MEMORY, &menuMain},
	{&entry[CMD_RECALL_MEMORY][3],  CMD_RECALL_MEMORY, &menuMain},
	{&entry[CMD_RECALL_MEMORY][4],  CMD_RECALL_MEMORY, &menuMain},
	{&entry[CMD_RECALL_MEMORY][5],  CMD_RECALL_MEMORY, &menuMain},
	{&entry[CMD_RECALL_MEMORY][6],  CMD_RECALL_MEMORY, &menuMain},
	{&entry[CMD_RECALL_MEMORY][7],  CMD_RECALL_MEMORY, &menuMain},
	{&entry[CMD_RECALL_MEMORY][8],  CMD_RECALL_MEMORY, &menuMain},
	{&entry[CMD_RECALL_MEMORY][9],  CMD_RECALL_MEMORY, &menuMain},
	{&entry[CMD_RECALL_MEMORY][10],  CMD_RECALL_MEMORY, &menuMain},
	{&entry[CMD_RECALL_MEMORY][11],  CMD_RECALL_MEMORY, &menuMain},
};

MenuEntry menuStoreMemory[MAX_MENU] = {
	{&entry[CMD_NONE][0],     CMD_NONE, &menuMain},
	{&entry[CMD_STORE_MEMORY][1],  CMD_STORE_MEMORY, &menuMain},
	{&entry[CMD_STORE_MEMORY][2],  CMD_STORE_MEMORY, &menuMain},
	{&entry[CMD_STORE_MEMORY][3],  CMD_STORE_MEMORY, &menuMain},
	{&entry[CMD_STORE_MEMORY][4],  CMD_STORE_MEMORY, &menuMain},
	{&entry[CMD_STORE_MEMORY][5],  CMD_STORE_MEMORY, &menuMain},
	{&entry[CMD_STORE_MEMORY][6],  CMD_STORE_MEMORY, &menuMain},
	{&entry[CMD_STORE_MEMORY][7],  CMD_STORE_MEMORY, &menuMain},
	{&entry[CMD_STORE_MEMORY][8],  CMD_STORE_MEMORY, &menuMain},
	{&entry[CMD_STORE_MEMORY][9],  CMD_STORE_MEMORY, &menuMain},
	{&entry[CMD_STORE_MEMORY][10],  CMD_STORE_MEMORY, &menuMain},
	{&entry[CMD_STORE_MEMORY][11],  CMD_STORE_MEMORY, &menuMain},
};

MenuEntry menuTxDelay[MAX_MENU] = {
	{&entry[CMD_NONE][0],     CMD_NONE, &menuMain},
	{&entry[CMD_TX_DELAY][0], CMD_TX_DELAY, &menuMain},
	{&entry[CMD_TX_DELAY][1], CMD_TX_DELAY, &menuMain},
	{&entry[CMD_TX_DELAY][2], CMD_TX_DELAY, &menuMain},
	{&entry[CMD_TX_DELAY][3], CMD_TX_DELAY, &menuMain},
	{&entry[CMD_TX_DELAY][4], CMD_TX_DELAY, &menuMain},
	{&entry[CMD_TX_DELAY][5], CMD_TX_DELAY, &menuMain},
	{&entry[CMD_TX_DELAY][6], CMD_TX_DELAY, &menuMain},
	{&entry[CMD_TX_DELAY][7], CMD_TX_DELAY, &menuMain},
	{&entry[CMD_TX_DELAY][8], CMD_TX_DELAY, &menuMain},
	{&entry[CMD_TX_DELAY][9], CMD_TX_DELAY, &menuMain},
	{&entry[CMD_TX_DELAY][10], CMD_TX_DELAY, &menuMain},
};

Entry settingsEntry[] = {{"TxDelay",0},{"TxCompr", 0}};
MenuEntry menuSettings[MAX_MENU] = {
	{&entry[CMD_NONE][0], CMD_NONE, &menuMain},
	{&settingsEntry[0], 0, &menuTxDelay},
	{&settingsEntry[1], 0, &menuAudioComp},
};

MenuEntry menuAnt[MAX_MENU] = {
	{&entry[CMD_NONE][0],     CMD_NONE, &menuMain},
	{&entry[CMD_ANT][0],  CMD_ANT, &menuMain},
	{&entry[CMD_ANT][1],  CMD_ANT, &menuMain},
};

MenuEntry menuTxSrc[MAX_MENU] = {
	{&entry[CMD_TWO_TONE_TEST][0],     CMD_TWO_TONE_TEST, &menuMain},
	{&entry[CMD_TWO_TONE_TEST][1],     CMD_TWO_TONE_TEST, &menuMain},
	{&entry[CMD_TWO_TONE_TEST][2],     CMD_TWO_TONE_TEST, &menuMain},
	{&entry[CMD_TWO_TONE_TEST][3],     CMD_TWO_TONE_TEST, &menuMain},
	{&entry[CMD_TWO_TONE_TEST][4],     CMD_TWO_TONE_TEST, &menuMain},
	{&entry[CMD_TWO_TONE_TEST][5],     CMD_TWO_TONE_TEST, &menuMain},
};

MenuEntry menuBand[MAX_MENU] = {
	{&entry[CMD_NONE][0],     CMD_NONE, &menuMain},
	{&entry[CMD_BAND][1], CMD_BAND, &menuMain},
	{&entry[CMD_BAND][2], CMD_BAND, &menuMain},
	{&entry[CMD_BAND][3], CMD_BAND, &menuMain},
	{&entry[CMD_BAND][4], CMD_BAND, &menuMain},
	{&entry[CMD_BAND][5], CMD_BAND, &menuMain},
	{&entry[CMD_BAND][6], CMD_BAND, &menuMain},
	{&entry[CMD_BAND][7], CMD_BAND, &menuMain},
	{&entry[CMD_BAND][8], CMD_BAND, &menuMain},
	{&entry[CMD_BAND][9], CMD_BAND, &menuMain},
	{&entry[CMD_BAND][10], CMD_BAND, &menuMain},
};

MenuEntry menuNotch[MAX_MENU] = {
	{&entry[CMD_NONE][0],     CMD_NONE, &menuMain},
	{&entry[CMD_NOTCH][0],  CMD_NOTCH, &menuMain},
	{&entry[CMD_NOTCH][1],  CMD_NOTCH, &menuMain},
	{&entry[CMD_NOTCH][2],  CMD_NOTCH, &menuMain},
};

Entry mainMenu[] = {{"MStore", 0},{"Settings", 0},  {"TX-Src", 0}, {"Notch", 0}, {"Presel", 0}, {"FreqOffs", 0}, {"Server", 0}};
MenuEntry menuMain1[MAX_MENU] = {
	{&entry[CMD_NONE][0],     CMD_NONE, &menuMain},
	{ &mainMenu[0], 0, &menuStoreMemory},
	{ &mainMenu[1], 0, &menuSettings},
	{ &mainMenu[2], 0, &menuTxSrc},
	{ &mainMenu[3], 0, &menuNotch},
	{ &mainMenu[4], 0, &menuPresel},
	{ &mainMenu[5], 0, &menuFreqOffset},
	{ &mainMenu[6], 0, &menuConnect},
};

Entry mainMenu1[] = {{"Freq", 0}, {"Step", 0}, {"Band", 0}, {"Mode", 0}, {"Display", 0}, {"Ant", 0}, {"Att", 0}, {"Fi Lo", 0}, {"Fi Hi", 0}, {"AGC", 0}, {"MRecall", 0}, {"more", 0}};
MenuEntry menuMain[MAX_MENU] = {
	{ &mainMenu1[0], 0, &menuFreq},
	{ &mainMenu1[1], 0, &menuStep},
	{ &mainMenu1[2], 0, &menuBand},
	{ &mainMenu1[3], 0, &menuMode},
	{ &mainMenu1[4], 0, &menuDisplay},
	{ &mainMenu1[5], 0, &menuAnt},
	{ &mainMenu1[6], 0, &menuAtt},
	{ &mainMenu1[7], 0, &menuFilterRxLo},
	{ &mainMenu1[8], 0, &menuFilterRxHi},
	{ &mainMenu1[9], 0, &menuAGC},
	{ &mainMenu1[10], 0, &menuRecallMemory},
	{ &mainMenu1[11], 0, &menuMain1},
}; 


Display_touch::Display_touch(QSettings *settings, QWidget* /*parent*/) : Display_base(), settings(settings) , ui(new Ui::Display_touch){

	initReady = false;
	timer = new QTimer();

	fftSize=FFT_SIZE;
	fftSampRate = RX_CLOCK; 
	rxFreq = 7100000;
	txFreq = 7100000;
	update = true;
	inputMode = false;
	inputVal = 0;
	txrx = true;
	div1 = 8;
	div2 = 40;
	ptt = 0;
	smtr = NULL;
	fullScreen = 0;
	volume = 0;
	encSens = 5;
	encDelta = 0;
	freqOffset = 0;

	for(int i=0;i<arraysize(mem);i++) {
		mem[i].name = QString("mem %1").arg(i+1);
		mem[i].mode = MODE_LSB;
		mem[i].rxFreq = 7000000;
		mem[i].txFreq = 7000000; 
		mem[i].filterLo = 100;
		mem[i].filterHi = 2700;
		mem[i].preamp = 0;
		mem[i].ant = 0;
	}
	
	for(int i=0;i<arraysize(memBand);i++) {
		memBand[i].name = QString("memBand %1").arg(i+1);
		memBand[i].mode = MODE_LSB;
		if (i>1)
			memBand[i].mode = MODE_USB;
		memBand[i].rxFreq = bandMin[i];
		memBand[i].txFreq = bandMin[i]; 
		memBand[i].filterLo = 100;
		memBand[i].filterHi = 2700;
		memBand[i].preamp = 0;
		memBand[i].ant = 0;
	}

	for (int i=0;i<arraysize(button);i++) {
		button[i].name = "unused";
		button[i].xPos = 0;
		button[i].yPos = 0;
		button[i].xSize = 0;
		button[i].ySize = 0;
		button[i].pos = 0;
		button[i].cmd = 0;
	}
	for (int i=0;i<arraysize(analog);i++) {
		analog[i].name = "unused";
		analog[i].xPos = 0;
		analog[i].yPos = 0;
		analog[i].xSize = 0;
		analog[i].ySize = 0;
		analog[i].min = 0;
		analog[i].max = 0;
		analog[i].pos = 0;
		analog[i].cmd = 0;
	}
	for (int i=0;i<arraysize(label);i++) {
		label[i].name = "unused";
		label[i].xPos = 0;
		label[i].yPos = 0;
		label[i].xSize = 0;
		label[i].ySize = 0;
		label[i].cmd = 0;
	}

    for (int i=0;i<MAX_ENTRY;i++) {
        sprintf(entry[CMD_CONNECT][i].name, "-");
        entry[CMD_CONNECT][i].par = i;
    }

	memset(cmdStore, 0, sizeof(cmdStore));
	readSettings();

    ui->setupUi(this);
	show();
	if (touchscreen)
		this->setWindowState(Qt::WindowFullScreen);
	//resize(DISP_X_SIZE*GRID_SIZE,DISP_Y_SIZE*GRID_SIZE);
	//ui->graphicsView->resize(DISP_X_SIZE*GRID_SIZE, DISP_Y_SIZE*GRID_SIZE);
	ui->graphicsView->setMouseTracking(true);

	QLinearGradient linearGrad(QPointF(0, -0), QPointF(DSIZE_X, DSIZE_X));
	linearGrad.setColorAt(0, QColor(getSettings(settings,"display/colorBackground1","#1f1f1f")));
	linearGrad.setColorAt(1, QColor(getSettings(settings,"display/colorBackground2","#4b4b4b")));

	scene = new QGraphicsScene();
	// ui->graphicsView->setRenderHint(QPainter::Antialiasing);
	scene->setBackgroundBrush(linearGrad);
	scene->setItemIndexMethod(QGraphicsScene::NoIndex);

	fftGraph = new FFTGraph(settings, 0, 0);
	scene->addItem(fftGraph);
	filterGraph = new FilterGraph(settings, 0, 0);
	scene->addItem(filterGraph);

	smtr = new SMeter(settings);
	scene->addItem(smtr);

	rxfreq = new Numeric(settings, "RX", 0, 0, rxFreq,8,0,RX_CLOCK/2) ;
	scene->addItem(rxfreq);
	txfreq = new Numeric(settings, "TX", 0, 0, txFreq,8,0,RX_CLOCK/2) ;
	scene->addItem(txfreq);
	input = new Numeric(settings, "",0,0,0,6,0,999999) ;
	scene->addItem(input);
	clock = new Clock(settings, 0,0) ;
	scene->addItem(clock);

	ui->graphicsView->setScene(scene);

	fftGraph->setMin(-120);
	fftGraph->setMax(-50);

	setLayout(layOut);

	connect(timer, SIGNAL(timeout()), this, SLOT(updateOn()));
	connect(smtr, SIGNAL(pressed(int,int)), this, SLOT(smtrPressed(int,int)));
	connect(rxfreq, SIGNAL(changed(int)), this, SLOT(setFreq(int)));
	connect(txfreq, SIGNAL(changed(int)), this, SLOT(setTXFreq(int)));
	connect(filterGraph, SIGNAL(freqChanged(int)), this, SLOT(setFreq(int)));

	initReady = true;
	resize(settings->value("Size",QSize(DSIZE_X,DSIZE_Y)).toSize()); 
	resizeDisp(settings->value("Size",QSize(DSIZE_X,DSIZE_Y)).toSize()); 
}


Display_touch::~Display_touch() {
	writeSettings();
    delete ui;
}

void Display_touch::start() {
}

void Display_touch::setServers(QStringList servers) {

    QStringList::iterator i;
    int k=0;
    for (i=servers.begin(); i!=servers.end(); ++i) {
        std::string s = (*i).toStdString();;
        memset(entry[CMD_CONNECT][k].name, 0, 32);
        strncpy(entry[CMD_CONNECT][k++].name, s.data(), std::min((int)s.size(), (int)32));
    }
    button[connectButtonIdx].buttonText = servers;
    button[connectButtonIdx].button->doUpdate();
}

void Display_touch::resizeEvent(QResizeEvent* event) {
	//QSize s = QSize(event->size().height()*1.6, event->size().height());
	//resize(s);
	//resizeDisp(s);
	resizeDisp(event->size());
}

void Display_touch::resizeDisp(QSize size) {
	int xs = size.width();
	int ys = size.height();

	if (!initReady)
		return;
#if 0
#ifndef ANDROID
	if (layOut == 0 && xs>=1072)
		setLayout(2);
	if (layOut == 1 && xs< 1072)
		setLayout(1);
#endif
#endif

	if (!smtr)
		return;
	scene->setSceneRect(0,0,xs-4,ys-4);

	// Buttons

	switch (layOut) {
		case 0:
			for (int i=0;i<arraysize(button);i++) {
				if (button[i].cmd != CMD_NONE) {
					button[i].button->setPos(button[i].xPos*xs/DSIZE_X, button[i].yPos*ys/DSIZE_Y);
					button[i].button->setScale(1.0*xs/DSIZE_X);
				}
				if (analog[i].cmd != CMD_NONE) {
					analog[i].analog->setPos(analog[i].xPos*xs/DSIZE_X, analog[i].yPos*ys/DSIZE_Y);
					analog[i].analog->setScale(1.0*xs/DSIZE_X);
				}
				if (label[i].cmd != CMD_NONE) {
					label[i].label->setPos(label[i].xPos*xs/DSIZE_X, label[i].yPos*ys/DSIZE_Y);
					label[i].label->setScale(1.0*xs/DSIZE_X);
				}
			}
			for (int i=0;i<ITEM_MAX;i++) {
				if (items[i].it) {
					items[i].it->setPos (items[i]. xPos*xs/DSIZE_X, items[i]. yPos*ys/DSIZE_Y);
					items[i].it->setSize(items[i].xSize*xs/DSIZE_X, items[i].ySize*ys/DSIZE_Y);
				}
			}
			break;

		case 1:
			for (int i=0;i<arraysize(button);i++) {
				if (button[i].cmd != CMD_NONE) 
					button[i].button->setPos(button[i].xPos, ys-DSIZE_Y + button[i].yPos);
				if (analog[i].cmd != CMD_NONE) 
					analog[i].analog->setPos(analog[i].xPos, ys-DSIZE_Y + analog[i].yPos);
				if (label[i].cmd != CMD_NONE) 
					label[i].label->setPos(label[i].xPos, ys-DSIZE_Y + label[i].yPos);
			}
			for (int i=0;i<ITEM_MAX;i++) {
				if (items[i].it) {
					if (items[i].yPos >= 30*GRID_SIZE) {
						items[i].it->setPos (items[i].xPos, ys-DSIZE_Y + items[i].yPos);
						items[i].it->setSize(items[i].xSize, items[i].ySize);
					} else {
						items[i].it->setPos (items[i]. xPos*xs/DSIZE_X, items[i]. yPos*ys/DSIZE_Y);
						items[i].it->setSize(items[i].xSize*xs/DSIZE_X, items[i].ySize*ys/DSIZE_Y);
					}
				}
			}
			break;
	}
}

void Display_touch::setLayout(int l) {
	int aIdx = 0;
	int bIdx = 0;
	int lIdx = 0;
	int xp,yp,xs,ys;

	layOut = l;

	for (int i=0;i<arraysize(button);i++) {
		if (analog[i].cmd != CMD_NONE) {
			scene->removeItem(analog[i].analog);
			delete analog[i].analog;
			analog[i].cmd = CMD_NONE;
		}
		if (button[i].cmd != CMD_NONE) {
			scene->removeItem(button[i].button);
			delete button[i].button;
			button[i].cmd = CMD_NONE;
		}
		if (label[i].cmd != CMD_NONE) {
			scene->removeItem(label[i].label);
			delete label[i].label;
			label[i].cmd = CMD_NONE;
		}
	}

	switch (layOut) {
		case 0:
		default:
			items[ITEM_SMTR]        = {smtr,        16*GRID_SIZE,  7*GRID_SIZE, 74*GRID_SIZE,  1*GRID_SIZE};
			items[ITEM_RXFREQ]      = {rxfreq,      36*GRID_SIZE,  7*GRID_SIZE,  1*GRID_SIZE,  1*GRID_SIZE};
			items[ITEM_TXFREQ]      = {txfreq,      36*GRID_SIZE,  7*GRID_SIZE, 37*GRID_SIZE,  1*GRID_SIZE};
			items[ITEM_INPUT]       = {input,       12*GRID_SIZE,  3*GRID_SIZE,  1*GRID_SIZE,  9*GRID_SIZE};
			items[ITEM_FILTERGRAPH] = {filterGraph, 64*GRID_SIZE, 29*GRID_SIZE,  1*GRID_SIZE, 13*GRID_SIZE};
			items[ITEM_FFTGRAPH]    = {fftGraph,    64*GRID_SIZE, 29*GRID_SIZE,  1*GRID_SIZE, 13*GRID_SIZE};
			items[ITEM_CLOCK]       = {clock,       17*GRID_SIZE,  3*GRID_SIZE, 14*GRID_SIZE,  9*GRID_SIZE};

			xp = 32*GRID_SIZE; yp = 9*GRID_SIZE; xs = 7*GRID_SIZE; ys = 3*GRID_SIZE; 
			NEW_BUTTON("Color", 0, CMD_DISP_COLOR); RIGHT;
			NEW_BUTTON("Zoom", 2, CMD_FFT_SIZE); RIGHT; 
			NEW_BUTTON("Notch", 0, CMD_NOTCH); RIGHT; 
			NEW_BUTTON("FDX", 1, CMD_FULL_DUPLEX); RIGHT;
			NEW_BUTTON("TX=RX", 1, CMD_TX_RX);

			xp = 66*GRID_SIZE; yp = 13*GRID_SIZE; xs = 5*GRID_SIZE; ys = 14*GRID_SIZE; 
			NEW_ANALOG("Max", -120,WATERFALL_MAX,-50,CMD_WATERFALL_MAX); DOWN;
			NEW_ANALOG("Min", WATERFALL_MIN,-50,-120,CMD_WATERFALL_MIN); 
			xp = 72*GRID_SIZE; yp =  9*GRID_SIZE; xs = 12*GRID_SIZE; ys =  6*GRID_SIZE; 

			NEW_BUTTON("Filter hi", 4, CMD_FILTER_RX_HI); DOWN;
			NEW_BUTTON("Filter lo", 3, CMD_FILTER_RX_LO); DOWN;
			NEW_BUTTON("Step", 2, CMD_STEP); DOWN;
			xs = 6*GRID_SIZE;
			NEW_BUTTON("", 0, CMD_FREQ_STEP_DOWN); xp+=xs;
			NEW_BUTTON("", 0, CMD_FREQ_STEP_UP); xp-=xs; DOWN;
			xs = 12*GRID_SIZE; ys =  7*GRID_SIZE; 
			NEW_BUTTON("TX-Src", 0, CMD_TWO_TONE_TEST);

			xp = 85*GRID_SIZE; yp =  9*GRID_SIZE; xs = 12*GRID_SIZE; ys =  6*GRID_SIZE; 
			NEW_BUTTON("Mode", 0, CMD_MODE); DOWN;
			NEW_BUTTON("Antenna", 0, CMD_ANT); DOWN;
			NEW_BUTTON("Preamp", 0, CMD_PREAMP); DOWN;
			NEW_BUTTON("AGC",  1, CMD_AGC_DEC); DOWN;
			NEW_BUTTON("Display", 0, CMD_DISP_MODE); DOWN;

			xs = 12*GRID_SIZE; ys =  7*GRID_SIZE; 
			NEW_BUTTON("TX", 0, CMD_TX);

			xp = 89*GRID_SIZE; yp = 1*GRID_SIZE; xs = 8*GRID_SIZE; ys = 7 *GRID_SIZE; 
			NEW_BUTTON("", 0, CMD_SMETER_MODE);

			xp =  1*GRID_SIZE; yp = 43*GRID_SIZE; xs =  7*GRID_SIZE; ys = 4*GRID_SIZE; 
			NEW_BUTTON("Intern", 0, CMD_INTERNAL); yp+=ys;;
			NEW_BUTTON("OFF", 0, CMD_EXIT); RIGHT; 

			yp = 43*GRID_SIZE; xs = 7*GRID_SIZE; ys=8*GRID_SIZE;
			NEW_ANALOG("Volume", 0,255,30,CMD_VOLUME); RIGHT;
			NEW_ANALOG("Mic", 0,255,0,CMD_MIC); RIGHT;
			NEW_ANALOG("TX",0,255,30,CMD_TX_POWER); RIGHT;
			NEW_ANALOG("Gain",0,100,0,CMD_GAIN);  RIGHT;
			NEW_ANALOG("NB",0,255,255,CMD_NB_LEVEL);  RIGHT;
			NEW_ANALOG("Squelch",0,255,255,CMD_SQUELCH);  RIGHT;

			xs =12*GRID_SIZE, ys=2*GRID_SIZE;
			NEW_LABEL("Gain",CMD_GAIN); yp+=ys;
			NEW_LABEL("Vol",CMD_VOLUME); yp+=ys;
			NEW_LABEL("Mode",CMD_MODE); yp+=ys;
			NEW_LABEL("SWR",CMD_SWR);

			xp = 1*GRID_SIZE; yp = 52*GRID_SIZE; xs = 7*GRID_SIZE; ys =  5*GRID_SIZE; 
			menuStartIdx = bIdx;
			NEW_BUTTON("", 0, CMD_MENU); RIGHT;
			NEW_BUTTON("", 0, CMD_MENU); RIGHT;
			NEW_BUTTON("", 0, CMD_MENU); RIGHT;
			NEW_BUTTON("", 0, CMD_MENU); RIGHT;
			NEW_BUTTON("", 0, CMD_MENU); RIGHT;
			NEW_BUTTON("", 0, CMD_MENU); RIGHT;
			NEW_BUTTON("", 0, CMD_MENU); RIGHT;
			NEW_BUTTON("", 0, CMD_MENU); RIGHT;
			NEW_BUTTON("", 0, CMD_MENU); RIGHT;
			NEW_BUTTON("", 0, CMD_MENU); RIGHT;
			NEW_BUTTON("", 0, CMD_MENU); RIGHT;
			NEW_BUTTON("", 0, CMD_MENU); RIGHT;

			curMenu = &menuMain;
			for (int i=0;i<MAX_MENU;i++) {
				button[menuStartIdx + i].buttonText.append((*curMenu)[i].entry->name);
				button[menuStartIdx + i].buttonValue[0]=i;
			}
			break;

		case 1:
			items[ITEM_RXFREQ]      = {rxfreq,      30*GRID_SIZE,  6*GRID_SIZE,  1*GRID_SIZE, 38*GRID_SIZE};
			items[ITEM_TXFREQ]      = {txfreq,      30*GRID_SIZE,  6*GRID_SIZE, 32*GRID_SIZE, 38*GRID_SIZE};
			items[ITEM_INPUT]       = {input,       17*GRID_SIZE,  3*GRID_SIZE, 63*GRID_SIZE, 38*GRID_SIZE};
			items[ITEM_CLOCK]       = {clock,       17*GRID_SIZE,  2*GRID_SIZE, 63*GRID_SIZE, 42*GRID_SIZE};
			items[ITEM_SMTR]        = {smtr,        16*GRID_SIZE,  6*GRID_SIZE, 81*GRID_SIZE, 38*GRID_SIZE};
			items[ITEM_FILTERGRAPH] = {filterGraph,100*GRID_SIZE, 60*GRID_SIZE,  0*GRID_SIZE,  0*GRID_SIZE};
			items[ITEM_FFTGRAPH]    = {fftGraph,   100*GRID_SIZE, 60*GRID_SIZE,  0*GRID_SIZE,  0*GRID_SIZE};

			xp =  1*GRID_SIZE; yp = 35*GRID_SIZE; xs =  80*GRID_SIZE; ys = 2*GRID_SIZE; 
			NEW_LABEL("Server",CMD_CONNECT); 

			xp =  1*GRID_SIZE; yp = 45*GRID_SIZE; xs =  8*GRID_SIZE; ys = 4*GRID_SIZE; 
			DOWN2;
			DOWN2;
			NEW_BUTTON("OFF", 0, CMD_EXIT); RIGHT2; 

			UP2;UP2;	

			xs = 5*GRID_SIZE; ys =13*GRID_SIZE; 
			NEW_ANALOG("Vol", 0,255,30,CMD_VOLUME); RIGHT2;
			NEW_ANALOG("Mic", 0,255,0,CMD_MIC); RIGHT2;
			// NEW_ANALOG("TX",0,255,30,CMD_TX_POWER); RIGHT2;
			NEW_ANALOG("--",0,255,30,CMD_TX_POWER); RIGHT2;
			// NEW_ANALOG("Gain",0,100,0,CMD_GAIN);  RIGHT2;
			NEW_ANALOG("--",0,100,0,CMD_GAIN);  RIGHT2;
			// NEW_ANALOG("NB",0,255,255,CMD_NB_LEVEL);  RIGHT2;
			NEW_ANALOG("--",0,255,255,CMD_NB_LEVEL);  RIGHT2;
			// NEW_ANALOG("Squ",0,255,255,CMD_SQUELCH);  RIGHT2;
			NEW_ANALOG("--",0,255,255,CMD_SQUELCH);  RIGHT2;
			NEW_ANALOG("Max", -120,WATERFALL_MAX,-50,CMD_WATERFALL_MAX); RIGHT2;
			NEW_ANALOG("Min", WATERFALL_MIN,-50,-120,CMD_WATERFALL_MIN); RIGHT2; 

			xs = 8*GRID_SIZE; ys =  4*GRID_SIZE; 
			NEW_BUTTON("Zoom", 2, CMD_FFT_SIZE); DOWN2; 
			NEW_BUTTON("Display", 0, CMD_DISP_MODE); DOWN2;
			NEW_BUTTON("Color", 0, CMD_DISP_COLOR); DOWN2;

			RIGHT2;UP2;UP2;UP2;
			NEW_BUTTON("Mode", 0, CMD_MODE); DOWN2;
			NEW_BUTTON("Filter hi", 4, CMD_FILTER_RX_HI); DOWN2;
			NEW_BUTTON("Filter lo", 3, CMD_FILTER_RX_LO); DOWN2;

			RIGHT2;UP2;UP2;UP2;
			NEW_BUTTON("Preamp", 0, CMD_PREAMP); DOWN2;
			NEW_BUTTON("AGC",  1, CMD_AGC_DEC); DOWN2;
			NEW_BUTTON("Notch", 0, CMD_NOTCH); DOWN2; 

			RIGHT2;UP2;UP2;UP2;
			NEW_BUTTON("Step", 2, CMD_STEP); DOWN2;
			NEW_BUTTON("", 0, CMD_FREQ_STEP_UP); DOWN2;
			NEW_BUTTON("", 0, CMD_FREQ_STEP_DOWN); DOWN2;

			RIGHT2;UP2;UP2;UP2;
			NEW_BUTTON("Antenna", 0, CMD_ANT); DOWN2;
			NEW_BUTTON("S-Meter", 0, CMD_SMETER_MODE); DOWN2;
            connectButtonIdx = bIdx;
			xs =  8*GRID_SIZE*2+1; 
			NEW_BUTTON("Server", 1, CMD_CONNECT); DOWN2;
			xs =  8*GRID_SIZE; 

			RIGHT2;UP2;UP2;UP2;
			DOWN2;
			NEW_BUTTON("PTT", 0, CMD_TX); DOWN2;
			DOWN2;

			menuStartIdx = bIdx;

			xp += GRID_SIZE*2 ; RIGHT2; UP2;
			NEW_BUTTON("", 0, CMD_MENU); RIGHT2; 
			NEW_BUTTON("", 0, CMD_MENU); 

			LEFT2; UP2;
			NEW_BUTTON("", 0, CMD_MENU); RIGHT2; 
			NEW_BUTTON("", 0, CMD_MENU); RIGHT2; 
			NEW_BUTTON("", 0, CMD_MENU); 

			LEFT2; LEFT2; UP2;
			NEW_BUTTON("", 0, CMD_MENU); RIGHT2; 
			NEW_BUTTON("", 0, CMD_MENU); RIGHT2; 
			NEW_BUTTON("", 0, CMD_MENU); 
			
			LEFT2; LEFT2; UP2;
			NEW_BUTTON("", 0, CMD_MENU); RIGHT2; 
			NEW_BUTTON("", 0, CMD_MENU); RIGHT2; 
			NEW_BUTTON("", 0, CMD_MENU); 

			DOWN2; DOWN2; DOWN2;
			NEW_BUTTON("", 0, CMD_MENU); 

			curMenu = &menuMain;
			for (int i=0;i<MAX_MENU;i++) {
				button[menuStartIdx + i].buttonText.append((*curMenu)[i].entry->name);
				button[menuStartIdx + i].buttonValue[0]=i;
			}
			break;
	}

	// Buttons
	for (int i=0;i<arraysize(button);i++) {
		if (button[i].cmd == 0)
			continue;
		button[i].button = new TextButton(settings, &button[i]);		
		scene->addItem(button[i].button);
		button[i].button->setPos(button[i].xPos, button[i].yPos);
		connect(button[i].button, SIGNAL(pressed(int,int)), this, SLOT(buttonPressed(int,int)));
	}

	// Analog
	for (int i=0;i<arraysize(analog);i++) {
		if (analog[i].cmd == 0)
			continue;
		analog[i].analog = new Analog(settings, &analog[i]);		
		scene->addItem(analog[i].analog);
		analog[i].analog->setPos(analog[i].xPos, analog[i].yPos);
		connect(analog[i].analog, SIGNAL(changed(int,int)), this, SLOT(analogChanged(int,int)));
	}

	// Label 
	for (int i=0;i<arraysize(label);i++) {
		if (label[i].cmd == 0)
			continue;
		label[i].label = new Label(settings, &label[i]);		
		scene->addItem(label[i].label);
		label[i].label->setPos(label[i].xPos, label[i].yPos);
	}

	emit command(SRC_DISP, CMD_LAYOUT_CHANGED, layOut); 
}

int Display_touch::getVal(int cmd) {
	return cmdStore[cmd];
}

void Display_touch::setVal(int cmd, int val) {
	cmdStore[cmd]=val;
	for (int i=0;i<arraysize(label);i++) {
		if (label[i].cmd == CMD_NONE)
			break;
		if (label[i].cmd == cmd) {
			label[i].label->setVal(val);
			// only one label
			break;
		}
	}
	for (int i=0;i<arraysize(button);i++) {
		if (button[i].cmd == CMD_NONE)
			break;
		if (button[i].cmd == cmd) {
			button[i].button->setVal(val);
		}
	}
	for (int i=0;i<arraysize(analog);i++) {
		if (analog[i].cmd == CMD_NONE)
			break;
		if (analog[i].cmd == cmd) {
			analog[i].analog->setVal(val);
			break;;
		}
	}
	return;
}

void Display_touch::setFreq(int f) {
	storeBand();
	filterGraph->setFreq(f);
	displaySet(SRC_DISP, CMD_RX_FREQ, f);
	if (txrx)
		displaySet(SRC_DISP, CMD_TX_FREQ, f);
}

void Display_touch::setTXFreq(int f) {
	displaySet(SRC_DISP, CMD_TX_FREQ, f);
	if (txrx)
		displaySet(SRC_DISP, CMD_RX_FREQ, f);
}

void Display_touch::displaySet(int src, int cmd, int val) {
	int freq;
	PDEBUG(MSG1, "%s -> Display: %s, Par: %i",SrcString[src], CmdString[cmd],val);
	if (src != SRC_CTL)
		sendCmd(cmd, val);
	setVal(cmd, val);
	switch (cmd) {
		case CMD_MENU:
			if ((*curMenu)[val].cmd != CMD_NONE)
				displaySet(SRC_DISP, (*curMenu)[val].cmd, (*curMenu)[val].entry->par);
			if ((*curMenu)[val].next) {
				lastMenu = curMenu;
				curMenu = (*curMenu)[val].next;
				for (int i=0;i<MAX_MENU;i++) {
					button[menuStartIdx + i].buttonText[0] = (*curMenu)[i].entry->name;
					button[menuStartIdx + i].button->doUpdate();
				}
			}
			if (val==0) {
				inputVal = 0;
				input->setVal(inputVal);
			}
			break;
		case CMD_RX_FREQ:
			freqChanged(val);
			break;
		case CMD_TX_RX:
			txrx = val;
			if (txrx)
				setTXFreq(rxFreq);
			break;
		case CMD_TX_FREQ:
			txFreq = val;
			txfreq->setVal(txFreq);
			break;
		case CMD_MODE:
			filterGraph->setFilter(rxFreq, getVal(CMD_FILTER_RX_LO), getVal(CMD_FILTER_RX_HI), getVal(CMD_MODE));
			break;
		case CMD_FFT_SAMPLE_RATE:
			fftSampRate = val;
			filterGraph->setSampleRate(fftSampRate);
			break;
		case CMD_FFT_SIZE:
			setFFTSize(val);
			break;
		case CMD_DISP_MODE:
			fftGraph->setDisplayMode(val);
			filterGraph->setDisplayMode(val);
			break;
		case CMD_FILTER_RX_HI:
		case CMD_FILTER_RX_LO:
			filterGraph->setFilter(rxFreq, getVal(CMD_FILTER_RX_LO), getVal(CMD_FILTER_RX_HI), getVal(CMD_MODE));
			break;
		case CMD_RECALL_MEMORY:
			setMemory(val==0 ? inputVal: val);
			inputVal = 0;
			break;
		case CMD_STORE_MEMORY:
			storeMemory(val==0 ? inputVal: val);
			inputVal = 0;
			break;
		case CMD_FREQ_STEP_UP:
			freq = rxFreq + step;
			freq -= (freq % step);
			setFreq(freq);
			break;
		case CMD_FREQ_STEP_DOWN:
			freq = rxFreq;
			if ((freq % step) == 0)
				freq-= step;
			freq -= (freq % step);
			setFreq(freq);
			break;
		case CMD_INPUT_FREQ:
			setFreq(inputVal*1000-freqOffset);
			inputVal = 0;
			input->setVal(inputVal);
			break;
		case CMD_NUMBER:
			switch(val) {
				case 0 ... 9:
					inputVal = inputVal*10+val;
					if (inputVal>999999)
						inputVal=999999;
					input->setVal(inputVal);
					break;
				case 10: // DEL
					inputVal /= 10;
					input->setVal(inputVal);
					break;
				default:
					;
			}
			break;
		case CMD_BAND:
			storeBand();
			setBand(val==0 ? inputVal: val);
			inputVal = 0;
			break;
		case CMD_STEP:
			step = val;
			filterGraph->setFreqStep(step);
			break;
		case CMD_WATERFALL_MIN:
			fftGraph->setMin(val);
			break;
		case CMD_WATERFALL_MAX:
			fftGraph->setMax(val);
			break;
		case CMD_SMETER_MODE:
			smtr->setMode(val);
			break;
		case CMD_RSSI:
			smtr->setRSSIVal(val);
			break;
		case CMD_IN_LEVEL:
			smtr->setLevel(val);
			break;
		case CMD_TXPOWER_PEEK_LEVEL:
			smtr->setPowerPeekVal((double)val/0x4000); // 128^2
			break;
		case CMD_TXPOWER_AV_LEVEL:
			smtr->setPowerAvVal((double)val/0x4000);
			break;
		case CMD_SWR:
			smtr->setSWRVal((double)val/1000);
			break;
		case CMD_ADC_OV:
			smtr->setOverflow(val==1);
			break;
		case CMD_GAIN:
		case CMD_ANT:
		case CMD_PREAMP:
			break;
		case CMD_TX:
			ptt = val;
			if (val) {
				setVal(CMD_SMETER_MODE, SMETER_LEVEL);
				smtr->setMode(SMETER_LEVEL);
			} else {
				setVal(CMD_SMETER_MODE, SMETER_RX);
				smtr->setMode(SMETER_RX);
			}
			break;
		case CMD_DISP_COLOR:
			fftGraph->settingsChanged(val);
			break;
		case CMD_VOLUME:
			volume = val;
			break;
		case CMD_AGC_DEC:
		case CMD_TX_POWER:
		case CMD_NB_LEVEL:
		case CMD_NOTCH:
		case CMD_INTERNAL:
			break;
		case CMD_PRESEL:
			break;
		case CMD_FREQ_OFFSET:
			freqOffset = val;
			rxfreq->setOffset(freqOffset);
			txfreq->setOffset(freqOffset);
			filterGraph->setOffset(freqOffset);
			break;
		default:
			PDEBUG(ERR1, "Display: unhandled command: %s, par %i",CmdString[cmd],val);
			break;
	}
}


void Display_touch::storeMemory(int val) {
	if (val >= arraysize(mem))
		return;
	mem[val].name = QString("mem %1").arg(val+1);
	mem[val].mode = getVal(CMD_MODE);
	mem[val].rxFreq = rxFreq;
	mem[val].txFreq = txFreq;
	mem[val].filterHi = getVal(CMD_FILTER_RX_HI);
	mem[val].filterLo = getVal(CMD_FILTER_RX_LO);
	mem[val].preamp = getVal(CMD_PREAMP);
	mem[val].ant = getVal(CMD_ANT);
}

void Display_touch::setMemory(int val) {
	if (val >= arraysize(mem))
		return;
	displaySet(SRC_DISP, CMD_MODE, mem[val].mode);
	displaySet(SRC_DISP, CMD_RX_FREQ, mem[val].rxFreq);
	displaySet(SRC_DISP, CMD_TX_FREQ, mem[val].txFreq);
	displaySet(SRC_DISP, CMD_FILTER_RX_HI, mem[val].filterHi);
	displaySet(SRC_DISP, CMD_FILTER_RX_LO, mem[val].filterLo);
	displaySet(SRC_DISP, CMD_PREAMP, mem[val].preamp);
	displaySet(SRC_DISP, CMD_ANT, mem[val].ant);
}

void Display_touch::storeBand() {
	int curBand = getBand(rxFreq); 
	if (curBand < 0)
		return;
	memBand[curBand].name = QString("memBand %1").arg(curBand+1);
	memBand[curBand].mode = getVal(CMD_MODE);
	memBand[curBand].rxFreq = rxFreq;
	memBand[curBand].txFreq = txFreq;
	memBand[curBand].filterLo = getVal(CMD_FILTER_RX_LO);
	memBand[curBand].filterHi = getVal(CMD_FILTER_RX_HI);
	memBand[curBand].preamp = getVal(CMD_PREAMP);
	memBand[curBand].ant = getVal(CMD_ANT);
}

void Display_touch::setBand(int val) {
	if (val>=arraysize(memBand))
		return;
	displaySet(SRC_DISP, CMD_MODE, memBand[val].mode);
	displaySet(SRC_DISP, CMD_RX_FREQ, memBand[val].rxFreq);
	displaySet(SRC_DISP, CMD_TX_FREQ, memBand[val].txFreq);
	displaySet(SRC_DISP, CMD_FILTER_RX_HI, memBand[val].filterHi);
	displaySet(SRC_DISP, CMD_FILTER_RX_LO, memBand[val].filterLo);
	displaySet(SRC_DISP, CMD_PREAMP, memBand[val].preamp);
	displaySet(SRC_DISP, CMD_ANT, memBand[val].ant);
}

void Display_touch::setFFTSize(int val) {
	fftSize = val;
	window.clear();

	fftGraph->setFFTSize(fftSize);
	filterGraph->setFFTSize(fftSize);
}

void Display_touch::fftData(QByteArray fftData) {
	if (!update)
		return;
	fftGraph->fftDataReady(fftData);
}

void Display_touch::smtrPressed(int,int) {
}

void Display_touch::freqChanged(int f) {
	static double shiftRemainder=0;
	int df = rxFreq-f;
	rxFreq = f; // - f % getVal(CMD_STEP) ;
	double vs = (double)df*fftSize/fftSampRate;

	shiftRemainder += vs-floor(vs);
	if (shiftRemainder >= 1) {
		vs += floor(shiftRemainder);
		shiftRemainder -= floor(shiftRemainder);
	}
	fftGraph->scrollVert(floor(vs));
	rxfreq->setVal(rxFreq);
	filterGraph->setFreq(rxFreq);
	filterGraph->setFilter(rxFreq, getVal(CMD_FILTER_RX_LO), getVal(CMD_FILTER_RX_HI), getVal(CMD_MODE));

	update = false;
	timer->start(400);
}

void Display_touch::updateOn() {
	timer->stop();
	update = true;
}

void Display_touch::buttonPressed(int id, int val) {
	displaySet(SRC_DISP, id,val);
}

void Display_touch::analogChanged(int id, int val) {
	displaySet(SRC_DISP, id,val);
}

void Display_touch::readSettings() {
	settings->beginGroup("display_touch");

	settings->beginReadArray("Mem");
	for (int i=0;i<arraysize(mem);i++) {
		settings->setArrayIndex(i);
		mem[i].mode = settings->value("mode",mem[i].mode).toInt();
		mem[i].rxFreq = settings->value("rxFreq",mem[i].rxFreq).toInt();
		mem[i].txFreq = settings->value("txFreq",mem[i].txFreq).toInt();
		mem[i].filterLo = settings->value("filterLo",mem[i].filterLo).toInt();
		mem[i].filterHi = settings->value("filterHi",mem[i].filterHi).toInt();
		mem[i].preamp = settings->value("preamp",mem[i].preamp).toInt();
		mem[i].ant = settings->value("ant",mem[i].ant).toInt();
	}
	settings->endArray();

	settings->beginReadArray("MemBand");
	for (int i=0;i<arraysize(memBand);i++) {
		settings->setArrayIndex(i);
		memBand[i].mode = settings->value("mode",memBand[i].mode).toInt();
		memBand[i].rxFreq = settings->value("rxFreq",memBand[i].rxFreq).toInt();
		memBand[i].txFreq = settings->value("txFreq",memBand[i].txFreq).toInt();
		memBand[i].filterLo = settings->value("filerLo",memBand[i].filterLo).toInt();
		memBand[i].filterHi = settings->value("filterHi",memBand[i].filterHi).toInt();
		memBand[i].preamp = settings->value("preamp",memBand[i].preamp).toInt();
		memBand[i].ant = settings->value("ant",memBand[i].ant).toInt();
	}
	settings->endArray();
	settings->endGroup();
}

void Display_touch::writeSettings() {

	settings->beginGroup("display_touch");

	settings->beginWriteArray("Mem");
	for (int i=0;i<arraysize(mem);i++) {
		settings->setArrayIndex(i);
		settings->setValue("mode", mem[i].mode);
		settings->setValue("rxFreq", mem[i].rxFreq);
		settings->setValue("txFreq", mem[i].txFreq);
		settings->setValue("filterLo", mem[i].filterLo);
		settings->setValue("filterHi", mem[i].filterHi);
		settings->setValue("preamp", mem[i].preamp);
		settings->setValue("ant", mem[i].ant);
	}
	settings->endArray();

	settings->beginWriteArray("MemBand");
	for (int i=0;i<arraysize(memBand);i++) {
		settings->setArrayIndex(i);
		settings->setValue("mode", memBand[i].mode);
		settings->setValue("rxFreq", memBand[i].rxFreq);
		settings->setValue("txFreq", memBand[i].txFreq);
		settings->setValue("filterLo", memBand[i].filterLo);
		settings->setValue("filterHi", memBand[i].filterHi);
		settings->setValue("preamp", memBand[i].preamp);
		settings->setValue("ant", memBand[i].ant);
	}
	settings->endArray();
	settings->endGroup();
	settings->setValue("Size",size());
}

void Display_touch::sendCmd(int cmd, int val) {
	//PDEBUG(MSG1, "<-- Display: %s, Par: %i",CmdString[cmd],val);
	emit command(SRC_DISP, cmd, val); 
}

void Display_touch::keyPressEvent(QKeyEvent *event) {
	int key = event->key();
	qDebug() << key;
	switch (key) {
		case Qt::Key_F1 ... Qt::Key_F12: 
			displaySet(SRC_DISP_KEY, CMD_MENU, key-Qt::Key_F1);
			break;
		case '0' ... '9': 
			displaySet(SRC_DISP_KEY, CMD_NUMBER, key-'0');
			break;
		case Qt::Key_Delete:
			displaySet(SRC_DISP_KEY, CMD_NUMBER, 10);
			break;
		case Qt::Key_Enter:
		case Qt::Key_Return:
			displaySet(SRC_DISP_KEY, CMD_INPUT_FREQ, 0);
			break;
		case Qt::Key_Plus:
			displaySet(SRC_DISP_KEY, CMD_FREQ_STEP_UP,0);
			break;
		case Qt::Key_Minus:
			displaySet(SRC_DISP_KEY, CMD_FREQ_STEP_DOWN, 0);
			break;
		case 'F':
			if (fullScreen)
				this->setWindowState(Qt::WindowNoState);
			else
				this->setWindowState(Qt::WindowFullScreen);
			fullScreen = ~fullScreen;
			break;
		case Qt::Key_Space:
			if (ptt) 
				displaySet(SRC_DISP_KEY, CMD_TX, 0);
			else
				displaySet(SRC_DISP_KEY, CMD_TX, 1);

			break;
		case Qt::Key_VolumeUp:
			if (volume<255) {
				volume++;
				displaySet(SRC_DISP_KEY, CMD_VOLUME, volume);
			}
			break;
		case Qt::Key_VolumeDown:
			if (volume>0) {
				volume--;
				displaySet(SRC_DISP_KEY, CMD_VOLUME, volume);
			}
			break;
	}
}
