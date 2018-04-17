#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int quant;

struct Router{
    int id;
    int porta;
	char ip[100];
	struct Router *prox;
};

typedef struct Router roteador;

void criaNodo(roteador *f, roteador *n){
    if(f->prox == NULL) f->prox = n;
    else criaNodo(f->prox, n);
}

void lerRoteadores(roteador *first){
    FILE *arquivo = fopen("roteador.config","r");
    if(!arquivo){
        printf("ERRO!!! Não foi possivel abrir o arquivo\n");
        exit(1);
    }
    char buf[100], *campo, delimita[] = " ";
    int quant = 0;
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

int main(){
    roteador *first = (roteador *) malloc(sizeof(roteador));
    lerRoteadores(first);
	for(roteador *p = first->prox; p != NULL; p = p->prox)
		printf("%5d", p->id);
    printf("\n");
    return 0;
}
