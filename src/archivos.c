#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "archivos.h"

#define MAX_CALLES 50
#define MAX_PUNTOS 500
#define MAX_VERTICES 500
#define EPS 1e-6

typedef struct {
    char nombre[MAX_NOMBRE];
    double x1, y1;
    double x2, y2;
    char eje;
} Calle;

typedef struct {
    char nombre[MAX_NOMBRE];
    char calle[MAX_NOMBRE];
    double posicion;
    int vertice;
} PuntoDeInteres;

typedef struct {
    char nombre[MAX_NOMBRE];
    double x, y;
    int PuntoInteres;
    int indicePunto;
} Vertice;

typedef struct {
    int indices[MAX_VERTICES];
    double posiciones[MAX_VERTICES];
    int cantidad;
} ListaVerticesCalle;

/*
 * Estos arreglos quedan guardados después de CargarGrafo.
 * Así BFS.c puede saber qué vértice corresponde a cada punto turístico.
 */
static Vertice verticesGlobal[MAX_VERTICES];
static PuntoDeInteres puntosGlobal[MAX_PUNTOS];
static int cantidadVerticesGlobal = 0;
static int cantidadPuntosGlobal = 0;

static int MismoPunto(double x1, double y1, double x2, double y2) {
    return fabs(x1 - x2) < EPS && fabs(y1 - y2) < EPS;
}

/*
 * Devuelve la posición de un punto sobre la calle según el eje de numeración.
 * Si la calle está numerada por X, se ordena por coordenada X.
 * Si está numerada por Y, se ordena por coordenada Y.
 */
static double PuntoEnCalle(Calle c, double x, double y) {
    if (c.eje == 'X')
        return x;
    if (c.eje == 'Y')
        return y;

    return sqrt((x - c.x1) * (x - c.x1) + (y - c.y1) * (y - c.y1));
}

/*
 * Obtiene las coordenadas reales de una posición dentro de una calle.
 * Esto también sirve para calles diagonales, porque se interpola sobre el segmento.
 */
static void InteresEnCalle(Calle c, double posicion, double *x, double *y) {
    double t = 0.0;

    if (c.eje == 'X') {
        *x = posicion;

        if (fabs(c.x2 - c.x1) < EPS)
            t = 0.0;
        else
            t = (posicion - c.x1) / (c.x2 - c.x1);

        *y = c.y1 + t * (c.y2 - c.y1);
        return;
    }

    if (c.eje == 'Y') {
        *y = posicion;

        if (fabs(c.y2 - c.y1) < EPS)
            t = 0.0;
        else
            t = (posicion - c.y1) / (c.y2 - c.y1);

        *x = c.x1 + t * (c.x2 - c.x1);
        return;
    }

    /* Caso de respaldo: posición como distancia desde el punto inicial. */
    double largo = sqrt((c.x2 - c.x1) * (c.x2 - c.x1) +
                        (c.y2 - c.y1) * (c.y2 - c.y1));

    if (largo >= EPS)
        t = posicion / largo;

    *x = c.x1 + t * (c.x2 - c.x1);
    *y = c.y1 + t * (c.y2 - c.y1);
}

/* Calcula la intersección entre dos segmentos. */
static int Intersectan(Calle a, Calle b, double *x, double *y) {
    double den = (a.x1 - a.x2) * (b.y1 - b.y2) -
                 (a.y1 - a.y2) * (b.x1 - b.x2);

    if (fabs(den) < EPS)
        return 0;

    *x = ((a.x1 * a.y2 - a.y1 * a.x2) * (b.x1 - b.x2) -
          (a.x1 - a.x2) * (b.x1 * b.y2 - b.y1 * b.x2)) / den;

    *y = ((a.x1 * a.y2 - a.y1 * a.x2) * (b.y1 - b.y2) -
          (a.y1 - a.y2) * (b.x1 * b.y2 - b.y1 * b.x2)) / den;

    if (*x < fmin(a.x1, a.x2) - EPS || *x > fmax(a.x1, a.x2) + EPS ||
        *x < fmin(b.x1, b.x2) - EPS || *x > fmax(b.x1, b.x2) + EPS ||
        *y < fmin(a.y1, a.y2) - EPS || *y > fmax(a.y1, a.y2) + EPS ||
        *y < fmin(b.y1, b.y2) - EPS || *y > fmax(b.y1, b.y2) + EPS)
        return 0;

    return 1;
}

