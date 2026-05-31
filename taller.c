#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct datos{
    char* identificador;
    int edad, prioridad, tickllegada, ticksestimados;
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

typedef struct col{
    Nodo* cabecera;
    Nodo* final;
} Cola;

void validarmalloc(void*);
void leerpacientes(FILE*, int, Nodo**); /*Se lee el archivo y se agregan pacientes a la cola cada vez que el tiempo aumenta en 1 tick, para simular bien los tiempos de llegada*/
void apilar(Nodo**);
void encolar(Nodo*, Cola*);
Nodo* desencolar(Cola*, Nodo*); 

int main(){
    int medicos, pacientes, tick = 0;
    Cola* cola[4];
    for (int i = 0; i < 4; i++){
        cola[i] = (Cola*)malloc(sizeof(Cola));
        validarmalloc(cola[i]);
        cola[i]->cabecera = NULL;
        cola[i]->final = NULL;
    }
    FILE *entrada;
    entrada = fopen ("Entrada.txt", "r");
    fscanf (entrada, "%d\n%d\n", &medicos, &pacientes);
    printf ("%d %d", medicos, pacientes);
    fclose (entrada);
    return 0;
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
        cola->cabecera = paciente->siguiente;
        if(cola->cabecera == NULL) cola->final = NULL;
        paciente->siguiente = NULL;
        return paciente;
    }
    else{
        aux = aux->siguiente;
        while(aux != paciente){
            previo = aux;
            aux = aux->siguiente;
        }
        previo->siguiente = aux->siguiente;
        aux->siguiente = NULL;
        return aux;
    }   
}