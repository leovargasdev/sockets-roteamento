// ALGORITMO DIJKSTRA:
#include <stdio.h>
#include <stdbool.h>
#define chinelo 6 //numero de vertices
#define vInicial 2
#define vBusca 0
int visitados[chinelo];
int matrix[chinelo][chinelo] = {
                                { 0, 10, 15, 0, 0, 0},
                                { 10, 0, 0, 2, 5, 0},
                                { 15, 0, 0, 2, 0, 0},
                                { 0, 2, 2, 0, 0, 10},
                                { 0, 5, 0, 0, 0, 5},
                                { 0, 0, 0, 10, 5, 0}};
int result[chinelo][2];
// Linha 1: vertice anterior.
// Linha 2: total.
void tabelaDijs(){
    printf("Vertices            : ");
    for(int g = 0; g < chinelo; g++)
        printf("%3d  |", g+1);
    printf("\nVertices anteriores : ");
    for(int g = 0; g < chinelo; g++)
        printf("%3d  |", result[g][0]+1);
    printf("\nTotal               : ");
    for(int g = 0; g < chinelo; g++)
        printf("%3d  |", result[g][1]);
    printf("\n\n -------------------- \n\n");
    return;
}
void dijstraAdjacencia(int v) {
    int a = 0, aux, auxTotal;
    do{
        // printf("Vertice -> %d\n\n", v+1);
        aux = 1234567;
        visitados[v] = 1;
        for(a = 0; a < chinelo; a++){ // fazendo o somatorio na tabela
            auxTotal = matrix[v][a] + result[v][1];
            if(matrix[v][a] != 0 && visitados[a] != 1 && (result[a][1] == 0 || auxTotal < result[a][1])){
                result[a][0] = v;
                result[a][1] = auxTotal;
            }
        }
        // tabelaDijs();
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
    // dijstraAdjacencia(vInicial);
    // // printf("Vertice origem: %d\nVertice destino: %d\nCusto: %d\nCaminho: ", vInicial+1, vBusca+1, result[vBusca][1]);
    // int v = vBusca;
    for(int a = 0; a < chinelo; a++){
        for(int g = 0; g < chinelo; g++)
            result[g][1] = result[g][0] = visitados[g] = 0;
        dijstraAdjacencia(a);
        printf("Vertice origem: %d\n", a+1);
        tabelaDijs();
    }
    // while(true){
    //     printf("%d <-- ", v+1);
    //     v = result[v][0];
    //     if(v == 0) break;
    // }
    // printf("\n");
    return 0;
}
