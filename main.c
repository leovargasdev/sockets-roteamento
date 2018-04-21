#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int quant;

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
	struct Router *prox;
};

typedef struct Router roteador;

void criaNodo(roteador *f, roteador *n){
    if(f->prox == NULL) f->prox = n;
    else criaNodo(f->prox, n);
}

void lerRoteadores(roteador *first){
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

void criaGrafo(int m[quant][quant]){
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
    // for (int i = 0; i < quant; i++){
    //     for (int h = 0; h < quant; h++)
    //         printf("%4d |", m[i][h]);
    //     printf("\n");
    // }
}

void dijstra(int v, int matrix[quant][quant], int visitados[quant], int result[quant][2]){
    int a = 0, aux, auxTotal;
    do{
        printf("Vertice -> %d\n\n", v+1);
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
        printf("%3d  |", g);
    printf("\nVertices anteriores : ");
    for(int g = 0; g < quant; g++)
        printf("%3d  |", result[g][0]);
    printf("\nTotal               : ");
    for(int g = 0; g < quant; g++)
        printf("%3d  |", result[g][1]);
    printf("\n\n -------------------- \n\n");
    return;
}

int main(){
    roteador *first = (roteador *) malloc(sizeof(roteador));
    lerRoteadores(first);
    int matrix[quant][quant], result[quant][2];
    int visitados[quant];
    int origem, destino;
    criaGrafo(matrix);
    printf("Selecionar roteador: ");
    scanf("%d", &origem);
    dijstra(origem, matrix, visitados, result);
    do{
        printf("Selecionar roteador destino[9 = sair]:");
        scanf("%d", &destino);
        printf("Custo: %d\nCaminho: ", result[destino][1]);
        int v = destino;
        // while(1){
        //     printf("%d <-- ", v+1);
        //     v = result[v][0];
        //     if(v == 0) break;
        // }
    }while(destino != 9);
    return 0;
}
