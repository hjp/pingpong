#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "gethostbynameornumber.h"

#define WAIT_FOR_REPLY 1

int main(int argc, char **argv) {
    int s;
    char *cmnd;
    struct sockaddr_in sa;
    int    data_len;
    int i;
    int size;
    int count;
    unsigned char buffer[65537];
    struct timeval tv0, tv1;
    double t;
    struct hostent *hep;
    int err = 0;
    int max_err = 0;

    cmnd = argv[0];


    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s == -1) {
	fprintf(stderr, "%s: cannot create socket: %s\n",
		cmnd, strerror(errno));
	exit(1);
    }

    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    if (!(hep = gethostbynameornumber(argv[1]))) {
	fprintf(stderr, "%s: cannot resolve hostname %s: %s\n",
		cmnd, argv[1], strerror(errno));
	herror(NULL);
	exit(1);
    }
    if (hep->h_addr_list[1]) {
	int i;

	fprintf(stderr, "%s: host %s is multihomed (",
		cmnd, argv[1]);
	for (i = 0; hep->h_addr_list[i+1]; i++) {
	    fprintf(stderr, "%d.%d.%d.%d, ",
	    	   (unsigned char) hep->h_addr_list[i][0],
	    	   (unsigned char) hep->h_addr_list[i][1],
	    	   (unsigned char) hep->h_addr_list[i][2],
	    	   (unsigned char) hep->h_addr_list[i][3]);
	}
	fprintf(stderr, "%d.%d.%d.%d) - using first address\n",
	       (unsigned char) hep->h_addr_list[i][0],
	       (unsigned char) hep->h_addr_list[i][1],
	       (unsigned char) hep->h_addr_list[i][2],
	       (unsigned char) hep->h_addr_list[i][3]);
    }
    assert(hep->h_length <= sizeof(sa.sin_addr));
    memcpy(&sa.sin_addr, hep->h_addr_list[0], hep->h_length);

    sa.sin_port = htons(strtoul(argv[2], NULL, 0));

    size = strtoul(argv[3], 0, 0);
    count = strtoul(argv[4], 0, 0);

    for (i = count; i>>=1; max_err++);

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
	if (data_len == -1) {
	    fprintf(stderr, "%s: sendto failed: %s\n", cmnd, strerror(errno));
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
	    fprintf(stderr, "%s: timeout at serial %d: retrying\n", cmnd, i);
	    if (++err >= max_err) {
		fprintf(stderr, "%s: bailing out\n", cmnd);
		break;
	   } 
	}
#endif
    }

    gettimeofday(&tv1, NULL);

    t = (tv1.tv_sec + tv1.tv_usec * 1E-6) -
        (tv0.tv_sec + tv0.tv_usec * 1E-6) ;
    printf ("%d %d %d %g %g\n", size, i, err, t, ((double)size * i) / t);
    return 0;
}
