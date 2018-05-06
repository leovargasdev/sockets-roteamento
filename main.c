/*
--------------UNIVERSIDADE FEDERAL DA FRONTEIRA SUL
--Alunos: Leonardo Vargas e Marcelo Acordi
--Disciplina: Redes
--2018/1
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>

#define BUFLEN 100  //Max length of buffer
#define PACOTE_TAM 113 //Tamanho do pacote enviado pelos roteadores

int quant; //Número de roteadores

/* Struct da mensagem para ser enviada*/
struct PacoteDados{
    int origem;
    int destino;
    char typeMsg;
    char mensagem[BUFLEN];
    int numMessageRouter;
}; typedef struct PacoteDados pacote;
/*----------------------------------*/

/* Struct que representa o roteador */
struct Router{
    int id;
    int porta;
    int numMessage;
    char ip[100];
    struct Router *prox;
    int tabela[][2];
}; typedef struct Router roteador;
/*----------------------------------*/

/*Definindo roteadores globais */
roteador *myRouter, *first;

/*Esta função instancia os roteadores lidos no arquivo */
void criaNodo(roteador *f, roteador *n){
    if(f->prox == NULL) f->prox = n;
    else criaNodo(f->prox, n);
}
/*----------------------------------*/

/*Lendo roteadores e suas configurações como porta e IP */
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
        nodo->numMessage = 0;
        nodo->prox = NULL;
        criaNodo(first, nodo);
        quant++;
    }
}
/*----------------------------------*/

