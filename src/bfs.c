#include <stdio.h>
#include <stdlib.h>
#include "bfs.h"

// BFS busca un camino dirigido desde el origen hasta destino

/* Entrada:
    - grafo: matriz de adyacencia. grafo[i][j] == 1 significa que existe arco i -> j.
    - nVertices: cantidad total de vértices.
    - origen: índice del vértice inicial.
    - destino: índice del vértice final.
    - padre: arreglo donde se guarda el vértice anterior en el camino.

    Retorno:
    - 1 si existe camino.
    - 0 si no existe camino.
*/

int BFS(int **grafo, int nVertices, int origen, int destino, int padre[]) {
    int *visitado = malloc(nVertices * sizeof(int));
    int *cola = malloc(nVertices * sizeof(int));

    if (visitado == NULL || cola == NULL) {
        printf("Error de memoria en BFS.\n");

        free(visitado);
        free(cola);

        return 0;
    }

    for (int i = 0; i < nVertices; i++) {
        visitado[i] = 0;
        padre[i] = -1;
    }

    int inicio = 0;
    int fin = 0;

    visitado[origen] = 1;
    cola[fin] = origen;
    fin++;

    while (inicio < fin) {
        int actual = cola[inicio];
        inicio++;

        if (actual == destino) {
            free(visitado);
            free(cola);
            return 1;
        }

        for (int vecino = 0; vecino < nVertices; vecino++) {
            if (grafo[actual][vecino] == 1 && visitado[vecino] == 0) {
                visitado[vecino] = 1;
                padre[vecino] = actual;

                cola[fin] = vecino;
                fin++;
            }
        }
    }

    free(visitado);
    free(cola);

    return 0;
}


// Imprime el camino encontrado por BFS usando el arreglo padre.
void ImprimirCamino(int padre[], int origen, int destino) {
    int camino[1000];
    int largo = 0;
    int actual = destino;

    while (actual != -1) {
        camino[largo] = actual;
        largo++;

        if (actual == origen) {
            break;
        }

        actual = padre[actual];
    }

    if (camino[largo - 1] != origen) {
        printf("No existe camino.\n");
        return;
    }

    printf("Camino encontrado: ");

    for (int i = largo - 1; i >= 0; i--) {
        printf("%d", camino[i]);

        if (i > 0) {
            printf(" -> ");
        }
    }

    printf("\n");
}