static int BuscarCalle(Calle calles[], int n, const char *nombre) {
    for (int i = 0; i < n; i++) {
        if (strcmp(calles[i].nombre, nombre) == 0)
            return i;
    }
    return -1;
}

/* Agrega un vértice si no existe. */
static int AgregarVertice(Vertice vertices[], int *n, const char *nombre,
                          double x, double y, int esTuristico, int indicePunto) {
    if (*n >= MAX_VERTICES) {
        fprintf(stderr, "Se superó el máximo de vértices.\n");
        return -1;
    }

    for (int i = 0; i < *n; i++) {
        if (MismoPunto(vertices[i].x, vertices[i].y, x, y)) {
            if (esTuristico) {
                strcpy(vertices[i].nombre, nombre);
                vertices[i].PuntoInteres = 1;
                vertices[i].indicePunto = indicePunto;
            }
            return i;
        }
    }

    strcpy(vertices[*n].nombre, nombre);
    vertices[*n].x = x;
    vertices[*n].y = y;
    vertices[*n].PuntoInteres = esTuristico;
    vertices[*n].indicePunto = indicePunto;
    (*n)++;

    return (*n) - 1;
}

static void OrdenarLista(ListaVerticesCalle *lista) {
    for (int i = 0; i < lista->cantidad - 1; i++) {
        for (int j = i + 1; j < lista->cantidad; j++) {
            if (lista->posiciones[i] > lista->posiciones[j]) {
                double tp = lista->posiciones[i];
                lista->posiciones[i] = lista->posiciones[j];
                lista->posiciones[j] = tp;

                int ti = lista->indices[i];
                lista->indices[i] = lista->indices[j];
                lista->indices[j] = ti;
            }
        }
    }
}

/*
 * Agrega una arista no dirigida.
 * El enunciado trabaja con grafos y no indica calles de un solo sentido.
 */
static void AgregarArista(int **graph, int i, int j) {
    if (i < 0 || j < 0 || i == j)
        return;

    graph[i][j] = 1;
    graph[j][i] = 1;
}

