/* ---------- Tamaños máximos para cada campo ---------- */
#define TAM_CODIGO 15
#define TAM_NOMBRE 50
#define TAM_TIPO 30
#define TAM_VERSION 20
#define TAM_UBICACION 50
#define TAM_ESTADO 20

/* Nombre del archivo donde se guardan los datos */
#define NOMBRE_ARCHIVO "elementos.txt"

/* Códigos ANSI para dar color al texto en consola */
#define COLOR_RESET     "\033[0m"
#define COLOR_VERDE     "\033[32m"
#define COLOR_ROJO      "\033[31m"
#define COLOR_AMARILLO  "\033[33m"
#define COLOR_CIAN      "\033[36m"
#define COLOR_NEGRITA   "\033[1m"

/* Ancho usado para las tablas y bordes en pantalla */
#define ANCHO_TABLA 90

/* Es una lista doblemente enlazada:
   cada nodo apunta al anterior y al siguiente. */
typedef struct ElementoConfig {
    char codigo[TAM_CODIGO];
    char nombre[TAM_NOMBRE];
    char tipo[TAM_TIPO];
    char version[TAM_VERSION];
    char ubicacion[TAM_UBICACION];
    char estado[TAM_ESTADO];
    struct ElementoConfig *anterior;
    struct ElementoConfig *siguiente;
} ElementoConfig;

/* Agrupa el primero y el ultimo de la lista en una sola estructura.
   Así, las funciones que necesitan modificarlos reciben un solo
   puntero (ListaConfig *) en vez de un doble puntero. */
typedef struct {
    ElementoConfig *primero;
    ElementoConfig *ultimo;
} ListaConfig;

/* ---------- Funciones principales del CMDB ---------- */
void registrarElemento(ListaConfig *lista);
void modificarElemento(ElementoConfig *primero);
void buscarElemento(ElementoConfig *primero);
void eliminarElemento(ListaConfig *lista);
void mostrarTodos(ElementoConfig *primero);
void generarReportePorTipo(ElementoConfig *primero);

/* Busca un elemento por codigo. Devuelve NULL si no existe. */
ElementoConfig *buscarPorCodigo(ElementoConfig *primero, char codigo[]);

/* ---------- Validacion de datos ---------- */
/* Piden el tipo y el estado en mayusculas y no dejan continuar hasta
   que el usuario ingrese exactamente uno de los valores permitidos.
   Se exige mayusculas para poder comparar con strcmp sin ambiguedad. */
void leerTipoValido(char destino[], int tamano);
void leerEstadoValido(char destino[], int tamano);

/* ---------- Manejo de archivo ---------- */
void guardarEnArchivo(ElementoConfig *primero);
void cargarDesdeArchivo(ListaConfig *lista);

/* ---------- Funciones auxiliares ---------- */
void leerTexto(char destino[], int tamano, char mensaje[]);
int leerOpcionMenu(void);
void limpiarBufferEntrada(void);
void liberarLista(ListaConfig *lista);

/* ---------- Interfaz de consola ---------- */
void imprimirLinea(char caracter, int longitud);
void mostrarMenu(void);
