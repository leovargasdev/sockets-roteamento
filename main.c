#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>

#define BUFLEN 100  //Max length of buffer
#define PACOTE_TAM 108
int quant;

struct Mensage{   //Struct da mensagem para ser enviada
    int origem;
    int destino;
    char mensagem[BUFLEN];
}; typedef struct Mensage msg;

struct Router{
    int id;
    int porta;
    char ip[100];
    int *tabela;
    struct Router *prox;
}; typedef struct Router roteador;

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
    char buf[BUFLEN], delimita[] = " ";
    int n1 = 0, n2 = 0, c = 0;
    while(fgets(buf, BUFLEN, arquivo) != NULL){
        n1 = atoi(strtok(buf, delimita)) - 1;
        n2 = atoi(strtok(NULL, delimita)) - 1;
        c = atoi(strtok(NULL, delimita));
        m[n1][n2] = c;
        m[n2][n1] = c;
    }
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

roteador *getRouter(int r){
    for(roteador *p = first->prox; p != NULL; p = p->prox)
    if(p->id == r)
    return p;
    return NULL;
}

roteador *readRouter(char t){
    t == 'o' ? printf("Origem: ") : printf("Destino: ");
    int router;
    scanf("%d", &router);
    printf("\n");
    return getRouter(router);
}

void die(char *s){
    perror(s);
    exit(1);
}

void *ouvir(void *ser){
    printf("ouvindo...");
    printf("\n");
    msg *pRecebido = (msg *) malloc(sizeof(msg));
    struct sockaddr_in si_me, si_other;
    int s, i, slen = sizeof(si_other), recv_len;
    if((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    die("socket");
    memset((msg *) &si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(myRouter->porta);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(s, (struct sockaddr*)&si_me, sizeof(si_me)) == -1)
    die("bind"); // Mensagem de endereço já utilizado
    while(1){
        fflush(stdout);
        memset(pRecebido,'\0', PACOTE_TAM);
        if ((recv_len = recvfrom(s, pRecebido, PACOTE_TAM, 0, (struct sockaddr *) &si_other, &slen)) == -1)
        die("recvfrom()");
        printf("\nMensagem recebida IP: %s, PORTA: %d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        printf("[ORIGEM]: %d \t [DESTINO]: %d\n" , pRecebido->origem, pRecebido->destino);
        printf("[MENSAGEM]: %s\n" , pRecebido->mensagem);
        if(sendto(s, pRecebido, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
        die("sendto()");
    }
    free(pRecebido);
    close(s);
}

void encaminhar(roteador *r, msg *pacote){
    struct sockaddr_in si_other;
    int s, i, slen=sizeof(si_other);
    if((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    die("socket");
    memset((msg *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(r->porta);
    if(inet_aton(r->ip , &si_other.sin_addr) == 0){
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
    if(sendto(s, pacote, PACOTE_TAM, 0, (struct sockaddr *) &si_other, slen)==-1)
    die("sendto()");
    memset(pacote,'\0', PACOTE_TAM);
    if(recvfrom(s, pacote, PACOTE_TAM, 0, (struct sockaddr *) &si_other, &slen) == -1)
    die("recvfrom()");
    close(s);
}

void *mandar(void *cli){
    roteador *otherRouter;
    msg *m = (msg *) malloc(sizeof(msg));
    m->origem = myRouter->id;
    while (1){
        int nextRoute;
        otherRouter = readRouter('d');
        //nextRoute = definirProximoRoteador(myRouter->id,otherRouter->id);
        //printf("Deve ir para o %d\n",nextRoute);
        m->destino = otherRouter->id;
        if(otherRouter){
            printf("\nMensagem:");
            //limpa buffer
            setbuf(stdin,NULL);
            fgets(m->mensagem,100,stdin);
            encaminhar(otherRouter, m);
        } else {
            printf("Digite um roteador válido\n");
        }
    }
    free(otherRouter);
}

int definirProximoRoteador(int start,int end) {
    int matrix[quant][quant], result[quant][2];
    criaGrafo(matrix);
    dijstra(start-1, matrix, result);
    tabelaDijs(result);
    int next = end-1,prox;
    printf("Custo: %d\nCaminho: ", result[next][1]);
    while(1){
        printf("%d <-- ", next+1);
        prox = next;
        next = result[next][0];
        //printf("--%d-",next);
        if(next == 0) break;
    }
    //printf("o nodos anterior e:%d\n",prox+1 );
    prox++;
    return prox;
}
int main(){
    first = (roteador *) malloc(sizeof(roteador));
    lerRoteadores();
    do{
        myRouter = readRouter('o');
        if(myRouter) break;
        else printf("roteador inválido\n");
    }while (1);
    pthread_t t1, t2;
    pthread_create(&t1, NULL, ouvir, NULL);
    sleep(2);
    pthread_create(&t2, NULL, mandar, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    free(myRouter);
    free(first);
    exit(0);
}
