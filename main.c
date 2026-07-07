#include <stdio.h>
#include <stdlib.h>
#include "configuracion.h"

int main(void) {
    /* "lista" agrupa el primero y el ultimo en una sola variable.
       Se pasa su direccion (&lista) a las funciones que necesitan
       modificarla; sigue siendo un solo puntero porque apunta a la
       estructura, no a otro puntero */
    ListaConfig lista = { NULL, NULL };
    int opcion;

    cargarDesdeArchivo(&lista);

    do {
        mostrarMenu();
        opcion = leerOpcionMenu();

        switch (opcion) {
            case 1:
                registrarElemento(&lista);
                break;
            case 2:
                modificarElemento(lista.primero);
                break;
            case 3:
                buscarElemento(lista.primero);
                break;
            case 4:
                eliminarElemento(&lista);
                break;
            case 5:
                mostrarTodos(lista.primero);
                break;
            case 6:
                generarReportePorTipo(lista.primero);
                break;
            case 7:
                guardarEnArchivo(lista.primero);
                printf(COLOR_AMARILLO "Guardando datos y saliendo del programa...\n" COLOR_RESET);
                break;
            default:
                printf(COLOR_ROJO "Opcion invalida. Por favor intente de nuevo.\n" COLOR_RESET);
        }

    } while (opcion != 7);

    liberarLista(&lista);

    return 0;
}
