#include<stdio.h>
#include<stdlib.h>
#include<time.h>

int main(){
    int medicos, pacientes, edad, prioridad, tickllegada, tiempoestimado;
    float prob_mejorar, prob_empeorar;
    srand(time(NULL));
    medicos = 2 + (rand()%(7));
    pacientes = 10 + (rand()%(21));
    FILE* archivo = fopen("Entrada.txt", "w");
    fprintf(archivo, "%d\n%d\n", medicos, pacientes);
    for(int i = 0; i < pacientes; i++){
        edad = 1 + (rand()%70);
        prioridad = 1 + (rand()%4);
        tickllegada = (rand()%15);
        tiempoestimado = 2 + (rand()%9);
        prob_empeorar = 0.10 + (float)rand()/RAND_MAX * 0.20;
        prob_mejorar = 0.02 + (float)rand()/RAND_MAX * 0.08;
        fprintf(archivo, "P%03d;%d;%d;%d;%d;%.2f;%.2f\n", i+1, edad, prioridad, tickllegada, tiempoestimado, prob_empeorar, prob_mejorar);
    }
    fclose(archivo);
    return 0;
}