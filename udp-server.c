#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/ip.h>
#include <unistd.h>

// Macro assert para comprobar + mensaje de error.
#define ASSERT(_bool, ...)            \
    if (!(_bool))                     \
    {                                 \
        fprintf(stderr, __VA_ARGS__); \
        exit(EXIT_FAILURE);           \
    }

#define MAX_LENGHT 80

void process_string();


int main(int argc, char **argv)
{

    char* msg_in2;
    char* msg_out2;

    uint16_t puerto = htons(2000);

    int aux;
    int sockfd;
    unsigned int client_legth = 0;
    struct sockaddr_in client_addr;
    struct sockaddr_in my_addr;

    memset(&client_addr,0,sizeof(client_addr));
    memset(&my_addr,0,sizeof(my_addr));

    msg_in2 = (char*)calloc(MAX_LENGHT+1,sizeof(char));
    msg_out2 = (char*)calloc(MAX_LENGHT+1,sizeof(char));

    ASSERT((argc == 1 || argc == 3), "Uso: %s [-p puerto_servidor]\n", argv[0]);

    if (argc == 3)
    {
        ASSERT((strcmp(argv[1], "-p") == 0), "Uso: %s ip_servidor [-p puerto_servidor] cadena\n", argv[0]);
        sscanf(argv[1], "%i", &aux);
        ASSERT((aux > 0 && aux <= 65535), "Error: puerto_servidor no es un puerto válido\n");
        puerto = htons(aux);
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = puerto;
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    ASSERT(sockfd != -1, "Error creando socket: %s\n", strerror(errno));

    aux = bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr));
    ASSERT(aux == 0, "Error vinculando socket: %s\n", strerror(errno));

    printf("Servidor Echo UDP en  %s escuchando puerto %i\n", inet_ntoa(my_addr.sin_addr), ntohs(puerto));
    fflush(stdout);

    while (1)
    {
        client_legth = sizeof(client_addr);

        aux = recvfrom(sockfd, msg_in2, MAX_LENGHT+1, 0, (struct sockaddr *)&client_addr, &client_legth);
        ASSERT(aux != -1, "Error recibiendo datagrama: %s\n", strerror(errno));

        printf("Recibida string: %s desde %s\n", msg_in2, inet_ntoa(client_addr.sin_addr));
        
        process_string(msg_in2, msg_out2);

        aux = sendto(sockfd, msg_out2, strlen(msg_out2)+1, 0, (struct sockaddr *)&client_addr, client_legth);
        ASSERT(aux != -1, "Error enviando datagrama: %s\n", strerror(errno));
    }

    close(sockfd);
}

void process_string(char *msg_in, char *msg_out)
{
    strncpy(msg_out,msg_in,MAX_LENGHT);
    
    for(int i = 0; i < strlen(msg_in); i++){
        if(msg_in[i]>64 && msg_in[i]<91) msg_out[i]+=32;
        else if(msg_in[i]>96 && msg_in[i]<123) msg_out[i]-=32;
    }
    
}