/* Criando a tabela de adjascências das ligações dos roteadores */
void criaEnlaces(int m[quant][quant]){
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
/*----------------------------------*/

/* Imprimindo a tabela que mostra as menores distâncias entre os roteadores  e o roteador  do processo*/
void tabelaDijs(int result[quant][2]){
    printf("[TABELA]:\n\tVertices            : ");
    for(int g = 0; g < quant; g++)
    printf("%3d  |", g+1);
    printf("\n\tVertices anteriores : ");
    for(int g = 0; g < quant; g++)
    printf("%3d  |", result[g][0]+1);
    printf("\n\tTotal               : ");
    for(int g = 0; g < quant; g++)
    printf("%3d  |", result[g][1]);
    printf("\n");
}
/*----------------------------------*/

/* Calcula e armazena em uma tabela os vértices(roteadores) anteriores e o custo de ir para outros roteadores*/
void dijstra(int tabela[quant][2]){
    int matrix[quant][quant], visitados[quant];
    int v = (myRouter->id-1);
    criaEnlaces(matrix);
    for(int g = 0; g < quant; g++)
    tabela[g][1] = tabela[g][0] = visitados[g] = 0;
    int a = 0, aux, auxTotal;
    do{
        aux = 1234567;
        visitados[v] = 1;
        for(a = 0; a < quant; a++){ // fazendo o somatorio na tabela
            auxTotal = matrix[v][a] + tabela[v][1];
            if(matrix[v][a] != 0 && visitados[a] != 1 && (tabela[a][1] == 0 || auxTotal < tabela[a][1])){
                tabela[a][0] = v;
                tabela[a][1] = auxTotal;
            }
        }
        for(a = 0; a < quant; a++) // buscando o vertice com o menor total e não visitado ainda
        if(visitados[a] != 1)
        if(tabela[a][1] < aux && tabela[a][1] > 0){
            v = a;
            aux = tabela[a][1];
        }
        if(visitados[v] == 1) return;
    }while(1);
}
/*----------------------------------*/

/* Entramos com um inteiro que
representa qual roteador queremos e ela retorna um ponteiro
para a struct do roteador especifíco
*/
roteador *getRouter(int r){
    for(roteador *w = first->prox; w != NULL; w = w->prox){
        if(w->id == r){
            printf("[ID]: %d  [PORTA]: %d  [IP]: %s", w->id, w->porta, w->ip);
            // printf("[TABELA]:\n");
            // dijstra(w);
            return w;
        }
    }
    return NULL;
}
/*----------------------------------*/

/* Leitura do roteador que é especifíco da entrada do usuário */
roteador *readRouter(char t){
    t == 'o' ? printf("Origem: ") : printf("\nDestino: ");
    int router;
    scanf("%d", &router);
    return getRouter(router);
}
/*----------------------------------*/

void die(char *s){
    perror(s);
    exit(1);
}

/* Define qual é o pŕoximo roteador para encaminhar a msg baseado no resultado do djkstra */
roteador *proximoSalto(int dest){
    int tabela[quant][2], proxId = -1, count = dest - 1;
    dijstra(tabela);
    // tabelaDijs(tabela);
    while(1) {
        count = tabela[count][0];
        if(tabela[count][0] == 0 && (count == (myRouter->id-1)) ) break;
        proxId = count;
    }
    //Estabelendo próximo baseado se precisa de um salto ou mais.
    if(proxId == -1) {
        proxId = dest;
    }else{
        proxId += 1;
    }
    return getRouter(proxId);
}
/*----------------------------------*/

/* Enviar mensagem para o roteador de entrada,
sendo enviada pelo roteador atual ou
tendo sido recebida de outro roteador e encaminhada.
*/
void encaminhar(roteador *r, pacote *pac){
    struct sockaddr_in si_other;
    int s, i, slen=sizeof(si_other);
    if((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    die("socket");
    memset((pacote *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(r->porta);
    if(inet_aton(r->ip , &si_other.sin_addr) == 0){
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
    if(sendto(s, pac, PACOTE_TAM, 0, (struct sockaddr *) &si_other, slen)==-1)
    die("sendto()");
    memset(pac,'\0', PACOTE_TAM);
    if(pac->typeMsg == 'N') {
        if(recvfrom(s, pac, PACOTE_TAM, 0, (struct sockaddr *) &si_other, &slen) == -1){
            die("recvfrom()");
        }
    }
    close(s);
}
/*----------------------------------*/

/* Controla o envio do roteador instanciado no processo pelo usuário,envia mensagem*/
void *mandar(void *cli){
    roteador *otherRouter;
    pacote *p = (pacote *) malloc(sizeof(pacote));
    p->origem = myRouter->id;
    while (1){
        otherRouter = readRouter('d');
        p->destino = otherRouter->id;
        p->numMessageRouter = 0;
        if(otherRouter){
            printf("Mensagem: ");
            setbuf(stdin, NULL); //limpa buffer
            fgets(p->mensagem, BUFLEN, stdin);
            myRouter->numMessage++;
            p->numMessageRouter = myRouter->numMessage;
            p->typeMsg = 'N';
            otherRouter = proximoSalto(otherRouter->id);
            encaminhar(otherRouter, p);
        } else {
            printf("Digite um roteador válido\n");
        }
    }
    free(otherRouter);
}
/*----------------------------------*/

/* Checar se a mensagens recebidas que foram enviadas por outros
roteadores, se é o destino final enviar mensagem de resposta
informando que a mensagem for recebida. Caso não, encaminha a mensagem.
*/
void *ouvir(void *ser){
    pacote *pRecebido = (pacote *) malloc(sizeof(pacote));
    struct sockaddr_in si_me, si_other;
    int s, i, slen = sizeof(si_other), recv_len;
    if((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    die("socket");
    memset((pacote *) &si_me, 0, sizeof(si_me));
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
        if(pRecebido->destino == myRouter->id){
            printf("\n\n *** DESTINO FINAL!!! ***\n");
            printf("\nMensagem recebida IP: %s, PORTA: %d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
            printf("[ORIGEM]: %d \t [DESTINO]: %d\n" , pRecebido->origem, pRecebido->destino);
            printf("[SEQUENCIA]: %d \t [MENSAGEM]: %s\n" ,pRecebido->numMessageRouter, pRecebido->mensagem);
            if(pRecebido->typeMsg == 'N'){
                int destino = pRecebido->origem, origem = myRouter->id, nrmMessage = pRecebido->numMessageRouter;
                memset(pRecebido,'\0', PACOTE_TAM);
                pRecebido->origem = origem;
                pRecebido->destino = destino;
                pRecebido->numMessageRouter = nrmMessage;
                pRecebido->typeMsg = 'C';
                strcpy(pRecebido->mensagem,"Mensagem Recebida");
                encaminhar(proximoSalto(pRecebido->destino), pRecebido);
            }else if(pRecebido->typeMsg == 'C') {
                printf("\n\n *** Mensagem Confirmação!!! ***\n");
            }

        } else {
            printf("Roteador %d encaminhando mensagem com # sequência %d para o destino %d enviada por %d\n",myRouter->id,pRecebido->numMessageRouter,pRecebido->destino,pRecebido->origem);
            encaminhar(proximoSalto(pRecebido->destino), pRecebido);
        }
        if(sendto(s, pRecebido, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
        die("sendto()");
    }
    free(pRecebido);
    close(s);
}
/*----------------------------------*/

int main(){
    first = (roteador *) malloc(sizeof(roteador));
    //Lendo roteadores de entrada
    lerRoteadores();
    //informando roteador atual do processo
    do{
        myRouter = readRouter('o');
        if(myRouter) break;
        else printf("roteador inválido\n");
    }while(1);
    //definindo contador do número de mensagens do roteador atual
    myRouter->numMessage = 0;
    //Criando Threads
    pthread_t t1, t2;
    pthread_create(&t1, NULL, ouvir, NULL);
    sleep(0.5);
    pthread_create(&t2, NULL, mandar, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    free(myRouter);
    free(first);
    exit(0);
}
