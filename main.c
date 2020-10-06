#include <stdio.h>
#include <stdlib.h>
#include "simlib.h"

#define EVENT_ARRIVO_AEROPUERTO 1
#define EVENT_ARRIVO_HOTEL_1 2
#define EVENT_ARRIVO_HOTEL_2 3
#define EVENT_SALIDA_BUS_1_AEROPUERTO 4
#define EVENT_SALIDA_BUS_2_AEROPUERTO 5
#define EVENT_PARADA_BUS1_HOTEL1 6
#define EVENT_PARADA_BUS1_HOTEL2 7
#define EVENT_PARADA_BUS2_HOTEL1 8
#define EVENT_PARADA_BUS2_HOTEL2 9
#define EVENT_PARADA_BUS1_AEROPUERTO 10
#define EVENT_PARADA_BUS2_AEROPUERTO 11

#define QUEUE_AEROPUERTO 1
#define QUEUE_HOTEL1 2
#define QUEUE_HOTEL2 3
#define BUS1 4
#define BUS2 5

float promedio_trayecto, promedio_arrivo_aeropuerto, promedio_arrivo_hotel;
int numero_recorridos_totales, recorrido, maximo_sillas_bus,sillas_ocupadas_bus1, sillas_ocupadas_bus2;

//Contadores-acumuladores

FILE *infile, *outfile;

void leer_parametros();
void init_model();
void arrivoAeropuerto();
void arrivoHotel1();
void arrivoHotel2();
void inicio_simulacion();//Salida inicial del bus 1 del aeropuerto
void salida_bus_aeropuerto(int bus);//Salida de los buses desde el aeropuerto para el segundo trayecto
                                    //Programa eventos 6 y 8
void salidaHotel1Bus(int bus);//Paradas de los buses en el hotel 1. Programa eventos 7 y 9
void salidaHotel2Bus(int bus);//Paradas de los buses en el hotel 2: Programa eventos 10 y 11
void informe(void);




int main()
{
    leer_parametros();
    init_simlib();
    init_model();
    timing();
    while(recorrido<numero_recorridos_totales){
        switch(next_event_type){
            case EVENT_ARRIVO_AEROPUERTO:
               arrivoAeropuerto();
            break;
            case EVENT_ARRIVO_HOTEL_1:
            break;
            case EVENT_ARRIVO_HOTEL_2:
            break;
            case EVENT_SALIDA_BUS_1_AEROPUERTO:
            break;
            case EVENT_SALIDA_BUS_2_AEROPUERTO:
            break;
            case EVENT_PARADA_BUS1_HOTEL1:
            break;
            case EVENT_PARADA_BUS1_HOTEL2:
            break;
            case EVENT_PARADA_BUS2_HOTEL1:
            break;
            case EVENT_PARADA_BUS2_HOTEL2:
            break;
            case EVENT_PARADA_BUS1_AEROPUERTO:
            break;
            case EVENT_PARADA_BUS2_AEROPUERTO:
            break;
        }
    }
}

void init_model(){
    maximo_sillas_bus=-1;
}

void arrivoAeropuerto(){
}

void leer_parametros(){
    //Lee parametros de entrada
    infile = fopen("punto1.2.in", "r");
    outfile = fopen("punto1.2.out", "w");

    fscanf(infile, "%f %f %f %d", &promedio_trayecto, &promedio_arrivo_aeropuerto,
           &promedio_arrivo_hotel, &numero_recorridos_totales);

    //fprintf(outfile, "Single-server queueing system using simlib\n\n");
    fprintf(outfile, "Promedio trayecto: %f\n", promedio_trayecto);
    fprintf(outfile, "Promedio arrivo al aeropuerto: %f\n", promedio_arrivo_aeropuerto);
    fprintf(outfile, "Promedio arrivo al hotel: %f\n", promedio_arrivo_hotel);
    fprintf(outfile, "Recorridos a simular: %d\n", numero_recorridos_totales);
}
