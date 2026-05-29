#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct datos{
    char* numeropaciente;
    int edad, prioridad, tickllegada, ticksestimados;
    float p_empeorar, p_mejorar;
} Datos;

typedef struct nododoble{
    Datos paciente;
    struct nododoble* siguiente;
    struct nododoble* anterior;
} NodoDoble;

typedef struct nodo{
    Datos paciente;
    struct nodo* siguiente;
} Nodo;

int main(){

    return 0;
}