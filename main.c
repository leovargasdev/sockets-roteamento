#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include <pthread.h>
int quant;

#define BUFLEN 100  //Max length of buffer


typedef struct mensage{   //Struct da mensagem para ser enviada
    int origem;
    int destino;
    // struct roteador* info;
    char message[100];
} msg;

struct Router{
    int id;
    int porta;
    char ip[100];
    int *tabela;
    struct Router *prox;
};

typedef struct Router roteador;

roteador *myRouter, *first;

void criaNodo(roteador *f, roteador *n){
    if(f->prox == NULL) f->prox = n;
    else criaNodo(f->prox, n);
}

void lerRoteadores(){
    FILE *arquivo = fopen("./input/roteador.config","r");
    if(!arquivo){
        printf("ERRO!!! Não foi possivel abrir o arquivo\n");
        exit(1);
    }
    char buf[100], *campo, delimita[] = " ";
    quant = 0;
    while(fgets(buf, 100, arquivo) != NULL){
        roteador *nodo = (roteador *) malloc(sizeof(roteador));
        nodo->id = atoi(strtok(buf, delimita));
        nodo->porta = atoi(strtok(NULL, delimita));
        campo = strtok(NULL, delimita);
        strcpy(nodo->ip, campo);
        nodo->prox = NULL;
        criaNodo(first, nodo);
        quant++;
    }
}

void criaGrafo(int  m[quant][quant]){
    for (int i = 0; i < quant; i++)
        for (int h = 0; h < quant; h++)
            m[i][h] = 0;
    FILE *arquivo = fopen("./input/enlaces.config","r");
    if(!arquivo){
        printf("ERRO!!! Não foi possivel abrir o arquivo\n");
        exit(1);
    }
    char buf[100], delimita[] = " ";
    int n1 = 0, n2 = 0, c = 0;
    while(fgets(buf, 100, arquivo) != NULL){
        n1 = atoi(strtok(buf, delimita)) - 1;
        n2 = atoi(strtok(NULL, delimita)) - 1;
        c = atoi(strtok(NULL, delimita));
        m[n1][n2] = c;
        m[n2][n1] = c;
    }
    // PRINT DA MATRIX DE ADJACENCIA MONTADA:
    // for(int i = 0; i < quant; i++){
    //     for(int h = 0; h < quant; h++)
    //         printf("%4d |", m[i][h]);
    //         printf("\n");
    // }
}

void dijstra(int v, int matrix[][quant], int result[][2]){
    int visitados[quant];
    for(int g = 0; g < quant; g++)
        result[g][1] = result[g][0] = visitados[g] = 0;
    int a = 0, aux, auxTotal;
    do{
        aux = 1234567;
        visitados[v] = 1;
        for(a = 0; a < quant; a++){ // fazendo o somatorio na tabela
            auxTotal = matrix[v][a] + result[v][1];
            if(matrix[v][a] != 0 && visitados[a] != 1 && (result[a][1] == 0 || auxTotal < result[a][1])){
                result[a][0] = v;
                result[a][1] = auxTotal;
            }
        }
        // tabelaDijs();
        for(a = 0; a < quant; a++) // buscando o vertice com o menor total e não visitado ainda
        if(visitados[a] != 1)
        if(result[a][1] < aux && result[a][1] > 0){
            v = a;
            aux = result[a][1];
        }
        if(visitados[v] == 1) return;
    }while(1);
}

void tabelaDijs(int result[quant][2]){
    printf("Vertices            : ");
    for(int g = 0; g < quant; g++)
    printf("%3d  |", g+1);
    printf("\nVertices anteriores : ");
    for(int g = 0; g < quant; g++)
    printf("%3d  |", result[g][0]+1);
    printf("\nTotal               : ");
    for(int g = 0; g < quant; g++)
    printf("%3d  |", result[g][1]);
    printf("\n\n -------------------- \n\n");
}

roteador * getRouter(int r) {
    for(roteador *p = first->prox; p != NULL; p = p->prox){
        if(p->id == r){
            return p;
        }
    }
    return NULL;
}

