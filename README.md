# Proyecto Semestral - Matemáticas Discretas Tarea2026-1

## Integrantes

- Gustavo González
- Joaquin Reyes
- Roberto Cruz

## Descripción

Este proyecto consiste en modelar el mapa de una ciudad como un digrafo.  
El programa recibe un archivo `.txt` con la información de las calles y puntos de interés, construye el grafo correspondiente y busca rutas dentro del mapa.

## Requisitos

Para compilar y ejecutar el programa se necesita tener instalado `gcc`.

## Compilación

Desde la carpeta principal del proyecto, ejecutar:

```bash
gcc src/main.c src/archivos.c src/bfs.c -o programa -lm
```

Para ejecutar:
```./programa```
