#include <stdio.h>
#include <netinet/in.h>

#include "gethostbynameornumber.h"
struct hostent *gethostbynameornumber(const char *name) {
    unsigned int q[4];
    char dummy;
    if (sscanf(name, "%u.%u.%u.%u%c", q+0, q+1, q+2, q+3, &dummy) == 4
        && q[0] < 256 && q[1] < 256 && q[2] < 256 && q[3] < 256
    ){
	static struct hostent he;
	static char ipv4_addr[4];
	static char *ipv4_addr_list[2] = {ipv4_addr, NULL};

	he.h_name = name;
	he.h_aliases = NULL;
	he.h_addrtype = AF_INET;
	he.h_length = 4;
	he.h_addr_list = ipv4_addr_list;
	ipv4_addr[0] = q[0];
	ipv4_addr[1] = q[1];
	ipv4_addr[2] = q[2];
	ipv4_addr[3] = q[3];
	return &he;
    } else {
	return gethostbyname(name);
    }
}
