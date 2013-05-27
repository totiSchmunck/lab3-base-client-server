#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
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
#define IP_DESTINO "127.0.0.1"
#define FILE_BUFFER_SIZE 256
#define TAM_MENSAJE_LOGGER 256

///Defines de la GUI
#define GUI_ENVIAR_ARCHIVO 1
#define GUI_CERRAR_SESION 3
#define GUI_PEDIR_LISTADO 2

///Defines opciones de recepcion, las que me envia el cliente...
#define CLIENTE_ENVIANDO_ARCHIVO 1
#define CLIENTE_PIDIENDO_LISTADO_DE_CLIENTES 2

#define TAMANIO_OPCION 64

#define TAMANIO_MAXIMO_RUTA 256

#define ENVIO_SOCKET_SIZE 256

#define MSG_TAMANIO_ARCHIVO_SIZE 1024

void optionDispatcher(int opcionMenu);
void menu ();
int enviarArchivo();
int sock;

int main (int argc, char *argv[])
{
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

    menu();
    return 1;
}

void menu ()
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

        if (opcionMenu < GUI_ENVIAR_ARCHIVO || opcionMenu > GUI_CERRAR_SESION)
        {
            system("clear");
            printf("La opcion del menu es inexistente");
        }
        else
        {
            optionDispatcher(opcionMenu);
        }
        //system("clear");
    }
    while (opcionMenu != GUI_CERRAR_SESION);
}

void optionDispatcher(int opcionMenu)
{
    switch(opcionMenu)
    {
    case GUI_ENVIAR_ARCHIVO:
        enviarArchivo();
        break;
    case GUI_PEDIR_LISTADO:
        break;
    }
}

int enviarArchivo()
{
    struct stat datos;
    int fd;
    int cantLeida = 0;
    char buffer[FILE_BUFFER_SIZE];
    char bufferSocket[ENVIO_SOCKET_SIZE];
    char bufferTamanioArchivo[MSG_TAMANIO_ARCHIVO_SIZE];
    int acumuladorDeBytes = 0;
    char mensajeLogger[TAM_MENSAJE_LOGGER];
    char rutaArchivoEnviar[TAMANIO_MAXIMO_RUTA];
    char opcion[TAMANIO_OPCION];
    int socketDestino;

    printf("Ingrese una ruta de archivo a enviar: ");
    scanf("%s", rutaArchivoEnviar);
    printf("Ingrese el numero de destinatario: ");
    scanf("%d", &socketDestino);

    if(stat(rutaArchivoEnviar, &datos) == -1)
    {
        switch (errno)
        {
        case ENOENT:
            logger("No existe un componente del camino file_name o el camino es  una cadena vacía");
            break;
        case ENOTDIR:
            logger("Un componente del camino no es un directorio.");
            break;
        case EFAULT:
            logger("Direccion erronea");
            break;
        case EACCES:
            logger("Permiso denegado");
            break;
        case ENOMEM:
            logger("Fuera de memoria (es decir, memoria del núcleo).");
            break;
        case ENAMETOOLONG:
            logger("Nombre de fichero demasiado largo.");
            break;
        }
        return EXIT_FAILURE;
    }
    if (S_ISDIR(datos.st_mode))
    {
        logger("El archivo que se quiere enviar es un directorio.");
        return EXIT_FAILURE;
    }
    else
    {
        if ((fd = open(rutaArchivoEnviar, O_RDONLY)) == -1)
        {
            logger("Hubo un error al abrir el archivo");
            return EXIT_FAILURE;
        }
        else
        {
            //Envio la opcion
            memset(opcion, '\0', TAMANIO_OPCION);
            sprintf(opcion, "%d", CLIENTE_ENVIANDO_ARCHIVO);
            if(send(sock, opcion, TAMANIO_OPCION, 0) == -1)
            {
                logger("ERROR:mi_cliente.c:enviarArchivo():Error al enviar la opcion al servidor.");
                exit(EXIT_FAILURE);
            }

            //Envio el socket destinatario
            memset(bufferSocket, '\0', ENVIO_SOCKET_SIZE);
            sprintf(bufferSocket, "%d", socketDestino);
            if(send(sock, bufferSocket, ENVIO_SOCKET_SIZE, 0) == -1)
            {
                logger("ERROR:mi_cliente.c:enviarArchivo():Error al enviar el destinatario del archivo al servidor.");
                exit(EXIT_FAILURE);
            }

            memset(bufferTamanioArchivo, '\0', MSG_TAMANIO_ARCHIVO_SIZE);
            sprintf(bufferTamanioArchivo, "%d", datos.st_size);
            if(send(sock, bufferTamanioArchivo, MSG_TAMANIO_ARCHIVO_SIZE, 0) == -1)
            {
                logger("ERROR:mi_cliente.c:enviarArchivo():Error al enviar el tamanio del archivo");
                exit(EXIT_FAILURE);
            }

            memset(buffer, '\0', FILE_BUFFER_SIZE);
            cantLeida = read(fd, buffer, FILE_BUFFER_SIZE);
            if(cantLeida > 0)
                send(sock, buffer, FILE_BUFFER_SIZE, 0);
            while (cantLeida > 0)
            {
                acumuladorDeBytes += cantLeida;
                memset(buffer, '\0', FILE_BUFFER_SIZE);
                cantLeida = read (fd, buffer, FILE_BUFFER_SIZE);
                if(cantLeida > 0)
                    send(sock, buffer, FILE_BUFFER_SIZE, 0);
            }
            sprintf(mensajeLogger, "El tamaño a enviar era de %d bytes y se enviaron %d bytes", datos.st_size, acumuladorDeBytes);
            logger(mensajeLogger);
            return EXIT_SUCCESS;
        }
    }
}


