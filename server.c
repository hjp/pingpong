#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


#include "gethostbynameornumber.h"

char server_c_rcs_id[] = "$Id: server.c,v 1.6 2002-03-18 22:19:45 hjp Exp $";

char *cmnd;

static void usage(void) {
    fprintf(stderr, "Usage: %s address port\n", cmnd);
    exit(1);
}


int main(int argc, char **argv) {
    int s;
    struct sockaddr_in sa;
    struct hostent *hep;

    cmnd = argv[0];

    if (argc != 3) usage();

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
    if (bind(s, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
	    fprintf(stderr, "cannot bind socket: %s\n", strerror(errno));
	    exit(1);
    }
    for (;;) {
	struct sockaddr_in from;
	int    fromlen = sizeof(from);
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
	if (0) {
            printf("from %lx %d bytes, serial = %d\n",
                   (unsigned long)ntohl(from.sin_addr.s_addr), data_len, serial);
            fflush(stdout);
        }
	data_len = sendto(s, buffer, data_len, 0,
			    (struct sockaddr *)&from, fromlen);
   }

    return 0;
}

/*
    $Log: server.c,v $
    Revision 1.6  2002-03-18 22:19:45  hjp
    Added usage message.

    Revision 1.5  2002/03/18 22:11:07  hjp
    Added name resolution to server

 */
