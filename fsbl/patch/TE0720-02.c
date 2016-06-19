/*
 * TE0720-02.c
 *
 *  Created on: Jun 22, 2015
 *      Author: knk
 */

#include "TE0720-02.h"
#include "xil_types.h"
#include "xemacps.h"


int eth_init(int eth_id)
{
	u32 Status;
	u16 rval16;
	XEmacPs Emac;
	XEmacPs_Config *Mac_Config;

	unsigned char mac_addr[6];
	int i = 0;
	// MII Patch
	*((int *)0xE000B000) = 0x00000010;
	*((int *)0xE000B004) = 0x00100000;


	Mac_Config = XEmacPs_LookupConfig(eth_id);
	if(Mac_Config == NULL) {
		return XST_FAILURE;
	}

	Status = XEmacPs_CfgInitialize(&Emac, Mac_Config, Mac_Config->BaseAddress);
	if(Status != XST_SUCCESS){
		return XST_FAILURE;
	}

	Status = XEmacPs_PhyRead(&Emac, 0x1A,  9, &rval16);
	if(Status != XST_SUCCESS){
		return XST_FAILURE;
	}
	mac_addr[0] = (unsigned char)(rval16 >> 8);	mac_addr[1] = (unsigned char)(rval16 & 0xFF);

	Status = XEmacPs_PhyRead(&Emac, 0x1A,  10, &rval16);
	if(Status != XST_SUCCESS){
		return XST_FAILURE;
	}
	mac_addr[2] = (unsigned char)(rval16 >> 8);	mac_addr[3] = (unsigned char)(rval16 & 0xFF);

	Status = XEmacPs_PhyRead(&Emac, 0x1A,  11, &rval16);
	if(Status != XST_SUCCESS){
		return XST_FAILURE;
	}
	mac_addr[4] = (unsigned char)(rval16 >> 8);	mac_addr[5] = (unsigned char)(rval16 & 0xFF);

    xil_printf("\n\r+----------------------------------------------------------------+\n\r");
		xil_printf("|           TE0720 Unique MAC Addr: ");
		for(i = 0; i < 6; i++) {
			xil_printf("%02x ", mac_addr[i]);
		}
		xil_printf("           |\n\r");
    xil_printf("+----------------------------------------------------------------+\n\r" );

	Status = XEmacPs_SetMacAddress(&Emac, mac_addr, 1);
	if(Status != XST_SUCCESS){
		return XST_FAILURE;
	}
	XEmacPs_GetMacAddress(&Emac, mac_addr, 1);
	if(Status != XST_SUCCESS){
		return XST_FAILURE;
	}

	// Disable fiber
	Status = XEmacPs_PhyWrite(&Emac, 0x00, 0x16, 0x0012);
	Status = XEmacPs_PhyWrite(&Emac, 0x00, 0x14, 0x8210);
	Status = XEmacPs_PhyWrite(&Emac, 0x00, 0x16, 0x0000);
	// Reset USB
	Status = XEmacPs_PhyWrite(&Emac, 0x1A, 0x07, 0x0010);
	Status = XEmacPs_PhyWrite(&Emac, 0x1A, 0x07, 0x0000);

	return 0;
}

