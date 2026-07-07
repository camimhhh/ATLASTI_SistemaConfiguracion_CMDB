#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "configuracion.h"

/* Dibuja una linea repitiendo un caracter (sirve para bordes y tablas) */
void imprimirLinea(char caracter, int longitud) {
    int i;
    for (i = 0; i < longitud; i++) {
        printf("%c", caracter);
    }
    printf("\n");
}

void mostrarMenu(void) {
    printf("\n");
    printf(COLOR_CIAN);
    imprimirLinea('=', 55);
    printf(COLOR_NEGRITA "     SISTEMA DE GESTION DE CONFIGURACION (CMDB)\n" COLOR_RESET COLOR_CIAN);
    imprimirLinea('=', 55);
    printf(COLOR_RESET);

    printf("  [1] Registrar elemento\n");
    printf("  [2] Modificar elemento\n");
    printf("  [3] Buscar elemento por codigo\n");
    printf("  [4] Eliminar elemento\n");
    printf("  [5] Mostrar todos los elementos\n");
    printf("  [6] Generar reporte por tipo\n");
    printf(COLOR_ROJO "  [7] Salir\n" COLOR_RESET);

    printf(COLOR_CIAN);
    imprimirLinea('-', 55);
    printf(COLOR_RESET);
}

/* Descarta lo que haya quedado en el buffer de entrada, para que un
   dato mal ingresado no dañe la siguiente lectura */
void limpiarBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Pide un texto y no deja continuar si el campo queda vacio */
void leerTexto(char destino[], int tamano, char mensaje[]) {
    int valido = 0;
    int longitud;

    do {
        printf("%s", mensaje);
        fgets(destino, tamano, stdin);

        /* fgets deja el salto de linea al final; lo quitamos.
           Si el texto era mas largo que el buffer, limpiamos lo sobrante */
        longitud = strlen(destino);
        if (longitud > 0 && destino[longitud - 1] == '\n') {
            destino[longitud - 1] = '\0';
        } else {
            limpiarBufferEntrada();
        }

        if (strlen(destino) == 0) {
            printf(COLOR_ROJO "Error: el campo no puede quedar vacio.\n" COLOR_RESET);
        } else {
            valido = 1;
        }
    } while (valido == 0);
}

/* Lee la opcion del menu de forma segura: si el usuario escribe algo
   que no es un numero, vuelve a pedir el dato en vez de colgarse */
int leerOpcionMenu(void) {
    char linea[20];
    int opcion;
    int valido;

    do {
        printf("Seleccione una opcion: ");
        fgets(linea, sizeof(linea), stdin);
        valido = sscanf(linea, "%d", &opcion);

        if (valido != 1) {
            printf(COLOR_ROJO "Error: debe ingresar un numero valido.\n" COLOR_RESET);
        }
    } while (valido != 1);

    return opcion;
}

/* Pide el tipo en mayusculas y repite hasta que coincida exactamente
   (con strcmp) con uno de los valores permitidos. Exigir mayusculas
   evita tener que comparar ignorando mayusculas/minusculas */
void leerTipoValido(char destino[], int tamano) {
    int valido = 0;

    do {
        leerTexto(destino, tamano, "Ingrese el tipo EN MAYUSCULAS (HARDWARE, SOFTWARE, SERVICIO): ");

        if (strcmp(destino, "HARDWARE") == 0 ||
            strcmp(destino, "SOFTWARE") == 0 ||
            strcmp(destino, "SERVICIO") == 0) {
            valido = 1;
        } else {
            printf(COLOR_ROJO "Error: el tipo debe ser HARDWARE, SOFTWARE o SERVICIO (en mayusculas).\n" COLOR_RESET);
        }

    } while (valido == 0);
}

/* Igual que leerTipoValido, pero para el campo estado */
void leerEstadoValido(char destino[], int tamano) {
    int valido = 0;

    do {
        leerTexto(destino, tamano, "Ingrese el estado EN MAYUSCULAS (ACTIVO, INACTIVO, MANTENIMIENTO): ");

        if (strcmp(destino, "ACTIVO") == 0 ||
            strcmp(destino, "INACTIVO") == 0 ||
            strcmp(destino, "MANTENIMIENTO") == 0) {
            valido = 1;
        } else {
            printf(COLOR_ROJO "Error: el estado debe ser ACTIVO, INACTIVO o MANTENIMIENTO (en mayusculas).\n" COLOR_RESET);
        }

    } while (valido == 0);
}

