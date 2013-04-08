#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <time.h>

#include "sntp.h"

int
main(int argc, char **argv)
{
        char *hostname = "17.82.253.7";
        int port = 123;

        int sock;
        struct sockaddr_in server_addr;

        sock = socket(PF_INET, SOCK_DGRAM, 0);

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr(hostname);
        server_addr.sin_port = htons(port);

	time_t orgtime = time(NULL);
	if (orgtime == 0) {
		perror("time");
		return 1;
	}

	time_t ntp_time;
send:
	sntp_sendto(sock, &server_addr);
	if ((ntp_time = sntp_recvfrom(sock, &server_addr, orgtime)) != -1)
		printf("%s", ctime(&ntp_time));
	else
		goto send;
}

