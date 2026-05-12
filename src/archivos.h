#ifndef TAREA2026_1_ARCHIVOS_H
#define TAREA2026_1_ARCHIVOS_H

#define MAX_NOMBRE 100

int **CargarGrafo(const char *filename, int *n);
void LiberarGrafo(int **graph, int n);

/*
 * Funciones de consulta usadas por BFS.c.
 * CargarGrafo guarda internamente los puntos turísticos y los vértices
 * para que el BFS pueda armar la ruta turística sin cambiar demasiado el main.
 */
int ObtenerCantidadPuntosTuristicos(void);
int ObtenerVerticePuntoTuristico(int indicePunto);
const char *ObtenerNombrePuntoTuristico(int indicePunto);
const char *ObtenerNombreVertice(int indiceVertice);
int EsVerticeTuristico(int indiceVertice);
int ObtenerIndicePuntoPorVertice(int indiceVertice);

#endif