roteador * readRouter(char t) {
    t == 'o' ? printf("Origem:") : printf("Destino:");
    int router;
    scanf("%d", &router);
    return getRouter(router);
}

void die(char *s){
    perror(s);
    exit(1);
}

void sendMessageRouter(roteador *prox) {

    struct sockaddr_in si_other;
    int s, i, slen=sizeof(si_other);
    char buf[BUFLEN];
    char message[BUFLEN];

    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    die("socket");
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;  //for IPV4 internet protocol
    si_other.sin_port = htons(prox->porta); //assign port number on which server listening
    if (inet_aton(prox->ip , &si_other.sin_addr) == 0){ // assign server machine address
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
    while(1){
        printf("Enter message : ");
        //gets(message);
        scanf("%s",message);
        //send the message
        if (sendto(s, message, strlen(message) , 0 , (struct sockaddr *) &si_other, slen)==-1)
        die("sendto()");
        //receive a reply and print it
        //clear the buffer by filling null, it might have previously received data
        memset(buf,'\0', BUFLEN);
        //try to receive some data, this is a blocking call
        if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
        die("recvfrom()");
        puts(buf);
    }
    close(s);
}

void * executaClient(void * arg){
    printf("Entrou client\n");
    //result[][0] = vertice ant, result[][1] = custo
    roteador * otherRouter = (roteador *) malloc(sizeof(roteador));
    int matrix[quant][quant], result[quant][2];
    int prox;
    criaGrafo(matrix);
    dijstra((myRouter->id - 1), matrix, result);
    do{
        otherRouter = readRouter('d');
        if(!otherRouter) break;
        int destino = otherRouter->id-1;
        printf("Custo: %d\nCaminho: ", result[destino][1]);
        while(1){
            printf("%d <-- ", destino+1);
            destino = result[destino][0];
            if(result[destino][0] == 0)
                break;
            prox = destino;
        }
        prox++;
        otherRouter = getRouter(prox);
        printf("\nroteador: %d porta: %d ip: %s", otherRouter->id, otherRouter->porta, otherRouter->ip);
    }while(1);
    free(otherRouter);
}

void * listenMessageRouter(void * arg){
    printf("porta: %d\n", myRouter->porta);
    struct sockaddr_in si_me, si_other;
    int s, i, slen = sizeof(si_other) , recv_len;
    char buf[BUFLEN];

    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        die("socket");
    memset((char *) &si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET; // for IPV4 internet protocol
    si_me.sin_port = htons(myRouter->porta); // assign port on which server will listen
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
        die("bind");
    while(1){
        // printf("Waiting for data...");
        fflush(stdout);
        //receive a reply and print it
        //clear the buffer by filling null, it might have previously received data
        memset(buf,'\0', BUFLEN);
        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
        die("recvfrom()");
        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        printf("Data: %s\n" , buf);
        //now reply the client with the same data
        if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
            die("sendto()");
    }
    close(s);
}

int main(){
    pthread_t threadServer, threadClient;
    first = (roteador *) malloc(sizeof(roteador));
    lerRoteadores();
    myRouter = readRouter('o');

    pthread_create(&threadServer, NULL, listenMessageRouter, NULL);
    pthread_join(threadServer,NULL);

    pthread_create(&threadClient, NULL, executaClient, NULL);
    pthread_join(threadClient,NULL);

    //
    // dijstra((myRouter->id - 1), matrix, result);
    // do{
    //     otherRouter = readRouter('d');
    //     if(!otherRouter) break;
    //     int destino = otherRouter->id-1;
    //     printf("Custo: %d\nCaminho: ", result[destino][1]);
    //     while(1){
    //         printf("%d <-- ", destino+1);
    //         destino = result[destino][0];
    //         if(result[destino][0] == 0)
    //             break;
    //         prox = destino;
    //     }
    //     prox++;
    //     otherRouter = getRouter(prox);
    //     //printf("\nroteador: %d porta: %d ip: %s", otherRouter->id, otherRouter->porta, otherRouter->ip);
    // }while(1);
    free(first);
    free(myRouter);
    return 0;
}
