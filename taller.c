#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct datos{
    char* id;
    int edad, prioridad, tickllegada, ticksestimados, ticksespera;
    float p_empeorar, p_mejorar;
} Datos;

typedef struct nododoble{
    Datos* data;
    struct nododoble* siguiente;
    struct nododoble* anterior;
} NodoDoble;

typedef struct nodo{
    Datos* data;
    struct nodo* siguiente;
} Nodo;

typedef struct CambioPrioridad {
    char* idPaciente;
    int prioridadAnterior;
    int prioridadNueva;
} CambioPrioridad;

typedef struct NodoPila {
    CambioPrioridad dato;
    struct NodoPila *sig;
} NodoPila;

typedef struct col{
    Nodo* cabecera;
    Nodo* final;
} Cola;

typedef struct lista {
    NodoDoble* cabecera;
    NodoDoble* final;
} Lista;

typedef struct {
    NodoPila *tope;
} Pila;

typedef struct Medico{
    int id, duracion;
    Nodo* pacienteactual;
}medico;

void validarmalloc(void*);
void insertarHistorial(Lista* , Nodo*);
void encolar(Nodo*, Cola*);
Nodo* desencolar(Cola*, Nodo*); 
void registrarCambioPrioridad(Pila*, char*, int);
CambioPrioridad revertirUltimoCambio(Pila *);
void mostrarHistorialCambios(Pila *);
void leerdatos(Cola*, int*, int*);
int probabilidad(Nodo*);
void liberarLista(Lista*);

int main(){
    int medicos, pacientes, cambio, pacienteslistos = 0, j, k, tick = 0;
    srand(time(NULL));
    Cola* listapacientes = (Cola*)malloc(sizeof(Cola));
    validarmalloc(listapacientes);
    leerdatos(listapacientes, &medicos, &pacientes);
    
    Cola* cola[4];
    for (int i = 0; i < 4; i++){
        cola[i] = (Cola*)malloc(sizeof(Cola));
        validarmalloc(cola[i]);
        cola[i]->cabecera = NULL;
        cola[i]->final = NULL;
    }

    Pila* pila = (Pila*)malloc(sizeof(Pila));
    validarmalloc(pila);
    pila->tope = NULL;

    medico Medicos[medicos];
    for(j = 0; j < medicos; j++){
        Medicos[j].id = j + 1;
        Medicos[j].pacienteactual = NULL;
    }

    Lista historial;
    historial.cabecera = NULL;
    historial.final = NULL;

    while (1){
        Nodo* pacienteactual = listapacientes->cabecera;
        while(pacienteactual != NULL){
            Nodo* sig = pacienteactual->siguiente;
            if(pacienteactual->data->tickllegada == tick){
                Nodo* recien_llegado = desencolar(listapacientes, pacienteactual);
                encolar(recien_llegado,cola[recien_llegado->data->prioridad - 1]);
                printf("tick (%d) Llego el paciente %s con prioridad %d\n", tick, recien_llegado->data->id, recien_llegado->data->prioridad);
            }
            pacienteactual = sig;
        }
        for (j = 0; j < 4; j++){
            pacienteactual = cola[j]->cabecera;
            if (pacienteactual != NULL){
                while (pacienteactual != NULL){
                    k = pacienteactual->data->prioridad;
                    cambio = probabilidad(pacienteactual);
                    if((k + cambio >= 1) && (k + cambio <= 4)){
                        pacienteactual->data->prioridad += cambio;
                        Nodo* aux = pacienteactual;
                        printf("tick(%d) Paciente %s cambia de prioridad %d a %d\n", tick, aux->data->id, k, aux->data->prioridad);
                        registrarCambioPrioridad(pila, aux->data->id, k);
                        pacienteactual = pacienteactual->siguiente;
                        encolar(desencolar(cola[j], aux), cola[aux->data->prioridad - 1]);
                    }
                    else{
                        pacienteactual = pacienteactual->siguiente;
                    }
                }
            }
        }
        for(j = 0; j < medicos; j++){ /* Ciclo que pasa medico por medico confirmando si esta libre u ocupado */
            if(Medicos[j].pacienteactual == NULL){
                for(k = 0; k < 4; k++){
                    if(cola[k]->cabecera != NULL){
                        Nodo* atendiendo = desencolar(cola[k], cola[k]->cabecera); 
                        Medicos[j].pacienteactual = atendiendo;
                        Medicos[j].duracion = 0;
                        printf("tick(%d) Medico %d esta atendiendo a paciente %s con prioridad %d\n", tick, Medicos[j].id, atendiendo->data->id,atendiendo->data->prioridad);
                        break;
                    }
                }
            }
        }
        for(j = 0; j < medicos; j++){
            if(Medicos[j].pacienteactual != NULL){
                Medicos[j].pacienteactual->data->ticksestimados--;
                Medicos[j].duracion++;
                if(Medicos[j].pacienteactual->data->ticksestimados <= 0){
                    printf("tick(%d) El Medico %d ha terminado de atender al paciente %s\n", tick, Medicos[j].id, Medicos[j].pacienteactual->data->id);
                    Medicos[j].pacienteactual->data->ticksespera = tick - Medicos[j].pacienteactual->data->tickllegada - Medicos[j].duracion; /*Tiempo de espera real*/
                    insertarHistorial(&historial, Medicos[j].pacienteactual);
                    free(Medicos[j].pacienteactual);
                    Medicos[j].duracion = 0;
                    Medicos[j].pacienteactual = NULL;
                    pacienteslistos++;
                }
            }
        }
        if(pacienteslistos == pacientes){
            break;
        }
        tick++;
    }
    return 0;
}

