#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
/// sockets =>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/un.h>
/// select =>
#include <sys/time.h>
#include <sys/select.h>
/// threads
#include <pthread.h>
/// log
#include "loggerCliente.h"

#define PUERTO_DESTINO 3456
#define IP_DESTINO "192.168.1.101"

void menu (int socketDescriptor);

int main (int argc, char *argv[])
{
    int sock;
    struct sockaddr_in server;

    inicializarLogger();

    ///@brief Creo el socket para el cliente
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Error al crear socket cliente");
        logger("Error al crear el socket cliente.");
        exit(EXIT_FAILURE);
    }

    server.sin_addr.s_addr = inet_addr(IP_DESTINO);
    server.sin_family = AF_INET;
    server.sin_port = htons(PUERTO_DESTINO);

    ///@brief Enlazo con el servidor
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror("Error en la conexion");
        logger("Error en la conexion al servidor.");
        exit(EXIT_FAILURE);
    }

    menu(sock);
    return 1;
}

void menu (int socketDescriptor)
{
    int opcionMenu;

    do
    {
        printf("Seleccione una opcion que desea realizar\n");
        printf("\n");
        printf("\t1 - Enviar un archivo\n");
        printf("\t2 - Ver listado de usuarios conectados\n");
        printf("\t3 - Salir\n");

        scanf("%d", &opcionMenu);

        if (opcionMenu < 1 || opcionMenu > 3)
        {
            system("clear");
            printf("La opcion del menu es inexistente");
        }
        else
        {

        }
    }
    while (opcionMenu != 3);
}