/* Recorre la lista desde el primero comparando codigos con strcmp */
ElementoConfig *buscarPorCodigo(ElementoConfig *primero, char codigo[]) {
    ElementoConfig *actual = primero;

    while (actual != NULL) {
        if (strcmp(actual->codigo, codigo) == 0) {
            return actual;
        }
        actual = actual->siguiente;
    }
    return NULL;
}

/* Pide los datos de un nuevo elemento, valida que el codigo no exista,
   reserva memoria con malloc y lo enlaza al final de la lista.
   Recibe un puntero a ListaConfig porque necesita poder actualizar
   el primero o el ultimo de la lista */
void registrarElemento(ListaConfig *lista) {
    char codigoTemp[TAM_CODIGO];

    leerTexto(codigoTemp, TAM_CODIGO, "Ingrese el codigo del elemento: ");

    if (buscarPorCodigo(lista->primero, codigoTemp) != NULL) {
        printf(COLOR_ROJO "Error: ya existe un elemento registrado con ese codigo.\n" COLOR_RESET);
        return;
    }

    ElementoConfig *nuevo = (ElementoConfig *) malloc(sizeof(ElementoConfig));
    if (nuevo == NULL) {
        printf(COLOR_ROJO "Error: no hay memoria disponible en este momento.\n" COLOR_RESET);
        return;
    }

    strcpy(nuevo->codigo, codigoTemp);
    leerTexto(nuevo->nombre, TAM_NOMBRE, "Ingrese el nombre: ");
    leerTipoValido(nuevo->tipo, TAM_TIPO);
    leerTexto(nuevo->version, TAM_VERSION, "Ingrese la version: ");
    leerTexto(nuevo->ubicacion, TAM_UBICACION, "Ingrese la ubicacion: ");
    leerEstadoValido(nuevo->estado, TAM_ESTADO);

    nuevo->anterior = NULL;
    nuevo->siguiente = NULL;

    if (lista->primero == NULL) {
        lista->primero = nuevo;
        lista->ultimo = nuevo;
    } else {
        lista->ultimo->siguiente = nuevo;
        nuevo->anterior = lista->ultimo;
        lista->ultimo = nuevo;
    }

    guardarEnArchivo(lista->primero);
    printf(COLOR_VERDE "Elemento registrado con exito.\n" COLOR_RESET);
}

/* Busca por codigo y, si existe, permite actualizar sus datos (menos
   el codigo). Solo necesita leer la lista, no modificar primero ni
   ultimo, por eso recibe un puntero normal a ElementoConfig */
void modificarElemento(ElementoConfig *primero) {
    char codigo[TAM_CODIGO];

    leerTexto(codigo, TAM_CODIGO, "Ingrese el codigo del elemento a modificar: ");

    ElementoConfig *encontrado = buscarPorCodigo(primero, codigo);
    if (encontrado == NULL) {
        printf(COLOR_ROJO "No se encontro ningun elemento con ese codigo.\n" COLOR_RESET);
        return;
    }

    printf("Elemento encontrado. Ingrese los nuevos datos:\n");
    leerTexto(encontrado->nombre, TAM_NOMBRE, "Nuevo nombre: ");
    leerTipoValido(encontrado->tipo, TAM_TIPO);
    leerTexto(encontrado->version, TAM_VERSION, "Nueva version: ");
    leerTexto(encontrado->ubicacion, TAM_UBICACION, "Nueva ubicacion: ");
    leerEstadoValido(encontrado->estado, TAM_ESTADO);

    guardarEnArchivo(primero);
    printf(COLOR_VERDE "Elemento modificado con exito.\n" COLOR_RESET);
}