void leerdatos(Cola* lista, int *medicos, int* pacientes){
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
        d->id = (char*)malloc(16 * sizeof(char));
        validarmalloc(d->id);
        int contador = fscanf(entrada, " %[^;];%d;%d;%d;%d;%f;%f\n", d->id, &d->edad, &d->prioridad, &d->tickllegada, &d->ticksestimados, &d->p_empeorar, &d->p_mejorar);
        if(contador != 7){
            free(d->id);
            free(d);
            free(p);
            break;
        }
        p->data = d;
        encolar(p, lista);
    }
    fclose(entrada);
} 

void validarmalloc(void* puntero){
    if (puntero == NULL){
        printf("Error al reservar memoria\n");
        exit(1);
    }
    else{
        return;
    }
}

int probabilidad(Nodo* paciente){
    float aleatorio = (float)rand()/(float)RAND_MAX;
    if (aleatorio >= 1.0 - paciente->data->p_mejorar){ /*Probabilidad de mejorar*/
        return 1;
    }
    else if (aleatorio <= paciente->data->p_empeorar){ /*Probabilidad de empeorar*/
        return -1;
    }
    return 0; 
}

void encolar(Nodo* paciente, Cola* cola){
    paciente->siguiente = NULL;
    if(cola->cabecera == NULL){
        cola->cabecera = paciente;
        cola->final = paciente;
        return;
    }
    cola->final->siguiente = paciente;
    cola->final = paciente;
}

Nodo* desencolar(Cola* cola, Nodo* paciente){
    Nodo* aux = cola->cabecera;
    Nodo* previo = cola->cabecera;
    if (cola->cabecera == NULL) return NULL;
    else if(aux == paciente){
        /*Por si el paciente se encuentra al tope de la cola, desencolar normalmente*/
        cola->cabecera = paciente->siguiente;
        if(cola->cabecera == NULL) cola->final = NULL;
        paciente->siguiente = NULL;
        return paciente;
    }
    else{
        /*Para desencolar en cualquier posicion*/
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

void insertarHistorial(Lista* l, Nodo* paciente) {
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

void registrarCambioPrioridad(Pila *p, char* idPaciente, int prioridadAnterior) {
    NodoPila *nuevo = (NodoPila *)malloc(sizeof(NodoPila));
    validarmalloc(nuevo);
    strcpy(nuevo->dato.idPaciente, idPaciente);
    nuevo->dato.prioridadAnterior = prioridadAnterior;
    nuevo->sig = p->tope;
    p->tope = nuevo;
}

CambioPrioridad revertirUltimoCambio(Pila *p) {
    CambioPrioridad cambio = {-1, -1, -1};
    if (p == NULL) {
        return cambio;
    }
    NodoPila *aux = p->tope;
    cambio = aux->dato;
    p->tope = aux->sig;
    free(aux);
    return cambio;
}

void mostrarHistorialCambios(Pila *p) {
    NodoPila *aux = p->tope;
    printf("\n===== HISTORIAL DE CAMBIOS =====\n");
    if (aux == NULL) {
        printf("No hay cambios registrados.\n");
        return;
    }
    while (aux != NULL) {
        printf("Paciente %d: %d -> %d\n",aux->dato.idPaciente, aux->dato.prioridadAnterior, aux->dato.prioridadNueva);
        aux = aux->sig;
    }
}

void liberarLista(Lista* l){
    if(l == NULL) return;

    NodoDoble* aux = l->cabecera;
    while(aux != NULL){
        NodoDoble* aux2 = aux;
        aux = aux->siguiente;
        if(aux2->data != NULL){
            if(aux2->data->id != NULL){
                free(aux2)
            }
        }
    }
}