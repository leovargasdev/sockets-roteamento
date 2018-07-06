//  UNIVERSIDADE FEDERAL DA FRONTEIRA SUL
//     --> Alunos: Leonardo Vargas e Marcelo Acordi
//     --> Disciplina: Redes
//     --> Data: maio, 07-2018
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>

#define BUFLEN 100      // Tamanho das mensagens em bytes
#define PACOTE_TAM 193  // Tamanho da struct pacote em bytes
#define NUMERO_ROTEADORES 20
#define ENVIAR_MENSAGEM 'e'
#define CONFIRMAR_MENSAGEM 'c'
#define MANDA_VETOR 'm'
#define STATUS_ENLACE 's'
#define INFINITO 1234567

int quant;              // Quantidade de roteadores
pthread_mutex_t escreverTABELA_mutex = PTHREAD_MUTEX_INITIALIZER;
/* Estrutura do pacote */
struct PacoteDados{
    int origem;
    int destino;
    char typeMsg;       // Flag que garante que o envio não entre em uma recursão
    char mensagem[BUFLEN];
    int vDistancia[NUMERO_ROTEADORES];
    int numMessageRouter;
}; typedef struct PacoteDados pacote;

/* Estrutura do roteador */
struct Router{
    int id;
    int porta;
    int numMessage;
    char ip[100];
    struct Router *prox;
    int **tabela;
}; typedef struct Router roteador;

roteador *myRouter;     // Ponteiro para o roteador selecionado pelo usuário
roteador *first;        // Ponteiro para a cabeça da lista dos roteadores

/* Esta função instancia os roteadores lidos no arquivo */
void criaNodo(roteador *f, roteador *n){
    if(f->prox == NULL) f->prox = n;
    else criaNodo(f->prox, n);
}

