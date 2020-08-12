#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <libpq-fe.h>

#include "dns_types.h"
#include "config.h"
#include "guardaIP.h"
#include "mod_inite.h"
#include "dnslib.h"


int num_forks = 0;

// Captive portal 
// Return, in string human readable format, the most resent ip registered to a postgresql database
char* getLastIPRegistered() {
  char *ret_val;
  char credentials[strlen("user= password= dbname=") 
                  + strlen(db_user) 
                  + strlen(db_password) 
                  + strlen(db_name)
                  + 1];

  // Creates a postgresql connection with credentials from config.h
  sprintf(credentials, "user=%s password=%s dbname=%s", db_user, db_password, db_name);
  PGconn *conn = PQconnectdb(credentials);

  // Checks db connection
  if (PQstatus(conn) == CONNECTION_BAD) {
      
    fprintf(stderr, "Connection to database failed: %s\n",
        PQerrorMessage(conn));
        
    PQfinish(conn);
    exit(1);
  }

  // Execute query
  PGresult *res = PQexec(conn, "select ip from portal_registre order by registrat desc limit 1");
  
  // Check content of response and stores it in return value
  if (PQresultStatus(res) == PGRES_TUPLES_OK) {
    ret_val = PQgetvalue(res, 0, 0);
  }

  // Clean and finish db connection
  PQclear(res);
  PQfinish(conn);

  return ret_val;
}

void handler(int sig) {
  if (sig == SIGCHLD){
      wait(NULL);
      --num_forks;
  } 
}

int main(int argc, char * argv[]) {

  printf("max_forks=%d\n", max_forks);
  signal(SIGCHLD, handler);

  // Captive portal setup. 
  init_inite();

  int opt = 1; // TRUE
  int master_socket;

  struct sockaddr_in address; // address del servidor

  Packet packet;
  int msgLen;

  // creem el master socket
  if ((master_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket() -> Error");
    exit(EXIT_FAILURE);

  }

  // configurem el master socket per tractar multiples peticions
  if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char * ) & opt, sizeof(opt)) < 0) {
    perror("setsockopt() -> Error");
    exit(EXIT_FAILURE);

  }

  // tipus de socket creat
  address.sin_family = AF_INET; // IPv4
  address.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
  address.sin_port = htons(53);

  // binding del socket
  if (bind(master_socket, (struct sockaddr * ) & address, sizeof(address)) < 0) {
    perror("bind() -> Error");
    exit(EXIT_FAILURE);
  }
  puts("Socket listening on 0.0.0.0:53");
	


  while (1) {

    socklen_t client_len = sizeof(packet.client_addr);
    msgLen = recvfrom(master_socket, & (packet.msg), sizeof(packet.msg), 0, &(packet.client_addr), &client_len);
    // considerem tamany minim del paacket 12 bytes


    if (msgLen >= 12) {
      if (num_forks < max_forks) {
        num_forks++;
        int p = fork();
        if (p < 0) {
          perror("fork() -> Error");
          exit(EXIT_FAILURE);
        }
        if (p == 0) {
					// #### ATENCIO PERQUE CAL CANVIAR AIXÒ ####
          //generate_success_response( Request, public_ip, comment, master_socket, client_addr, client_len);
          exit(0);
        }
      } else {

        parse_requested_domain(& (packet.msg));
        resolve_query(& (packet.msg), records, RECORDS_SIZE);

	  		exec_inite(&packet);

        if (packet.msg.answer.rr == NULL) {
          generate_failure_response(&(packet.msg), master_socket, packet.client_addr, client_len);
        }
        else {
          generate_success_response(&(packet.msg), packet.msg.answer.rr->ip, comment, master_socket, packet.client_addr, client_len);
        }
      
			}

    }

  }

}