void buscarElemento(ElementoConfig *primero) {
    char codigo[TAM_CODIGO];

    leerTexto(codigo, TAM_CODIGO, "Ingrese el codigo a buscar: ");

    ElementoConfig *encontrado = buscarPorCodigo(primero, codigo);
    if (encontrado == NULL) {
        printf(COLOR_ROJO "No se encontro ningun elemento con ese codigo.\n" COLOR_RESET);
        return;
    }

    printf("\n");
    printf(COLOR_VERDE);
    imprimirLinea('-', 40);
    printf(COLOR_NEGRITA "  Elemento encontrado\n" COLOR_RESET COLOR_VERDE);
    imprimirLinea('-', 40);
    printf(COLOR_RESET);
    printf("Codigo   : %s\n", encontrado->codigo);
    printf("Nombre   : %s\n", encontrado->nombre);
    printf("Tipo     : %s\n", encontrado->tipo);
    printf("Version  : %s\n", encontrado->version);
    printf("Ubicacion: %s\n", encontrado->ubicacion);
    printf("Estado   : %s\n", encontrado->estado);
    printf(COLOR_VERDE);
    imprimirLinea('-', 40);
    printf(COLOR_RESET);
}

/* Busca por codigo, reacomoda los punteros anterior/siguiente de sus
   vecinos y libera su memoria con free. Recibe un puntero a
   ListaConfig porque puede necesitar actualizar primero o ultimo */
void eliminarElemento(ListaConfig *lista) {
    char codigo[TAM_CODIGO];

    leerTexto(codigo, TAM_CODIGO, "Ingrese el codigo del elemento a eliminar: ");

    ElementoConfig *encontrado = buscarPorCodigo(lista->primero, codigo);
    if (encontrado == NULL) {
        printf(COLOR_ROJO "No se encontro ningun elemento con ese codigo.\n" COLOR_RESET);
        return;
    }

    if (encontrado->anterior != NULL) {
        encontrado->anterior->siguiente = encontrado->siguiente;
    } else {
        /* Se esta eliminando el primero de la lista */
        lista->primero = encontrado->siguiente;
    }

    if (encontrado->siguiente != NULL) {
        encontrado->siguiente->anterior = encontrado->anterior;
    } else {
        /* Se esta eliminando el ultimo de la lista */
        lista->ultimo = encontrado->anterior;
    }

    free(encontrado);

    guardarEnArchivo(lista->primero);
    printf(COLOR_VERDE "Elemento eliminado con exito.\n" COLOR_RESET);
}

void mostrarTodos(ElementoConfig *primero) {
    if (primero == NULL) {
        printf(COLOR_ROJO "No hay elementos registrados todavia.\n" COLOR_RESET);
        return;
    }

    ElementoConfig *actual = primero;
    int total = 0;

    printf("\n");
    printf(COLOR_CIAN);
    imprimirLinea('=', ANCHO_TABLA);
    printf(COLOR_NEGRITA "%-10s %-20s %-15s %-10s %-20s %-10s\n" COLOR_RESET COLOR_CIAN,
           "CODIGO", "NOMBRE", "TIPO", "VERSION", "UBICACION", "ESTADO");
    imprimirLinea('-', ANCHO_TABLA);
    printf(COLOR_RESET);

    while (actual != NULL) {
        printf("%-10s %-20s %-15s %-10s %-20s %-10s\n",
               actual->codigo, actual->nombre, actual->tipo,
               actual->version, actual->ubicacion, actual->estado);
        actual = actual->siguiente;
        total++;
    }

    printf(COLOR_CIAN);
    imprimirLinea('=', ANCHO_TABLA);
    printf(COLOR_RESET);
    printf("Total de elementos: %d\n", total);
}

/* Muestra solo los elementos que coincidan con el tipo ingresado
   (comparacion sin distinguir mayusculas/minusculas) */
