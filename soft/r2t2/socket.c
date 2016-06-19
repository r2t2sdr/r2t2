#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <unistd.h>
#include "math.h"

#define MY_DEST_MAC0	0x00
#define MY_DEST_MAC1	0x00
#define MY_DEST_MAC2	0x00
#define MY_DEST_MAC3	0x00
#define MY_DEST_MAC4	0x00
#define MY_DEST_MAC5	0x00

#define ETH_P_R2T2      	0x7232
#define R2T2_HEADER_SIZE	16 

#define BUF_SIZ			(1024*10)	
#define PADDING_SIZE 	2

int totalSamples = 0;

int readSocket(const char *ifname)
{
	int sockfd, ret, i;
	ssize_t numbytes;
	uint8_t buf[BUF_SIZ];
	static int n=0;

	/* Open PF_PACKET socket, listening for EtherType ETHER_TYPE */
	if ((sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_R2T2))) == -1) {
		perror("listener: socket");	
		return -1;
	}


	/* Bind to device */
	if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, ifname, IFNAMSIZ-1) == -1)	{
		perror("SO_BINDTODEVICE");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	while (1) {
		numbytes = recvfrom(sockfd, buf, BUF_SIZ, 0, NULL, NULL);
		if (numbytes<=R2T2_HEADER_SIZE)
			continue;
		numbytes -= R2T2_HEADER_SIZE;
		//printf("listener: got packet %i bytes\n", (int)numbytes);

		totalSamples += numbytes/8;
		//if (n++ > 100) {
		//printf("samples %i\n",totalSamples);
		n=0;
		/* Print packet */
		printf("%i %08x\n", numbytes, *(uint32_t*)(buf+R2T2_HEADER_SIZE));
		//if (((*(uint32_t*)(buf+R2T2_HEADER_SIZE)) & 0x0f000000) == 0x08000000 )
		//	for (i=0; i<numbytes/4; i++) printf("%i ", ((*(uint32_t*)(buf+R2T2_HEADER_SIZE+i*4))&0xffffff) << 8);
		//printf("\n");
	}

	close(sockfd);
	return ret;
}

int writeSocket(const char *ifname) 
{
	int i,sockfd;
	struct ifreq if_idx;
	struct ifreq if_mac;
	int tx_len = 0;
	char sendbuf[BUF_SIZ];
	struct ether_header *eh = (struct ether_header *) sendbuf;
	//struct iphdr *iph = (struct iphdr *) (sendbuf + sizeof(struct ether_header));
	struct sockaddr_ll socket_address;
	static double p1=0,p2=0;

	/* Open RAW socket to send on */
	if ((sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) == -1) {
		perror("socket");
	}

	/* Get the index of the interface to send on */
	memset(&if_idx, 0, sizeof(struct ifreq));
	strncpy(if_idx.ifr_name, ifname, IFNAMSIZ-1);
	if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0)
		perror("SIOCGIFINDEX");
	/* Get the MAC address of the interface to send on */
	memset(&if_mac, 0, sizeof(struct ifreq));
	strncpy(if_mac.ifr_name, ifname, IFNAMSIZ-1);
	if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0)
		perror("SIOCGIFHWADDR");

	/* Construct the Ethernet header */
	memset(sendbuf, 0, BUF_SIZ);
	/* Ethernet header */
	eh->ether_shost[0] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[0];
	eh->ether_shost[1] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[1];
	eh->ether_shost[2] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[2];
	eh->ether_shost[3] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[3];
	eh->ether_shost[4] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[4];
	eh->ether_shost[5] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[5];
	eh->ether_dhost[0] = MY_DEST_MAC0;
	eh->ether_dhost[1] = MY_DEST_MAC1;
	eh->ether_dhost[2] = MY_DEST_MAC2;
	eh->ether_dhost[3] = MY_DEST_MAC3;
	eh->ether_dhost[4] = MY_DEST_MAC4;
	eh->ether_dhost[5] = MY_DEST_MAC5;
	/* Ethertype field */
	eh->ether_type = htons(ETH_P_R2T2);
	tx_len += sizeof(struct ether_header);

	tx_len += PADDING_SIZE;

	/* Packet data */
	for (i=0;i<64;i++) {
		*(int16_t*)(sendbuf + tx_len    ) = 0x2222;
		*(int16_t*)(sendbuf + tx_len + 2) = 0xbbbb;
		tx_len += 4;
	}

	/* Index of the network device */
	socket_address.sll_ifindex = if_idx.ifr_ifindex;
	/* Address length*/
	socket_address.sll_halen = ETH_ALEN;
	/* Destination MAC */
	socket_address.sll_addr[0] = MY_DEST_MAC0;
	socket_address.sll_addr[1] = MY_DEST_MAC1;
	socket_address.sll_addr[2] = MY_DEST_MAC2;
	socket_address.sll_addr[3] = MY_DEST_MAC3;
	socket_address.sll_addr[4] = MY_DEST_MAC4;
	socket_address.sll_addr[5] = MY_DEST_MAC5;

	i=0;
	while (1) {
#if 1 
		for (int n=0;n<64;n++) {
			*(int16_t*)(sendbuf + 16 + n*4) = sin(p1)*0x3ff0+sin(p2)*0x3f00;
			*(int16_t*)(sendbuf + 18 + n*4) = cos(p1)*0x3ff0+cos(p2)*0x3f00;
			p1 += 1.0/20*2*M_PI;
			p2 += -1.0/20*2*M_PI;
			if (p1>2*M_PI) p1-= 2*M_PI;
			if (p2<-2*M_PI) p2+= 2*M_PI;
		}
#endif

		/* Send packet */
		int ret = sendto(sockfd, sendbuf, tx_len, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll));
		if (i++ % 1000 == 0) {
			printf (".");
			fflush(stdout);
		}
		if (ret < 0) {
			printf("Send failed\n");
			usleep(5*1000);
		}
	}

	close(sockfd);
	return 0;

}

