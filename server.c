#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


int main(int argc, char **argv) {
    int s;
    char *cmnd;
    struct sockaddr_in sa;

    cmnd = argv[0];

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s == -1) {
	fprintf(stderr, "%s: cannot create socket: %s\n",
		cmnd, strerror(errno));
	exit(1);
    }

    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(strtoul(argv[1], NULL, 0));
    sa.sin_port = htons(strtoul(argv[2], NULL, 0));
    if (bind(s, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
	    fprintf(stderr, "cannot bind socket: %s\n", strerror(errno));
	    exit(1);
    }
    for (;;) {
	struct sockaddr_in from;
	int     fromlen = sizeof(from);
	int    data_len;
	unsigned char    buffer[65537];
	int serial;

	data_len = recvfrom(s, buffer, sizeof(buffer), 0,
			    (struct sockaddr *)&from, &fromlen);
	if (data_len >= 4) {
	    serial = (buffer[0] << 24) +
	             (buffer[1] << 16) +
	             (buffer[2] << 8) +
	             (buffer[3] << 0);
	} else {
	    serial = 0;
	}
	printf("from %lx %d bytes, serial = %d\n",
		ntohl(from.sin_addr.s_addr), data_len, serial);
	data_len = sendto(s, buffer, data_len, 0,
			    (struct sockaddr *)&from, fromlen);
	fflush(stdout);
   }

    return 0;
}
