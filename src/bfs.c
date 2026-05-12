#include <stdio.h>
#include <stdlib.h>
#include "bfs.h"
#include "archivos.h"

/*
 * BFS busca un camino desde origen hasta destino dentro del grafo.
 *
 * Entrada:
 *  - grafo: matriz de adyacencia. grafo[i][j] == 1 significa que i y j son adyacentes.
 *  - nVertices: cantidad total de vértices.
 *  - origen: índice del vértice inicial.
 *  - destino: índice del vértice final.
 *  - padre: arreglo donde se guarda el vértice anterior en el camino.
 *
 * Retorno:
 *  - 1 si existe camino.
 *  - 0 si no existe camino.
 */
int BFS(int **grafo, int nVertices, int origen, int destino, int padre[]) {
    if (grafo == NULL || padre == NULL || nVertices <= 0 ||
        origen < 0 || origen >= nVertices || destino < 0 || destino >= nVertices) {
        return 0;
    }

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

/*
 * Reconstruye el camino encontrado por BFS usando el arreglo padre.
 * El camino queda guardado en orden correcto: origen ... destino.
 */
int ConstruirCaminoBFS(int padre[], int origen, int destino, int camino[], int *largo) {
    int actual = destino;
    int temp[1000];
    int n = 0;

    if (padre == NULL || camino == NULL || largo == NULL) {
        return 0;
    }

    while (actual != -1 && n < 1000) {
        temp[n] = actual;
        n++;

        if (actual == origen)
            break;

        actual = padre[actual];
    }

    if (n == 0 || temp[n - 1] != origen) {
        *largo = 0;
        return 0;
    }

    *largo = n;

    for (int i = 0; i < n; i++) {
        camino[i] = temp[n - 1 - i];
    }

    return 1;
}

/* Imprime el camino encontrado por BFS usando solo los índices de vértices. */
void ImprimirCamino(int padre[], int origen, int destino) {
    int camino[1000];
    int largo = 0;

    if (!ConstruirCaminoBFS(padre, origen, destino, camino, &largo)) {
        printf("No existe camino.\n");
        return;
    }

    printf("Camino encontrado: ");

    for (int i = 0; i < largo; i++) {
        printf("%d", camino[i]);

        if (i < largo - 1) {
            printf(" -> ");
        }
    }

    printf("\n");
}

/*
 * Imprime un camino usando la información de los puntos turísticos.
 * Los vértices que no son turísticos se muestran como vértices intermedios.
 */
void ImprimirCaminoTuristico(int camino[], int largo) {
    printf("Camino valido: ");

    for (int i = 0; i < largo; i++) {
        int v = camino[i];

        if (EsVerticeTuristico(v)) {
            printf("%s", ObtenerNombreVertice(v));
        } else {
            printf("Vertice %d", v);
        }

        if (i < largo - 1) {
            printf(" -> ");
        }
    }

    printf("\n");
}

/*
 * Calcula la ruta turística pedida en el enunciado.
 *
 * Estrategia:
 *  1. Parte en el primer punto turístico del archivo.
 *  2. Busca con BFS un camino hacia el siguiente punto turístico no visitado.
 *  3. Si en ese camino aparecen otros puntos turísticos, también los marca como visitados.
 *  4. Repite hasta visitar todos o hasta encontrar un tramo imposible.
 *
 * No busca una ruta óptima. Solo busca caminos válidos entre puntos turísticos.
 */
int CalcularRutaTuristica(int **grafo, int nVertices) {
    int nPuntos = ObtenerCantidadPuntosTuristicos();

    if (nPuntos <= 0) {
        printf("No hay puntos turisticos para visitar.\n");
        return 0;
    }

    int *visitado = calloc(nPuntos, sizeof(int));
    int *padre = malloc(nVertices * sizeof(int));
    int *camino = malloc(nVertices * sizeof(int));

    if (visitado == NULL || padre == NULL || camino == NULL) {
        printf("Error de memoria al calcular la ruta turistica.\n");
        free(visitado);
        free(padre);
        free(camino);
        return 0;
    }

    int puntoActual = 0;
    int verticeActual = ObtenerVerticePuntoTuristico(puntoActual);
    int visitados = 1;

    /*
     * rutaCompleta acumula todos los vértices recorridos por los distintos
     * tramos BFS. Al final se usa para dibujar la ruta en el SVG.
     */
    int capacidadRuta = nVertices * nPuntos;
    int *rutaCompleta = malloc(capacidadRuta * sizeof(int));
    int largoRutaCompleta = 0;

    if (rutaCompleta == NULL) {
        printf("Error de memoria al guardar la ruta completa.\n");
        free(visitado);
        free(padre);
        free(camino);
        return 0;
    }

    if (verticeActual < 0) {
        printf("No se pudo ubicar el primer punto turistico en el grafo.\n");
        free(visitado);
        free(padre);
        free(camino);
        free(rutaCompleta);
        return 0;
    }

    rutaCompleta[largoRutaCompleta] = verticeActual;
    largoRutaCompleta++;

    visitado[puntoActual] = 1;

    printf("\nRuta turistica:\n");
    printf("Inicio en %s.\n", ObtenerNombrePuntoTuristico(puntoActual));

    while (visitados < nPuntos) {
        int siguientePunto = -1;

        for (int i = 0; i < nPuntos; i++) {
            if (!visitado[i]) {
                siguientePunto = i;
                break;
            }
        }

        if (siguientePunto == -1)
            break;

        int verticeDestino = ObtenerVerticePuntoTuristico(siguientePunto);

        if (verticeDestino < 0) {
            printf("No se pudo ubicar el punto turistico %s en el grafo.\n",
                   ObtenerNombrePuntoTuristico(siguientePunto));
            free(visitado);
            free(padre);
            free(camino);
            free(rutaCompleta);
            return 0;
        }

        printf("\nTramo: %s -> %s\n",
               ObtenerNombrePuntoTuristico(puntoActual),
               ObtenerNombrePuntoTuristico(siguientePunto));

        if (!BFS(grafo, nVertices, verticeActual, verticeDestino, padre)) {
            printf("No existe una ruta factible desde %s hasta %s.\n",
                   ObtenerNombrePuntoTuristico(puntoActual),
                   ObtenerNombrePuntoTuristico(siguientePunto));
            free(visitado);
            free(padre);
            free(camino);
            free(rutaCompleta);
            return 0;
        }

        int largo = 0;

        if (!ConstruirCaminoBFS(padre, verticeActual, verticeDestino, camino, &largo)) {
            printf("No se pudo reconstruir el camino encontrado por BFS.\n");
            free(visitado);
            free(padre);
            free(camino);
            free(rutaCompleta);
            return 0;
        }

        ImprimirCaminoTuristico(camino, largo);

        /*
         * Concatenar este tramo a la ruta completa. Se omite el primer
         * vértice del tramo para no repetir el vértice donde empieza.
         */
        for (int i = 1; i < largo; i++) {
            if (largoRutaCompleta < capacidadRuta) {
                rutaCompleta[largoRutaCompleta] = camino[i];
                largoRutaCompleta++;
            }
        }

        for (int i = 0; i < largo; i++) {
            int indicePunto = ObtenerIndicePuntoPorVertice(camino[i]);

            if (indicePunto >= 0 && indicePunto < nPuntos && !visitado[indicePunto]) {
                visitado[indicePunto] = 1;
                visitados++;

                if (indicePunto != siguientePunto) {
                    printf("Tambien se visito %s durante este tramo.\n",
                           ObtenerNombrePuntoTuristico(indicePunto));
                }
            }
        }

        /* En caso de que el destino no haya sido marcado dentro del ciclo anterior. */
        if (!visitado[siguientePunto]) {
            visitado[siguientePunto] = 1;
            visitados++;
        }

        puntoActual = siguientePunto;
        verticeActual = verticeDestino;
    }

    printf("\nTodos los puntos turisticos fueron visitados.\n");

    if (largoRutaCompleta > 1) {
        GenerarVisualizacionRuta(rutaCompleta, largoRutaCompleta);
    }

    free(visitado);
    free(padre);
    free(camino);
    free(rutaCompleta);

    return 1;
}