/* Carga el grafo del mapa. */
int **CargarGrafo(const char *filename, int *nVerticesOut) {
    FILE *file = fopen(filename, "r");

    if (!file) {
        perror("Error al abrir el archivo");
        return NULL;
    }

    Calle calles[MAX_CALLES];
    Vertice *vertices = verticesGlobal;
    PuntoDeInteres *puntos = puntosGlobal;
    int nCalles, nPuntos;
    int nVertices = 0;

    cantidadVerticesGlobal = 0;
    cantidadPuntosGlobal = 0;
    memset(verticesGlobal, 0, sizeof(verticesGlobal));
    memset(puntosGlobal, 0, sizeof(puntosGlobal));

    /* Leer número de calles. */
    if (fscanf(file, "%d", &nCalles) != 1) {
        fclose(file);
        return NULL;
    }

    if (nCalles > MAX_CALLES) {
        fprintf(stderr, "Demasiadas calles.\n");
        fclose(file);
        return NULL;
    }

    /* Leer calles. */
    for (int i = 0; i < nCalles; i++) {
        if (fscanf(file, "%99s %lf %lf %lf %lf %c",
                   calles[i].nombre, &calles[i].x1, &calles[i].y1,
                   &calles[i].x2, &calles[i].y2, &calles[i].eje) != 6) {
            fclose(file);
            return NULL;
        }
    }

    /* Leer puntos de interés. */
    if (fscanf(file, "%d", &nPuntos) != 1) {
        fclose(file);
        return NULL;
    }

    if (nPuntos > MAX_PUNTOS) {
        fprintf(stderr, "Demasiados puntos turisticos.\n");
        fclose(file);
        return NULL;
    }

    for (int i = 0; i < nPuntos; i++) {
        if (fscanf(file, "%99s %99s %lf",
                   puntos[i].nombre, puntos[i].calle, &puntos[i].posicion) != 3) {
            fclose(file);
            return NULL;
        }
        puntos[i].vertice = -1;
    }

    fclose(file);

    /* Agregar extremos de cada calle como vértices. */
    for (int i = 0; i < nCalles; i++) {
        AgregarVertice(vertices, &nVertices, "Extremo", calles[i].x1, calles[i].y1, 0, -1);
        AgregarVertice(vertices, &nVertices, "Extremo", calles[i].x2, calles[i].y2, 0, -1);
    }

    /* Agregar intersecciones. */
    for (int i = 0; i < nCalles; i++) {
        for (int j = i + 1; j < nCalles; j++) {
            double x, y;

            if (Intersectan(calles[i], calles[j], &x, &y)) {
                AgregarVertice(vertices, &nVertices, "Interseccion", x, y, 0, -1);
            }
        }
    }

    /* Agregar puntos turísticos. */
    for (int i = 0; i < nPuntos; i++) {
        int idx = BuscarCalle(calles, nCalles, puntos[i].calle);

        if (idx >= 0) {
            double x, y;
            InteresEnCalle(calles[idx], puntos[i].posicion, &x, &y);
            puntos[i].vertice = AgregarVertice(vertices, &nVertices, puntos[i].nombre, x, y, 1, i);
        }
    }

    /* Crear matriz de adyacencia. */
    int **graph = malloc(nVertices * sizeof(int *));

    if (!graph)
        return NULL;

    for (int i = 0; i < nVertices; i++) {
        graph[i] = calloc(nVertices, sizeof(int));

        if (!graph[i]) {
            for (int k = 0; k < i; k++)
                free(graph[k]);

            free(graph);
            return NULL;
        }
    }

    /* Conectar vértices consecutivos en cada calle. */
    for (int c = 0; c < nCalles; c++) {
        ListaVerticesCalle lista;
        lista.cantidad = 0;

        for (int v = 0; v < nVertices; v++) {
            double t = PuntoEnCalle(calles[c], vertices[v].x, vertices[v].y);
            double xTest, yTest;
            InteresEnCalle(calles[c], t, &xTest, &yTest);

            if (MismoPunto(xTest, yTest, vertices[v].x, vertices[v].y)) {
                if (lista.cantidad < MAX_VERTICES) {
                    lista.indices[lista.cantidad] = v;
                    lista.posiciones[lista.cantidad] = t;
                    lista.cantidad++;
                }
            }
        }

        if (lista.cantidad < 2)
            continue;

        OrdenarLista(&lista);

        for (int i = 0; i < lista.cantidad - 1; i++) {
            AgregarArista(graph, lista.indices[i], lista.indices[i + 1]);
        }
    }

    cantidadVerticesGlobal = nVertices;
    cantidadPuntosGlobal = nPuntos;
    *nVerticesOut = nVertices;

    return graph;
}

void LiberarGrafo(int **graph, int n) {
    if (!graph)
        return;

    for (int i = 0; i < n; i++) {
        free(graph[i]);
    }

    free(graph);
}

int ObtenerCantidadPuntosTuristicos(void) {
    return cantidadPuntosGlobal;
}

int ObtenerVerticePuntoTuristico(int indicePunto) {
    if (indicePunto < 0 || indicePunto >= cantidadPuntosGlobal)
        return -1;

    return puntosGlobal[indicePunto].vertice;
}

const char *ObtenerNombrePuntoTuristico(int indicePunto) {
    if (indicePunto < 0 || indicePunto >= cantidadPuntosGlobal)
        return "Punto desconocido";

    return puntosGlobal[indicePunto].nombre;
}

const char *ObtenerNombreVertice(int indiceVertice) {
    if (indiceVertice < 0 || indiceVertice >= cantidadVerticesGlobal)
        return "Vertice desconocido";

    return verticesGlobal[indiceVertice].nombre;
}

int EsVerticeTuristico(int indiceVertice) {
    if (indiceVertice < 0 || indiceVertice >= cantidadVerticesGlobal)
        return 0;

    return verticesGlobal[indiceVertice].PuntoInteres;
}

int ObtenerIndicePuntoPorVertice(int indiceVertice) {
    if (indiceVertice < 0 || indiceVertice >= cantidadVerticesGlobal)
        return -1;

    return verticesGlobal[indiceVertice].indicePunto;
}