/* Faz a leitura do arquivo roteador.config, cria um novo nodo roteador, seta seus campos
conforme a configuração do arquivo e coloca esse novo nodo na lista de roteadores */
void lerRoteadores(){
    FILE *arquivo = fopen("./input/roteador.config","r");
    if(!arquivo){
        printf("ERRO!!! Não foi possivel abrir o arquivo\n");
        exit(1);
    }
    char buf[BUFLEN], *campo, delimita[] = " ";
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

void criaEnlaces(roteador *r){
    int numR = r->id - 1;
    for (int i = 0; i < quant; i++) {
        for (int j = 0; j < quant; j++) {
            r->tabela[i * quant + j] = INFINITO;
        }
    }
    r->tabela[numR * quant + numR] = 0;
    FILE *arquivo = fopen("./input/enlaces.config","r");
    if(!arquivo){
        printf("ERRO!!! Não foi possivel abrir o arquivo\n");
        exit(1);
    }
    char buf[BUFLEN], delimita[] = " ";
    int n1 = 0, n2 = 0;
    while(fgets(buf, BUFLEN, arquivo) != NULL){
        n1 = atoi(strtok(buf, delimita)) - 1;
        n2 = atoi(strtok(NULL, delimita)) - 1;
        if(numR == n1)
            r->tabela[n1 * quant + n2] = atoi(strtok(NULL, delimita)); // peso do enlace
        else if(numR == n2)
            r->tabela[n2 * quant + n1] = atoi(strtok(NULL, delimita)); // peso do enlace
    }
}

/* Localiza o roteador com o id correspondente a variável passada com parâmetro, caso não localizado retorna NULL */
roteador *getRouter(int r){
    roteador *w;
    for(w = first->prox; w != NULL; w = w->prox)
        if(w->id == r)  //Achou o roteador correspondente ao parâmetro
            break;
    return w;
}

/* Leitura do roteador que é especifíco da entrada do usuário */
roteador *readRouter(char t){
    t == 'o' ? printf("Origem: ") : printf("\nDestino: ");
    int router;
    scanf("%d", &router);
    return getRouter(router);
}

void die(char *s){
    perror(s);
    exit(1);
}

/* Faz-se a configuração de um novo pacote */
pacote *criaPacote(int o, int d, char m[BUFLEN], char t, int n){
    pacote *p = (pacote *) malloc(PACOTE_TAM);
    p->origem = o;
    p->destino = d;
    strcpy(p->mensagem, m);
    p->typeMsg = t;
    p->numMessageRouter = n;
    for (int i = 0; i < NUMERO_ROTEADORES; i++)
        p->vDistancia[i] = INFINITO;
    return p;
}

roteador * proximoSalto(int router) {
    int tmp,routerDestino,aux2;
    routerDestino = router - 1;
    printf("entrou calcular proximo salto\n");
    for (int i = 0; i < quant; i++) {
        tmp = 0;
        aux2 = myRouter->id-1;
        for (int j = 0; j < quant; j++) {
            /*if(i == numRouter) {
                myRouter->tabela[i* quant +j] = tmp[j];
            }*/
            if(j == aux2 && i != aux2) {
                tmp = myRouter->tabela[i* quant +j];
                tmp += myRouter->tabela[i* quant +routerDestino];
                printf("\na soma e %d\n",tmp);
                if(tmp == myRouter->tabela[aux2* quant +routerDestino] ) {
                    printf("e igual\n");
                    return getRouter(i+1);
                }
            }
        }
    }
}

/* Função responsável por enviar pacotes, 1º parametro: roteador(destino) onde será entregue o pacote, 2º parâmetro: pacote à ser entregue */
void encaminhar(roteador *r, pacote *pac){
    struct sockaddr_in si_other;
    int s, slen = sizeof(si_other);
    if((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        die("socket");
    memset((pacote *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(r->porta); // Configura a porta do roteador destino
    if(inet_aton(r->ip , &si_other.sin_addr) == 0){
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
    if(sendto(s, pac, PACOTE_TAM, 0, (struct sockaddr *) &si_other, slen)==-1)
        die("sendto()");
    memset(pac,'\0', PACOTE_TAM);
    if(pac->typeMsg == ENVIAR_MENSAGEM || pac->typeMsg == MANDA_VETOR)
        if(recvfrom(s, pac, PACOTE_TAM, 0, (struct sockaddr *) &si_other, &slen) == -1)
            die("recvfrom()");
    close(s);
}

void *atualizaTabela(void *idRoteadoVizinho){
    int rVizinho = (int*) idRoteadoVizinho;
    int aux, rMeu = (myRouter->id-1), p1, p2;
    int p3 = (rVizinho * quant) + rMeu; // Meu custo no vetor distância do vizinho
    // printf("[PERDI 1]: atualizaTabela\n");
    pthread_mutex_lock(&escreverTABELA_mutex);
    // printf("[PERDI 2]: atualizaTabela\n");
    for (int i = 0; i < quant; i++){
        p1 = (rVizinho * quant) + i; //Percorrendo a linha do vetor distância do vizinho
        p2 = (rMeu * quant) + i; //Percorrendo meus custos
        if(rMeu == i || myRouter->tabela[p1] == -1)
            continue;
        aux = ((int) myRouter->tabela[p3]) + ((int) myRouter->tabela[p1]);
        if(aux < myRouter->tabela[p2]){
            myRouter->tabela[p2] = aux;
        }
    }
    pthread_mutex_unlock(&escreverTABELA_mutex);
    // printf("[PERDI 3]: atualizaTabela\n");
    // // PRINT DA TABELA
    printf("[TABELA MODIFICADA]:\n");
    for (int i = 0; i < quant; i++) {
        for (int j = 0; j < quant; j++) {
            if(myRouter->tabela[i * quant + j] == INFINITO)
                printf("-\t");
            else
                printf("%d\t", myRouter->tabela[i * quant + j]);
        }
        printf("\n");
    }
    printf("\n -  -  -  -  -  -  -  -  -  -  -\n");
}

/* Faz a função do client, controlando o envio de pacotes do roteador selecionado pelo usuário */
void *mandar(void *a){
    roteador *otherRouter;
    int destino;
    char mensagem[BUFLEN];
    while(1){
        otherRouter = readRouter('d'); // Localiza o roteador destino do pacote
        if(otherRouter){
            destino = otherRouter->id;
            printf("Mensagem: ");
            setbuf(stdin, NULL);
            fgets(mensagem, BUFLEN, stdin); // Faz a leitura da mensagem do pacote
            myRouter->numMessage++; // Contador de nº de pacotes originados por este roteador
            if(destino != myRouter->id) // Quando o destino for ele mesmo, assim economiza a operação de achar o proximo salto
                otherRouter = proximoSalto(otherRouter->id); // Localiza o proximo salto, caso os roteadores não forem vizinhos
            encaminhar(otherRouter, criaPacote(myRouter->id, destino, mensagem, ENVIAR_MENSAGEM, myRouter->numMessage)); // Faz a configuração do pacote e envia para o destino
        } else {
            printf("Roteador válido\n");
        }
    }
    free(otherRouter);
}

/* Faz a função do server, fica ouvindo os pacotes direcionados para a porta do roteador selecionado pelo usuário */
void *ouvir(void *a){
    pthread_t abigos[quant];
    pacote *pRecebido = (pacote *) malloc(sizeof(pacote));
    struct sockaddr_in si_me, si_other;
    int s, i, slen = sizeof(si_other), recv_len, rVizinho, alterou = 0;
    if((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        die("socket");
    memset((pacote *) &si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(myRouter->porta); // Configura a porta do roteador selecionado pelo usuário
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(s, (struct sockaddr*)&si_me, sizeof(si_me)) == -1)
        die("bind"); // Mensagem de endereço já utilizado
    while(1){
        fflush(stdout);
        memset(pRecebido,'\0', PACOTE_TAM);
        if ((recv_len = recvfrom(s, pRecebido, PACOTE_TAM, 0, (struct sockaddr *) &si_other, &slen)) == -1)
            die("recvfrom()");
        if(pRecebido->typeMsg == CONFIRMAR_MENSAGEM && pRecebido->destino == myRouter->id){
            printf("\n\n *** MENSAGEM CONFIRMADA!!! ***\n"); // Mensagem da entrega confiável
        } else if(pRecebido->typeMsg == MANDA_VETOR){
            rVizinho = (pRecebido->origem - 1);
            for(int i = 0; i < quant; i++){// Atualiza o vetor distância recebido
                if(myRouter->tabela[rVizinho* quant +i] != pRecebido->vDistancia[i]){
                    myRouter->tabela[rVizinho* quant +i] = pRecebido->vDistancia[i];
                    alterou = 1;
                }
            }
            if(alterou)
                pthread_create(&abigos[rVizinho], NULL, atualizaTabela, (void *) rVizinho);
            pRecebido = criaPacote(myRouter->id, pRecebido->origem, "ENLACE ATIVO", STATUS_ENLACE, 0);
            encaminhar(otherRouter, pacoteVetor);
            alterou = 0;
        }else if(pRecebido->typeMsg == STATUS_ENLACE){
            printf("enlace ativo\n");
        }else{
            if(pRecebido->destino == myRouter->id){  // Caso ele for o roteador destino
                int d = pRecebido->origem, nMsg = pRecebido->numMessageRouter;
                printf("\n\n *** DESTINO FINAL!!! ***\n\n");
                printf("[ORIGEM]: %d \t [DESTINO]: %d\n" , pRecebido->origem, pRecebido->destino);
                printf("[SEQUENCIA]: %d \t [MENSAGEM]: %s\n" ,pRecebido->numMessageRouter, pRecebido->mensagem);

                memset(pRecebido,'\0', PACOTE_TAM);
                pRecebido = criaPacote(myRouter->id, d, "Mensagem Recebida", CONFIRMAR_MENSAGEM, nMsg); // Reconfigura o pacote para mandar a confimação do pacote ao roteador origem
            } else {
                printf("\nRoteador %d encaminhando mensagem com # sequência %d para o destino %d enviada por %d\n", myRouter->id, pRecebido->numMessageRouter, pRecebido->destino, pRecebido->origem);
            }
            // Se entrou no if vai mandar um mensagem de confimação ao roteador origem, se não ele vai encaminhar o pacote para o destino
            encaminhar(proximoSalto(pRecebido->destino), pRecebido);
        }

        if(sendto(s, pRecebido, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
            die("sendto()");
    }
    free(pRecebido);
    close(s);
}

void *mandaVetorDist(void *a){
    roteador * otherRouter;
    pacote *pacoteVetor = (pacote *) malloc(sizeof(pacote));
    int meuRot = myRouter->id - 1, vizinhos[quant], destino;
    for (int j = 0; j < quant; j++) {
        if(myRouter->tabela[meuRot * quant + j] != INFINITO && myRouter->tabela[meuRot * quant + j]){
            vizinhos[j] = 1; // Eh vizinho
        }else{
            vizinhos[j] = 0;
        }
    }
    while(1){
        for(int i= 0; i < quant; i++){
            if(vizinhos[i]){
                otherRouter = getRouter(i+1);
                destino = otherRouter->id;
                pacoteVetor = criaPacote(myRouter->id, destino, "Meu vetor distancia", MANDA_VETOR, myRouter->numMessage);
                for (int j = 0; j < quant; j++) // Atualiza o vetor à ser enviado
                    pacoteVetor->vDistancia[j] = myRouter->tabela[meuRot * quant + j];
                encaminhar(otherRouter, pacoteVetor);
            }
        }
        sleep(5);
    }
    free(otherRouter);
}

int main(){
    first = (roteador *) malloc(sizeof(roteador));
    lerRoteadores();
    do{
        myRouter = readRouter('o');
        if(myRouter) break;
        else printf("roteador inválido\n");
    }while(1);

    myRouter->tabela = (int **) malloc(sizeof (int *) * (quant*quant));

    criaEnlaces(myRouter);
    pthread_t t1, t2, t3;

    pthread_create(&t1, NULL, ouvir, NULL);
    sleep(0.5);
    pthread_create(&t2, NULL, mandar, NULL);
    pthread_create(&t3, NULL, mandaVetorDist, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    free(myRouter);
    free(first);
    exit(0);
}
