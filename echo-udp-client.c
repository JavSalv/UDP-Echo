// Practica Tema 5: Salvador Peñacoba, Javier

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/ip.h>
#include <unistd.h>

//Macro assert para comprobar + mensaje de error.
#define ASSERT(_bool, ...) if (!(_bool)) { fprintf(stderr, __VA_ARGS__); exit(EXIT_FAILURE); }

//Tamaño máximo de string
#define MAX_LENGHT 80

int main(int argc, char **argv)
{

    int aux;
    int sockfd;
    struct in_addr addr;
    uint16_t puerto = htons(5);

    struct sockaddr_in server_addr;
    struct sockaddr_in my_addr = {.sin_family = AF_INET, .sin_port = 0, .sin_addr.s_addr = INADDR_ANY};

    char *msg_in2;
    char *msg_out2;

    memset(&addr, 0, sizeof(addr));
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&my_addr, 0, sizeof(my_addr));

    ASSERT((argc == 3 || argc == 5), "Uso: %s ip_servidor [-p puerto_servidor] cadena\n", argv[0]);

    //Extraemos dirección IP del primer argumento
    ASSERT((inet_aton(argv[1], &addr) == 1), "Uso: %s ip_servidor [-p puerto_servidor] cadena\n", argv[0]);

    aux = (argc == 3) ? 2 : 4;
    if (strlen(argv[aux]) > MAX_LENGHT)
        printf("La cadena será truncada a %i caracteres.\n", MAX_LENGHT);

    msg_out2 = strndup(argv[aux], MAX_LENGHT);
    msg_in2 = calloc(strlen(msg_out2) + 1, sizeof(char));

    if (argc == 5)
    {
        ASSERT((strcmp(argv[2], "-p") == 0), "Uso: %s ip_servidor [-p puerto_servidor] cadena\n", argv[0]);
        sscanf(argv[3], "%i", &aux);
        ASSERT((aux > 0 && aux <= 65535), "Error: puerto_servidor no es un puerto válido\n");
        puerto = htons(aux);
    }
    printf("Enviando %s a %s:%hu\n", msg_out2, argv[1], ntohs(puerto));

    // Poblamos el struct de la direccion del socket servidor con el puerto y la dirección que nos dan.
    server_addr.sin_port = puerto;
    server_addr.sin_addr = addr;
    server_addr.sin_family = AF_INET;

    // Creamos socket UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    ASSERT(sockfd != -1, "Error creando socket: %s\n", strerror(errno));

    // Bindeamos socket con IP y Puerto locales
    aux = bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr));
    ASSERT(aux == 0, "Error vinculando socket: %s\n", strerror(errno));

    // Enviamos mensaje al servidor
    aux = sendto(sockfd, msg_out2, strlen(msg_out2) + 1, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    ASSERT(aux != -1, "Error enviando datagrama: %s\n", strerror(errno));
    printf("Cadena enviada, esperando respuesta...\n");

    // Recibimos mensaje del servidor
    aux = recvfrom(sockfd, msg_in2, strlen(msg_out2) + 1, 0, NULL, NULL);
    ASSERT(aux != -1, "Error recibiendo datagrama: %s\n", strerror(errno));

    printf("Cadena recibida: %s\n", msg_in2);

    // Liberamos memoria y cerramos el socket
    free(msg_in2);
    free(msg_out2);
    close(sockfd);

    return 0;
}