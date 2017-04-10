#define R2T2_HEADER_SIZE	16 

int connectR2T2(const char *ifname);
int disconnectR2T2(int sockfd);
int readSocket(int sockfd, uint8_t *buf, size_t bufSize);
int writeSocket(int sockfd, uint8_t *buf, size_t len);
