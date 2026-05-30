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
    Datos paciente;
    struct nododoble* siguiente;
    struct nododoble* anterior;
} NodoDoble;

typedef struct nodo{
    Datos paciente;
    struct nodo* siguiente;
} Nodo;

void leerarchivo(Nodo**, int); /*Se lee el archivo y se agregan pacientes a la cola cada vez que el tiempo aumenta en 1 tick, para simular bien los tiempos de llegada*/
void apilar(Nodo**);
void encolar(Nodo**);
Nodo* desencolar(Nodo**); 

int main(){

    return 0;
}