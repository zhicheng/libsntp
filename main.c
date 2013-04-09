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
        char *hostname = "time.asia.apple.com";
        int sock;

	struct addrinfo *ai;
	int error;

	if ((error = getaddrinfo(hostname, "ntp", NULL, &ai)) < 0) {
		fprintf(stderr, "%s\n", gai_strerror(error));
		return 1;
	}

        sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);

	time_t orgtime = time(NULL);
	if (orgtime == 0) {
		perror("time");
		return 1;
	}

	time_t ntp_time;
send:
	sntp_sendto(sock, ai->ai_addr, ai->ai_addrlen);
	if ((ntp_time = sntp_recvfrom(sock, ai->ai_addr, &ai->ai_addrlen, orgtime)) != -1)
		printf("%s", ctime(&ntp_time));
	else
		goto send;
}

