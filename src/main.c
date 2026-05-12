#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "archivos.h"
#include "bfs.h"

#define MAX_NOMBRE_ARCHIVO 256
#define MAX_RESPUESTA 20

int main(void) {
    char nombretxt[MAX_NOMBRE_ARCHIVO];
    char respuesta[MAX_RESPUESTA];
    int n;
    int **grafo;
    int continuar = 1;

    while (continuar) {
        /*Pedir archivo válido*/
        while (1) {
            printf("Ingrese el nombre del archivo (.txt): ");

            if (scanf("%255s", nombretxt) != 1) {
                while (getchar() != '\n');

                printf("Entrada inválida\n");
                continue;
            }

            FILE *test = fopen(nombretxt, "r");
            if (!test) {
                printf("El archivo '%s' no existe\n\n", nombretxt);
                continue;
            }

            fclose(test);
            break;
        }

        /*Cargar grafo*/
        grafo = CargarGrafo(nombretxt, &n);

        if (!grafo) {
            printf("Error al construir el grafo\n\n");
            continue;
        }

        printf("\nGrafo cargado correctamente.\n");
        printf("Cantidad de vertices: %d\n", n);

        /*Calcular ruta turística usando BFS*/
        CalcularRutaTuristica(grafo, n);


        /*Liberar memoria*/
        LiberarGrafo(grafo, n);

        /*Preguntar si desea continuar*/
        while (1) {
            printf("Desea leer otro archivo? (Si/No): ");

            if (scanf("%19s", respuesta) != 1) {
                while (getchar() != '\n');

                printf("Entrada invalida\n");
                continue;
            }

            if (strcmp(respuesta, "SI") == 0 || strcmp(respuesta, "Si") == 0 ||
                strcmp(respuesta, "si") == 0 || strcmp(respuesta, "SÍ") == 0 ||
                strcmp(respuesta, "Sí") == 0 || strcmp(respuesta, "sí") == 0 ||
                strcmp(respuesta, "S") == 0 || strcmp(respuesta, "s") == 0) {
                printf("\n");
                break;
            }

            if (strcmp(respuesta, "NO") == 0 || strcmp(respuesta, "No") == 0 ||
                strcmp(respuesta, "no") == 0 || strcmp(respuesta, "N") == 0 ||
                strcmp(respuesta, "n") == 0) {
                continuar = 0;
                break;
            }

            printf("Respuesta inválida. Use 'Si' o 'No'.\n");
        }
    }

    printf("Gracias por usar el programa. Hasta luego!\n");
    return 0;
}