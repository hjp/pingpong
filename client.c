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
    int    data_len;
    int i;
    int on = 1;
    int size;
    int count;
    char buffer[65537];

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
    for (i = 0; i < count; i++) {

	buffer[0] = (i >> 24) & 0xFF;
	buffer[1] = (i >> 16) & 0xFF;
	buffer[2] = (i >> 8) & 0xFF;
	buffer[3] = (i >> 0) & 0xFF;
	data_len = sendto(s, buffer, size, 0,
			    (struct sockaddr *)&sa, sizeof(sa));
	printf("message to %lx\n", ntohl(sa.sin_addr.s_addr));
	if (data_len == -1) {
	    printf("%s\n", strerror(errno));
	} else {
	    printf("%d bytes\n", data_len);
	}
    }

    return 0;
}
