#include "utils.h"
#include "remoteproc.h"
#include "xil_printf.h"

/**
 *  Forged arp response, sender must be 192.168.1.1 with MAC
 *  00:57:44:4C:44:52 and ip destination must be 192.168.1.2
 *
 *  00 57 44 4C 44 52         MAC dest
 *  00 57 44 4C 44 53         MAC SRC
 *  08 06                     type arp
 *  00 01                     HW type ethernet
 *  08 00                     Protocol IP
 *  06                        HW size
 *  04                        Protocol size
 *  00 02                     opcode
 *  00 57 44 4C 44 53         Sender mac
 *  C0 A8 01 02               Sender IP
 *  00 57 44 4C 44 52         Target MAC
 *  C0 A8 01 01               Target IP
 *  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  padding
 */
#define ARP_RESPONSE_SIZE 60
unsigned char arp_response[ARP_RESPONSE_SIZE] = {
		0x00, 0x57, 0x44, 0x4C, 0x44, 0x52, 0x00, 0x57, 0x44, 0x4C, 0x44, 0x53,
		0x08, 0x06, 0x00, 0x01, 0x08, 0x00, 0x06, 0x04, 0x00, 0x02, 0x00, 0x57,
		0x44, 0x4C, 0x44, 0x53, 0xC0, 0xA8, 0x01, 0x02, 0x00, 0x57, 0x44, 0x4C,
		0x44, 0x52, 0xC0, 0xA8, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* Ethernet layer definition */
#define ETH_MTU 1500

#define ETH_DST_MAC_OFFSET			0x0
#define ETH_SRC_MAC_OFFSET			0x6

#define ETH_ADD_LENGTH				0x06

/* IP layer definitions */
#define IP_OFFSET					0xe
#define IP_NEXT_PROTOCOL_OFFSET 	0x17
#define IP_SRC_IP_OFFSET 			0x1A
#define IP_DST_IP_OFFSET 			0x1E
#define IP_FLAGS_OFFSET				0x14
#define IP_CHECKSUM_OFFSET			0x18

#define IP_HDR_LENGTH_MASK			0x0F
#define TCP_PROTOCOL_ICMP 			0x01

#define IP_ADD_LENGTH 				0x4
#define IP_CHECKSUM_LENGTH			0x2

/* ICMP layer definitions */
#define ICMP_TYPE_OFFSET			0x22

#define ICMP_TYPE_ECHO_REPLY 		0x0
#define ICMP_TYPE_ECHO_REQUEST 		0x8

/* Response skb */
static char skb_response[ETH_MTU];

/* -------------------------------------------------------------------------- */
int is_arp_request(void* data, unsigned int len)
{
	char* cdata = (char*)data;
	return	(len > 6) &&
			(cdata[0] == 0xFF && cdata[1] == 0xFF && cdata[2] == 0xFF &&
					cdata[3] == 0xFF && cdata[4] == 0xFF && cdata[5] == 0xFF);
}

/* -------------------------------------------------------------------------- */
int is_ping_request(void* data, unsigned int len)
{
	char* cdata = (char*)data;
	return (len > IP_NEXT_PROTOCOL_OFFSET) &&
			(cdata[IP_NEXT_PROTOCOL_OFFSET] == TCP_PROTOCOL_ICMP) &&
			(cdata[ICMP_TYPE_OFFSET] == ICMP_TYPE_ECHO_REQUEST);
}

/* -------------------------------------------------------------------------- */
void answer_to_arp()
{
	/* No need to forge the answer, it is statically done ... */
	remoteproc_send_data(arp_response, ARP_RESPONSE_SIZE);
}

/* -------------------------------------------------------------------------- */
uint16_t compute_ip_checksum(char* from, unsigned int size)
{
	uint32_t sum = 0;
	uint16_t val;

	/* Compute the sum of ip header */
	while(size)
	{
		val =  ((*((uint8_t*)from)) << 8) | *((uint8_t*)from + 1);
		from += 2;
		sum += val;
		/* Check for leftover bit */
		if(sum & 0x80000000)
			sum = (sum & 0xFFFF) + (sum >> 16);
		size -= 2;
	}

	/* Add leftover byte */
	if(size)
		sum += (uint32_t)*(uint16_t*)from;

	/* Carry */
	while(sum >> 16)
		sum = (sum & 0xFFFF) + (sum >> 16);
	sum = ((sum & 0xFF00) >> 8) | ((sum & 0xFF) << 8);

	return ~sum;
}

/* -------------------------------------------------------------------------- */
unsigned int get_ip_hdr_size(void* data, unsigned int len)
{
	if(len < ICMP_TYPE_OFFSET) return 0;
	return ((((char*)data)[IP_OFFSET]) & IP_HDR_LENGTH_MASK) * 4;
}

/* -------------------------------------------------------------------------- */
void answer_to_ping(void* data, unsigned int len)
{
	uint32_t checksum;

	/* Copy current skb for forging */
	memcpy(skb_response, data, len);

	/* Invert src and dst MAC */
	memcpy(skb_response + ETH_SRC_MAC_OFFSET,
			data + ETH_DST_MAC_OFFSET, ETH_ADD_LENGTH);
	memcpy(skb_response + ETH_DST_MAC_OFFSET,
			data + ETH_SRC_MAC_OFFSET, ETH_ADD_LENGTH);

	/* Invert src and dst IP */
	memcpy(skb_response + IP_SRC_IP_OFFSET,
			data + IP_DST_IP_OFFSET, IP_ADD_LENGTH);
	memcpy(skb_response + IP_DST_IP_OFFSET,
			data + IP_SRC_IP_OFFSET, IP_ADD_LENGTH);

	/* Remove TCP flags */
	skb_response[IP_FLAGS_OFFSET] = 0;

	/* Set ICMP to echo reply */
	skb_response[ICMP_TYPE_OFFSET] = ICMP_TYPE_ECHO_REPLY;

	/* Compute IP header checksum */
	memset(skb_response + IP_CHECKSUM_OFFSET, 0, IP_CHECKSUM_LENGTH);
	checksum = compute_ip_checksum(
			skb_response + IP_OFFSET, get_ip_hdr_size(skb_response, len));
	memcpy(skb_response + IP_CHECKSUM_OFFSET, &checksum, IP_CHECKSUM_LENGTH);

	/* Send the IP answer */
	remoteproc_send_data((void*)skb_response, len);
}

/* -------------------------------------------------------------------------- */
void r2t2_test_handle_skb(void* data, unsigned int len)
{
	static unsigned long skb_cnt = 0;

	/* DEBUG
	xil_printf("New socket data of size 0x%x!\r\n", len);
	dump_data(data, len);
	*/

	if(is_arp_request(data, len))
	{
		xil_printf("****** 0x%02x : Got ARP request ******\r\n", ++skb_cnt);
		answer_to_arp();
	}
	else if(is_ping_request(data, len))
	{
		xil_printf("****** 0x%02x : Got a ping ******\r\n", ++skb_cnt);
		answer_to_ping(data, len);
	}

	// TODO: Check for ping ...
}
