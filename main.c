#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int quant;
struct roteador{
    int id;
    int porta;
	char ip[100];
};

struct roteador * lerRoteadores(){
    char buf[100];
    char *campo;
    char delimita[] = " ";
    int a, quant = 0;
    FILE *arquivo = fopen("roteador.config","r");
    if(!arquivo){
        printf("ERRO!!! Não foi possivel abrir o arquivo\n");
        return NULL;
    }
    while(fgets(buf, 100, arquivo) != NULL){
        strtok(buf, "\n");
        quant++;
    }

    arquivo = fopen("roteador.config","r");
    struct roteador *roteadores=(struct roteador*) malloc(sizeof(struct roteador)*quant);

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

void criaMatrix(int m[quant][quant]){
    m[0][0] = 124;
}

int main(){
    struct roteador *r;
    r = lerRoteadores();
    int matrix[quant][quant];
    for(int a = 0; a < quant; a++)
        for(int k = 0; k < quant; k++)
            matrix[a][k] = 0;
    criaMatrix(matrix);
    printf("%d\n", matrix[0][0]);
    free(r);
    return 0;
}
