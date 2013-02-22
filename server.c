// gcc server.c -Wall

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

int tcp_socket_server;
int unix_socket_server;


int start_tcp_server(){

}

int start_udp_server(){

}

int start_unix_socket_server(){

}

int stop_tcp_server(){

}

int stop_udp_server(){

}

int stop_unix_socket_server(){

}

void listen_and_accept_new_clients(){

}

void logger(char *text) {
  printf("%s", text);
}


// main function
int main(int argc, char *argv[]) {

  if (start_tcp_server() && start_udp_server() && start_unix_socket_server()) {
    logger("Server started successfully");
  } else {
    stop_tcp_server();
    stop_udp_server();
    stop_unix_socket_server();
    return EXIT_FAILURE;
  }

  listen_and_accept_new_clients();

  return EXIT_SUCCESS;
}
