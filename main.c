#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define nRoteador 10

struct roteador{
    int id;
    int porta;
	char ip[100];
};

struct roteador * lerRoteadores(){
    char buf[100];
    char *campo;
    char delimita[] = " ";
    int a;
    struct roteador *roteadores=(struct roteador*) malloc(sizeof(struct roteador)*nRoteador);
    FILE *arquivo = fopen("roteador.config","r");
    if(!arquivo){
        printf("ERRO!!! Não foi possivel abrir o arquivo\n");
        return NULL;
    }

    while(fgets(buf, 100, arquivo) != NULL){
        roteadores[a].id = atoi(strtok(buf, delimita));

        roteadores[a].porta = atoi(strtok(NULL, delimita));

        campo = strtok(NULL, delimita);
        strcpy(roteadores[a].ip, campo);
        
        printf("id: %d\n", roteadores[a].id);
        printf("porta: %d\n", roteadores[a].porta);
        printf("ip: %s\n", roteadores[a].ip);
        a++;
    }
    return roteadores;
}

int main(){
    struct roteador *r;
    r = lerRoteadores();
    return 0;
}
