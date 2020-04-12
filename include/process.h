#ifndef __PROCESS_H
#define __PROCESS_H
#include "dns_RR_t.h"
#include "checks.h"


int process (char* client_ip, char* req_domain, DNS_RR *Request, const char *ip, const char *comment, int master_socket, const struct sockaddr client_addr, int client_len);

#endif
