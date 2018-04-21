// ALGORITMO DIJKSTRA:
#include <stdio.h>
#include <stdbool.h>
#define chinelo 7 //numero de vertices
#define vInicial 0
#define vBusca 6
int visitados[chinelo];
int matrix[chinelo][chinelo] = {
                                {0,7,0,5,0,0,0},
                                {7,0,8,9,7,0,0},
                                {0,8,0,0,5,0,0},
                                {5,9,0,0,15,6,0},
                                {0,7,5,15,0,8,7},
                                {0,0,0,6,8,0,11},
                                {0,0,0,0,7,11,0}};
char vertices[chinelo] = {'a','b','c','d','e','f','g'};
int result[chinelo][2];
// Linha 1: vertice anterior.
// Linha 2: total.
void tabelaDijs(){
    printf("Vertices            : ");
    for(int g = 0; g < chinelo; g++)
        printf("%3c  |", vertices[g]);
    printf("\nVertices anteriores : ");
    for(int g = 0; g < chinelo; g++)
        printf("%3c  |", vertices[result[g][0]]);
    printf("\nTotal               : ");
    for(int g = 0; g < chinelo; g++)
        printf("%3d  |", result[g][1]);
    printf("\n\n -------------------- \n\n");
    return;
}
void dijstraAdjacencia(int v) {
    int a = 0, aux, auxTotal;
    do{
        printf("Vertice -> %c\n\n", vertices[v]);
        aux = 1234567;
        visitados[v] = 1;
        for(a = 0; a < chinelo; a++){ // fazendo o somatorio na tabela
            auxTotal = matrix[v][a] + result[v][1];
            if(matrix[v][a] != 0 && visitados[a] != 1 && (result[a][1] == 0 || auxTotal < result[a][1])){
                result[a][0] = v;
                result[a][1] = auxTotal;
            }
        }
        tabelaDijs();
        for(a = 0; a < chinelo; a++) // buscando o vertice com o menor total e não visitado ainda
            if(visitados[a] != 1)
                if(result[a][1] < aux && result[a][1] > 0){
                    v = a;
                    aux = result[a][1];
                }
        if(visitados[v] == 1) return;
    }while(true);
}
int main(){
    int g;
    for(g = 0; g < chinelo; g++)
        result[g][1] = result[g][0] = visitados[g] = 0;
    dijstraAdjacencia(vInicial);
    printf("Vertice a ser buscado: %c\nCusto: %d\nCaminho: ", vertices[vBusca], result[vBusca][1]);
    int v = vBusca;
    while(true){
        printf("%c <-- ", vertices[v]);
        v = result[v][0];
        if(v == 0) break;
    }
    printf("%c \n", vertices[v]);
    return 0;
}
