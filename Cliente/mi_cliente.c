#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include "loggerCliente.h"
#define PUERTO_DESTINO 3456
#define IP_DESTINO "192.168.1.103"

int main (int argc, char *argv[])
{
    int sock;
    struct sockaddr_in server;

    ///@brief Creo el socket para el cliente
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Error al crear socket cliente");
        return EXIT_FAILURE;
    }

    server.sin_addr.s_addr = inet_addr(IP_DESTINO);
    server.sin_family = AF_INET;
    server.sin_port = htons(PUERTO_DESTINO);

    ///@brief Enlazo con el servidor
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror("Error en la conexion");
        return EXIT_FAILURE;
    }

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