void generarReportePorTipo(ElementoConfig *primero) {
    if (primero == NULL) {
        printf(COLOR_ROJO "No hay elementos registrados todavia.\n" COLOR_RESET);
        return;
    }

    char tipoBuscado[TAM_TIPO];
    leerTipoValido(tipoBuscado, TAM_TIPO);

    ElementoConfig *actual = primero;
    int contador = 0;

    printf("\n");
    printf(COLOR_CIAN);
    imprimirLinea('=', ANCHO_TABLA);
    printf(COLOR_NEGRITA "  Reporte de elementos de tipo: %s\n" COLOR_RESET COLOR_CIAN, tipoBuscado);
    imprimirLinea('-', ANCHO_TABLA);
    printf(COLOR_RESET);

    while (actual != NULL) {
        if (strcmp(actual->tipo, tipoBuscado) == 0) {
            printf("Codigo: %-10s Nombre: %-20s Version: %-10s Ubicacion: %-20s Estado: %-10s\n",
                   actual->codigo, actual->nombre, actual->version,
                   actual->ubicacion, actual->estado);
            contador++;
        }
        actual = actual->siguiente;
    }

    printf(COLOR_CIAN);
    imprimirLinea('=', ANCHO_TABLA);
    printf(COLOR_RESET);

    if (contador == 0) {
        printf(COLOR_ROJO "No se encontraron elementos de ese tipo.\n" COLOR_RESET);
    } else {
        printf(COLOR_AMARILLO "Total de elementos encontrados: %d\n" COLOR_RESET, contador);
    }
}

/* Guarda todos los elementos en el archivo de texto, separando los
   campos con '|'. Se llama despues de registrar, modificar o eliminar */
void guardarEnArchivo(ElementoConfig *primero) {
    FILE *archivo = fopen(NOMBRE_ARCHIVO, "w");
    if (archivo == NULL) {
        printf(COLOR_ROJO "Error: no se pudo abrir el archivo para guardar los datos.\n" COLOR_RESET);
        return;
    }

    ElementoConfig *actual = primero;
    while (actual != NULL) {
        fprintf(archivo, "%s|%s|%s|%s|%s|%s\n",
                actual->codigo, actual->nombre, actual->tipo,
                actual->version, actual->ubicacion, actual->estado);
        actual = actual->siguiente;
    }

    fclose(archivo);
}

/* Se ejecuta al iniciar el programa: si el archivo no existe lo crea
   vacio, si existe lo lee linea por linea y reconstruye la lista.
   Recibe un puntero a ListaConfig porque arma la lista desde cero */
void cargarDesdeArchivo(ListaConfig *lista) {
    FILE *archivo = fopen(NOMBRE_ARCHIVO, "r");

    if (archivo == NULL) {
        archivo = fopen(NOMBRE_ARCHIVO, "w");
        if (archivo != NULL) {
            fclose(archivo);
        }
        return;
    }

    char linea[250];
    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        ElementoConfig *nuevo = (ElementoConfig *) malloc(sizeof(ElementoConfig));
        if (nuevo == NULL) {
            printf(COLOR_ROJO "Error: no hay memoria disponible al cargar los datos.\n" COLOR_RESET);
            break;
        }

        int camposLeidos = sscanf(linea, "%14[^|]|%49[^|]|%29[^|]|%19[^|]|%49[^|]|%19[^\n]",
               nuevo->codigo, nuevo->nombre, nuevo->tipo,
               nuevo->version, nuevo->ubicacion, nuevo->estado);

        /* Si la linea no tenia los 6 campos completos, se descarta para
           no guardar un elemento con datos incompletos o basura */
        if (camposLeidos != 6) {
            free(nuevo);
            continue;
        }

        nuevo->anterior = NULL;
        nuevo->siguiente = NULL;

        if (lista->primero == NULL) {
            lista->primero = nuevo;
            lista->ultimo = nuevo;
        } else {
            lista->ultimo->siguiente = nuevo;
            nuevo->anterior = lista->ultimo;
            lista->ultimo = nuevo;
        }
    }

    fclose(archivo);
}

/* Libera la memoria de cada elemento antes de terminar el programa */
void liberarLista(ListaConfig *lista) {
    ElementoConfig *actual = lista->primero;
    ElementoConfig *siguienteTemp;

    while (actual != NULL) {
        siguienteTemp = actual->siguiente;
        free(actual);
        actual = siguienteTemp;
    }

    lista->primero = NULL;
    lista->ultimo = NULL;
}
