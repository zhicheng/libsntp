#include <arpa/inet.h>
#include <strings.h>
#include <time.h>
#include <sys/select.h>

#include "sntp.h"

void
sntp_sendto(int sock, struct sockaddr *addr, socklen_t addrlen)
{
	struct sntp_packet pkt;
	bzero(&pkt, sizeof(pkt));

	pkt.status = 0;
	pkt.status |= VN_V1;
	pkt.status |= MODE_CLIENT;
	
	/* don't set orgtime, for securet reason? */
        sendto(sock, &pkt, sizeof(pkt), 0, addr, addrlen);
}

time_t
sntp_recvfrom(int sock, struct sockaddr *addr, socklen_t *addrlen, time_t orgtime)
{
	int recvlen;
	char recvbuf[48];
	struct sntp_packet *ppkt;

	struct timeval timeout;
	fd_set rdfds;
 
	FD_ZERO(&rdfds);
	FD_SET(sock, &rdfds);

	timeout.tv_sec = 2;
	timeout.tv_usec = 0;
 
	while (1) {
		int nfds = select(sock + 1, &rdfds, NULL, NULL, &timeout);
		if (nfds == -1) {
			perror("select");
			return -1;
		}
		if (nfds > 0) {
			if (FD_ISSET(sock, &rdfds)) {
				recvlen = recvfrom(sock, recvbuf, 48, 0, addr, addrlen);
				break;
			}
		} else {
			return -1;
		}
	}

	ppkt = (struct sntp_packet *)recvbuf;
	ppkt->orgtime.seconds = ntohl(ppkt->orgtime.seconds);
	ppkt->rectime.seconds = ntohl(ppkt->rectime.seconds);
	ppkt->xmttime.seconds = ntohl(ppkt->xmttime.seconds);

	time_t rectime = ppkt->rectime.seconds - JAN_1970;
	time_t xmttime = ppkt->xmttime.seconds - JAN_1970;
	time_t dsttime = time(NULL);
	if (dsttime == -1) {
		perror("time");
		return -1;
	}

	return dsttime + ((rectime - orgtime) + (xmttime - dsttime))/2;
}

