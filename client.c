#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define WAIT_FOR_REPLY 1

int main(int argc, char **argv) {
    int s;
    char *cmnd;
    struct sockaddr_in sa;
    int    data_len;
    int i;
    int on = 1;
    int size;
    int count;
    unsigned char buffer[65537];
    struct timeval tv0, tv1;
    double t;

    cmnd = argv[0];


    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s == -1) {
	fprintf(stderr, "%s: cannot create socket: %s\n",
		cmnd, strerror(errno));
	exit(1);
    }
    if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)) == -1) {
	fprintf(stderr, "%s: cannot enable broadcasts: %s\n",
		cmnd, strerror(errno));
	exit(1);
    }

    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(strtoul(argv[1], NULL, 0));
    sa.sin_port = htons(strtoul(argv[2], NULL, 0));

    size = strtoul(argv[3], 0, 0);
    count = strtoul(argv[4], 0, 0);


    gettimeofday(&tv0, NULL);

    for (i = 0; i < count; ) {
	struct sockaddr_in from;
	int     fromlen = sizeof(from);
	fd_set fds;
	struct timeval to;

	buffer[0] = (i >> 24) & 0xFF;
	buffer[1] = (i >> 16) & 0xFF;
	buffer[2] = (i >> 8) & 0xFF;
	buffer[3] = (i >> 0) & 0xFF;
	data_len = sendto(s, buffer, size, 0,
			    (struct sockaddr *)&sa, sizeof(sa));
	if (0) printf("message to %lx\n", ntohl(sa.sin_addr.s_addr));
	if (data_len == -1) {
	    printf("%s\n", strerror(errno));
	} else {
	    if (0) printf("%d bytes\n", data_len);
	}

#if WAIT_FOR_REPLY
	FD_ZERO(&fds);
	FD_SET(s, &fds);
	to.tv_sec = 1;
	to.tv_usec = 0;
	if (select(s+1, &fds, NULL, NULL, &to) > 0) {
	    data_len = recvfrom(s, buffer, sizeof(buffer), 0,
				(struct sockaddr *)&from, &fromlen);
	    i++;
	} else {
	    printf("timeout at serial %d: retrying\n", i);
	}
		
#endif
    }

    gettimeofday(&tv1, NULL);

    t = (tv1.tv_sec + tv1.tv_usec * 1E-6) -
        (tv0.tv_sec + tv0.tv_usec * 1E-6) ;
    printf ("%d %g %g\n", size, t, ((double)size * count) / t);
    return 0;
}
