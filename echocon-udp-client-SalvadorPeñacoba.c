#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<errno.h>
#include<netinet/ip.h>
#include<unistd.h>

//Macro assert para comprobar + mensaje de error.
#define ASSERT(_bool, ...) if (!(_bool)) { fprintf(stderr, __VA_ARGS__); exit(EXIT_FAILURE); }

#define MAX_LENGHT 80

int main(int argc, char** argv){

    uint16_t puerto = htons(5);
    struct in_addr addr;

    char* msg_in2;
    char* msg_out2;

    int aux;
    int sockfd;
    struct sockaddr_in server_addr;
    struct sockaddr_in my_addr = {.sin_family= AF_INET, .sin_port = 0, .sin_addr.s_addr = INADDR_ANY};

    memset(&addr,0,sizeof(addr));
    memset(&server_addr,0,sizeof(server_addr));
    memset(&my_addr,0,sizeof(my_addr));


    ASSERT((argc== 3 || argc== 5),"Uso: %s ip_servidor [-p puerto_servidor] cadena\n", argv[0]);

    ASSERT((inet_aton(argv[1],&addr) == 1),"Uso: %s ip_servidor [-p puerto_servidor] cadena\n", argv[0]);
    
    aux = (argc == 3) ? 2 : 4;
    if(strlen(argv[aux]) > MAX_LENGHT)
        printf("La cadena será truncada a %i caracteres.\n",MAX_LENGHT);
    msg_out2 = strndup(argv[aux],MAX_LENGHT);
    msg_in2= calloc(MAX_LENGHT+1,sizeof(char));

    if(argc == 5){
        ASSERT((strcmp(argv[2],"-p")== 0), "Uso: %s ip_servidor [-p puerto_servidor] cadena\n", argv[0]);
        sscanf(argv[3],"%i",&aux);
        ASSERT((aux > 0 && aux <= 65535),"Error: puerto_servidor no es un puerto válido\n");
        puerto = htons(aux);
    }
    printf("Puerto: %i, Cadena: %s\n",ntohs(puerto),msg_out2);

    //Poblamos el struct de la direccion del socket servidor.
    server_addr.sin_port = puerto;
    server_addr.sin_addr = addr;
    server_addr.sin_family = AF_INET;

    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    ASSERT(sockfd != -1, "Error creando socket: %s\n", strerror(errno));

    aux = bind(sockfd, (struct sockaddr*) &my_addr, sizeof(my_addr));
    ASSERT(aux == 0,"Error vinculando socket: %s\n", strerror(errno));

    aux = sendto(sockfd, msg_out2, MAX_LENGHT,0, (struct sockaddr*)&server_addr,sizeof(server_addr));
    ASSERT(aux != -1,"Error enviando datagrama: %s\n",strerror(errno));
    printf("Cadena enviada, esperando respuesta...\n");

    aux = recvfrom(sockfd,msg_in2,MAX_LENGHT,0,NULL,NULL);
    ASSERT(aux != -1,"Error recibiendo datagrama: %s\n",strerror(errno));

    printf("Cadena recibida: %s\n",msg_in2);

    close(sockfd);

    return 0;
}