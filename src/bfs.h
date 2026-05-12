#ifndef TAREA2026_1_BFS_H
#define TAREA2026_1_BFS_H

int BFS(int **grafo, int nVertices, int origen, int destino, int padre[]);
int ConstruirCaminoBFS(int padre[], int origen, int destino, int camino[], int *largo);
void ImprimirCamino(int padre[], int origen, int destino);
void ImprimirCaminoTuristico(int camino[], int largo);
int CalcularRutaTuristica(int **grafo, int nVertices);

#endif