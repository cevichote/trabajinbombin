/*
Simulación de Sistema de Urgencias Hospitalarias con Triage Dinámico.
Nombres: Trinidad Morales, Renato Bustamante y Nicolás Becerra.
Materia: Estructuras de datos.
Sección: 1.
Profesor: Philip Vasquez.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct datos{ //Estructura de información asociada a cada paciente.
    char id[5];
    int edad, prioridad, tickllegada, ticksestimados, ticksespera;
    float p_empeorar, p_mejorar;
} Datos;

typedef struct nododoble{ //Estructura de nodo utilizado para la lista doble del historial.
    Datos* data;
    struct nododoble* siguiente;
    struct nododoble* anterior;
} NodoDoble;

typedef struct nodo{ //Estructura de nodo utilizado en las colas de pacientes.
    Datos* data;
    struct nodo* siguiente;
} Nodo;

typedef struct CambioPrioridad { //Estructura de registro de cambios de prioridades de los pacientes.
    char idPaciente[5];
    int prioridadAnterior;
    int prioridadNueva;
} CambioPrioridad;

typedef struct NodoPila { //Estructura de nodo de la pila que almacena los cambios de prioridad.
    CambioPrioridad dato;
    struct NodoPila *sig;
} NodoPila;

typedef struct col{ //Estructura de cola para almacenar a los pacientes.
    Nodo* cabecera;
    Nodo* final;
} Cola;

typedef struct lista { //Estructura de lista doblemente enlazada para guardar pacientes ya atendidos.
    NodoDoble* cabecera;
    NodoDoble* final;
} Lista;

typedef struct { //Estructura de piola que almacena el historial de cambios de prioridad.
    NodoPila *tope;
} Pila;

typedef struct Medico{ //Estructura de información sobre cada médico.
    int id, duracion;
    Nodo* pacienteactual;
}medico;

void validarmalloc(void*);
void inicializarColas(Cola* cola[]);
void inicializarMedicos(medico Medicos[], int medicos);
void leerdatos(Cola*, int*, int*);
void encolar(Nodo*, Cola*);
Nodo* desencolar(Cola*, Nodo*);
int probabilidad(Nodo*);
void insertarHistorial(Lista* , Nodo*);
void revertirUltimoCambio(Pila *, Cola*[]);
void registrarCambioPrioridad(Pila*, char*, int, int);
void mostrarHistorialCambios(Pila *);
void simularHospital(Cola* listapacientes, Cola* cola[], Pila* pila, medico Medicos[], int medicos, int pacientes, Lista* historial);
void liberarLista(Lista*);
void liberarPila(Pila* );
void calcularMetricas(Lista*);
void liberarRecursos(Lista* historial, Pila* pila, Cola* listapacientes, Cola* cola[]);

int main(){
    int medicos, pacientes;
    srand(time(NULL));
    Cola* listapacientes = (Cola*)malloc(sizeof(Cola));
    validarmalloc(listapacientes);
    leerdatos(listapacientes, &medicos, &pacientes);
    Cola* cola[4];
    inicializarColas(cola);
    Pila* pila = (Pila*)malloc(sizeof(Pila));
    validarmalloc(pila);
    pila->tope = NULL;
    medico Medicos[medicos];
    inicializarMedicos(Medicos, medicos);
    Lista historial;
    historial.cabecera = NULL;
    historial.final = NULL;
    simularHospital(listapacientes, cola, pila, Medicos, medicos, pacientes, &historial);
    calcularMetricas(&historial);
    liberarRecursos(&historial, pila, listapacientes, cola);
    return 0;
}

void validarmalloc(void* puntero){ //Función que verifica la reserva de memoria.
    if (puntero == NULL){
        printf("Error al reservar memoria\n");
        exit(1);
    }
    else{
        return;
    }
}

void inicializarColas(Cola* cola[]){ //Función que inicializa las colas de prioridad.
    int i;
    for(i = 0; i < 4; i++){
        cola[i] = (Cola*)malloc(sizeof(Cola));
        validarmalloc(cola[i]);
        cola[i]->cabecera = NULL;
        cola[i]->final = NULL;
    }
}

void inicializarMedicos(medico Medicos[], int medicos){ //Función que inicializa los médicos.
    int i;
    for(i = 0; i < medicos; i++){
        Medicos[i].id = i + 1;
        Medicos[i].duracion = 0;
        Medicos[i].pacienteactual = NULL;
    }
}

void leerdatos(Cola* lista, int *medicos, int* pacientes){ //Función que lee los datos del archivo de entrada.
    FILE *entrada = fopen ("Entrada.txt", "r");
    if (entrada == NULL){
        printf("No se pudo abrir el archivo\n");
        exit (1);
    }
    fscanf (entrada, "%d\n%d\n", medicos, pacientes);    
    lista->cabecera = NULL;
    lista->final = NULL;
    while(!feof(entrada)){
        Nodo* p = (Nodo*)malloc(sizeof(Nodo));
        validarmalloc(p);
        Datos* d = (Datos*) malloc(sizeof(Datos));
        validarmalloc(d);
        int contador = fscanf(entrada, " %[^;];%d;%d;%d;%d;%f;%f\n", d->id, &d->edad, &d->prioridad, &d->tickllegada, &d->ticksestimados, &d->p_empeorar, &d->p_mejorar);
        if(contador != 7){
            free(d);
            free(p);
            break;
        }
        p->data = d;
        encolar(p, lista);
    }
    fclose(entrada);
} 

void encolar(Nodo* paciente, Cola* cola){ //Función que inserta un paciente al final de una cola.
    paciente->siguiente = NULL;
    if(cola->cabecera == NULL){
        cola->cabecera = paciente;
        cola->final = paciente;
        return;
    }
    cola->final->siguiente = paciente;
    cola->final = paciente;
}

Nodo* desencolar(Cola* cola, Nodo* paciente){ //Función que extrae un paciente de la cola.
    Nodo* aux = cola->cabecera;
    Nodo* previo = cola->cabecera;
    if (cola->cabecera == NULL) return NULL;
    else if(aux == paciente){  //Si el paciente se encuentra al tope de la cola, se desencola normalmente.
        cola->cabecera = paciente->siguiente;
        if(cola->cabecera == NULL) cola->final = NULL;
        paciente->siguiente = NULL;
        return paciente;
    }
    else{ //Para desencolar en cualquier posición.
        aux = aux->siguiente;
        while(aux != paciente){
            previo = aux;
            aux = aux->siguiente;
        }
        previo->siguiente = aux->siguiente;
        aux->siguiente = NULL;
        if (aux == cola->final) cola->final = previo;
        return aux;
    }   
}

int probabilidad(Nodo* paciente){ //Función que determina si la prioridad del paciente se mantiene, mejora o empeora.
    float aleatorio = (float)rand()/(float)RAND_MAX;
    if (aleatorio >= 1.0 - paciente->data->p_mejorar){
        return -1;
    }
    else if (aleatorio <= paciente->data->p_empeorar){
        return 1;
    }
    return 0; 
}

void insertarHistorial(Lista* l, Nodo* paciente) { //Función que inserta un paciente atendido en el historial.
    NodoDoble* nuevo = (NodoDoble*)malloc(sizeof(NodoDoble));
    validarmalloc(nuevo);
    nuevo->data = paciente->data;
    nuevo->siguiente = NULL;
    nuevo->anterior = l->final;
    if (l->final != NULL) {
        l->final->siguiente = nuevo;
    } 
    else {
        l->cabecera = nuevo; 
    }
    l->final = nuevo; 
}

void registrarCambioPrioridad(Pila *p, char* idPaciente, int prioridadAnterior, int prioridadNueva) { //Función que registra el cambio de prioridad a la pila.
    NodoPila *nuevo = (NodoPila *)malloc(sizeof(NodoPila));
    validarmalloc(nuevo);
    strcpy(nuevo->dato.idPaciente, idPaciente);
    nuevo->dato.prioridadAnterior = prioridadAnterior;
    nuevo->dato.prioridadNueva = prioridadNueva;
    nuevo->sig = p->tope;
    p->tope = nuevo;
}

void revertirUltimoCambio(Pila *p, Cola* cola[4]) {  //Función que revierte el último cambio de prioridad registrado.
    if (p->tope == NULL) {
        return;  
    }
    Nodo* paciente = cola[p->tope->dato.prioridadNueva - 1]->cabecera;
    while(paciente != NULL && (strcmp(paciente->data->id, p->tope->dato.idPaciente) != 0)){
        paciente = paciente->siguiente;
    }
    if (paciente != NULL){
        paciente->data->prioridad = p->tope->dato.prioridadAnterior;
        desencolar(cola[p->tope->dato.prioridadNueva - 1], paciente);
        encolar(paciente, cola[p->tope->dato.prioridadAnterior - 1]);
        printf("\nPaciente %s revierte prioridad de %d a %d\n", paciente->data->id, p->tope->dato.prioridadNueva, p->tope->dato.prioridadAnterior);
    }
    NodoPila *aux = p->tope;
    p->tope = aux->sig;
    free(aux);
    return;
} 

void mostrarHistorialCambios(Pila *p) { //Función que muestra todos los cambios de prioridad.
    NodoPila *aux = p->tope;
    printf("\n===== HISTORIAL DE CAMBIOS =====\n");
    if (aux == NULL) {
        printf("No hay cambios registrados.\n");
        return;
    }
    while (aux != NULL) {
        printf("Paciente %s: %d -> %d\n",aux->dato.idPaciente, aux->dato.prioridadAnterior, aux->dato.prioridadNueva);
        aux = aux->sig;
    }
}

void simularHospital(Cola* listapacientes, Cola* cola[], Pila* pila, medico Medicos[], int medicos, int pacientes, Lista* historial){ //Función que ejecuta la simulación completa del hospital.
    int cambio;
    int pacienteslistos = 0;
    int tick = 0;
    int j, k;
    while(1){
        Nodo* pacienteactual = listapacientes->cabecera;
        while(pacienteactual != NULL){
            Nodo* sig = pacienteactual->siguiente;
            if(pacienteactual->data->tickllegada == tick){
                Nodo* recien_llegado = desencolar(listapacientes, pacienteactual);
                encolar(recien_llegado, cola[recien_llegado->data->prioridad - 1]);
                printf("\n ====== NUEVO PACIENTE ======\n");
                printf("Paciente %s tick %d | Edad: %d anios | Prioridad: %d | Tick de entrada: %d | Tick de atencion estimado: %d ticks\n", recien_llegado->data->id, tick, recien_llegado->data->edad, recien_llegado->data->prioridad, recien_llegado->data->tickllegada, recien_llegado->data->ticksestimados);
                printf("\n=========================\n");
            }
            pacienteactual = sig;
        }
        for(j = 0; j < 4; j++){
            pacienteactual = cola[j]->cabecera;
            if(pacienteactual != NULL){
                while(pacienteactual != NULL){
                    k = pacienteactual->data->prioridad;
                    cambio = probabilidad(pacienteactual);
                    if((cambio != 0) && (k + cambio >= 1) && (k + cambio <= 4)){
                        pacienteactual->data->prioridad += cambio;
                        Nodo* aux = pacienteactual;
                        printf("\ntick(%d) Paciente %s cambia de prioridad %d a %d\n", tick, aux->data->id, k, aux->data->prioridad);
                        registrarCambioPrioridad(pila, aux->data->id, k, pacienteactual->data->prioridad);
                        pacienteactual = pacienteactual->siguiente;
                        encolar(desencolar(cola[j], aux), cola[aux->data->prioridad - 1]);
                    }
                    else{
                        pacienteactual = pacienteactual->siguiente;
                    }
                }
            }
        }
        for(j = 0; j < medicos; j++){
            if(Medicos[j].pacienteactual != NULL){
                Medicos[j].pacienteactual->data->ticksestimados--;
                Medicos[j].duracion++;
                if(Medicos[j].pacienteactual->data->ticksestimados <= 0){
                    printf("\ntick(%d) El Medico %d ha terminado de atender al paciente %s\n", tick, Medicos[j].id, Medicos[j].pacienteactual->data->id);
                    Medicos[j].pacienteactual->data->ticksespera = tick - Medicos[j].pacienteactual->data->tickllegada - Medicos[j].duracion;
                    insertarHistorial(historial, Medicos[j].pacienteactual);
                    free(Medicos[j].pacienteactual);
                    Medicos[j].duracion = 0;
                    Medicos[j].pacienteactual = NULL;
                    pacienteslistos++;
                }
            }
        }
        for(j = 0; j < medicos; j++){
            if(Medicos[j].pacienteactual == NULL){
                for(k = 0; k < 4; k++){
                    if(cola[k]->cabecera != NULL){
                        Nodo* atendiendo = desencolar(cola[k], cola[k]->cabecera);
                        Medicos[j].pacienteactual = atendiendo;
                        Medicos[j].duracion = 0;
                        printf("\ntick(%d) Medico %d esta atendiendo a paciente %s con prioridad %d\n", tick, Medicos[j].id, atendiendo->data->id, atendiendo->data->prioridad);
                        break;
                    }
                }
            }
        }
        if(pacienteslistos == pacientes){
            break;
        }
        int random = rand()%10;
        if(pila->tope != NULL && random == 0){
            revertirUltimoCambio(pila, cola);
        }
        tick++;
    }
}

void liberarLista(Lista* l){ //Función que libera toda la memoria utilizada por la lista.
    if(l == NULL) return;
    NodoDoble* aux = l->cabecera;
    while(aux != NULL){
        NodoDoble* aux2 = aux;
        aux = aux->siguiente;
        if(aux2->data != NULL){
            free(aux2->data);
        }
        free(aux2);
    }   
}

void liberarPila(Pila* p){ //Función que libera toda la memoria utilizada por la pila.
    if(p == NULL) return;
    NodoPila* aux = p->tope;
    while(aux != NULL){
        NodoPila* aux2 = aux;
        aux = aux->sig;
        free(aux2);
    }
}

void calcularMetricas(Lista* historial){ // Función que calcula las metricas pedidas.
    if(historial->cabecera == NULL) return;
    NodoDoble* aux = historial->cabecera;
    int* sumatiempo_espera = (int*)malloc(4 * sizeof(int));
    validarmalloc(sumatiempo_espera);
    int* pacientes_por_prioridad = (int*)malloc(4 * sizeof(int));
    validarmalloc(pacientes_por_prioridad);
    int i;
    for(i = 0; i < 4; i++){
        sumatiempo_espera[i] = 0;
        pacientes_por_prioridad[i] = 0;
    }
    int espera_max = 0, pacientes_criticos = 0, criticos_atendidos = 0, umbral = 5;
    float promedio_espera = 0, proporcion = 0, tesp = 0, ca = 0;
    while(aux != NULL){
        if(aux->data->prioridad >= 1 && aux->data->prioridad <= 4){
            sumatiempo_espera[aux->data->prioridad -1] = sumatiempo_espera[aux->data->prioridad -1] + aux->data->ticksespera;
            pacientes_por_prioridad[aux->data->prioridad -1 ]++;
        }

        if(aux->data->ticksespera > espera_max){
            espera_max = aux->data->ticksespera;
        }
        if(aux->data->prioridad == 1){
            pacientes_criticos++;
            if(aux->data->ticksespera <= umbral){
                criticos_atendidos++;
            }
        }
        aux = aux->siguiente;
    }
    printf("\n ===== METRICAS ===== \n");
    for(i = 0; i < 4; i++){
        if(pacientes_por_prioridad[i] > 0){
            tesp = sumatiempo_espera[i];
            promedio_espera = tesp / pacientes_por_prioridad[i];
            printf("Tiempo de espera promedio en prioridad %d: %.2f ticks\n", i + 1, promedio_espera);
        }
    }
    printf("Tiempo maximo de espera: %d\n", espera_max);
    if(pacientes_criticos > 0){
        ca = criticos_atendidos;
        proporcion = (ca / pacientes_criticos) * 100.0;
        printf("Pacientes criticos atendidos a tiempo: %.2f\n", proporcion);
    }
    free(sumatiempo_espera);
    free(pacientes_por_prioridad);
}

void liberarRecursos(Lista* historial, Pila* pila, Cola* listapacientes, Cola* cola[]){ //Función que libera la memoria utilizada.
    int i;
    liberarLista(historial);
    liberarPila(pila);
    free(listapacientes);
    for(i = 0; i < 4; i++){
        free(cola[i]);
    }
    free(pila);
}