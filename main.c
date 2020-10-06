#include <stdio.h>
#include <stdlib.h>
#include "simlib.h"

#define EVENT_ARRIVO_AEROPUERTO 1
#define EVENT_ARRIVO_HOTEL_1 2
#define EVENT_ARRIVO_HOTEL_2 3
#define EVENT_SALIDA_BUS_1_AEROPUERTO 4  //evento primera salida del aeropierto bus 1
#define EVENT_SALIDA_BUS_2_AEROPUERTO 5  //evento primera salida del aeropierto bus 2
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


#define SAMPST_DEMORAS_HOTEL2 1


#define STREAM_TRAYECTO 1

float promedio_trayecto, promedio_arrivo_aeropuerto, promedio_arrivo_hotel;
int numero_recorridos_totales, recorrido, maximo_sillas_bus,sillas_ocupadas_bus1, sillas_ocupadas_bus2,
    contbus1, contbus2, total_pasajeros_trasnportados;

//Contadores-acumuladores

FILE *infile, *outfile;

void leer_parametros();
void init_model();
void arrivoAeropuerto();
void arrivoHotel1();
void arrivoHotel2();
void inicio_simulacion();//Salida inicial del bus 1 del aeropuerto eventos 4 y 5
void salida_bus_aeropuerto(int bus);//Salida de los buses desde el aeropuerto para el segundo trayecto
                                    //Programa eventos 6 y 8
void salidaHotel1Bus(int bus);//Paradas de los buses en el hotel 1. Programa eventos 7 y 9
void salidaHotel2Bus(int bus);//Paradas de los buses en el hotel 2: Programa eventos 10 y 11
void informe(void);

float getPromedioTrayecto();


int main()
{
    leer_parametros();
    init_simlib();
    init_model();
    while(recorrido<numero_recorridos_totales){
        timing();
        switch(next_event_type){
            case EVENT_ARRIVO_AEROPUERTO:
               arrivoAeropuerto();
            break;

            case EVENT_ARRIVO_HOTEL_1:
                arrivoHotel1();
            break;

            case EVENT_ARRIVO_HOTEL_2:
                arrivoHotel2();
            break;

            case EVENT_SALIDA_BUS_1_AEROPUERTO:
                salida_bus_aeropuerto(BUS1);
            break;

            case EVENT_SALIDA_BUS_2_AEROPUERTO:
                salida_bus_aeropuerto(BUS2);
            break;

            case EVENT_PARADA_BUS1_HOTEL1:
                salidaHotel1Bus(BUS1);
            break;

            case EVENT_PARADA_BUS1_HOTEL2:
                salidaHotel2Bus(BUS1);
            break;

            case EVENT_PARADA_BUS2_HOTEL1:
                salidaHotel1Bus(BUS2);
            break;

            case EVENT_PARADA_BUS2_HOTEL2:
                salidaHotel2Bus(BUS2);
            break;

            case EVENT_PARADA_BUS1_AEROPUERTO:
                parada_bus_aeropuerto(BUS1);
            break;

            case EVENT_PARADA_BUS2_AEROPUERTO:
                parada_bus_aeropuerto(BUS2);
            break;
        }
    }
}

void parada_bus_aeropuerto(int bus){
    if(bus==BUS1){
        total_pasajeros_trasnportados+=contbus1;
        contbus1=0;
        while(list_size[QUEUE_AEROPUERTO]>0){
            list_remove(FIRST,QUEUE_AEROPUERTO);
            contbus1++;
        }
        if(contbus1>maximo_sillas_bus) maximo_sillas_bus=contbus1;
        event_schedule(sim_time+getPromedioTrayecto(),EVENT_PARADA_BUS1_HOTEL1);
    }else if(bus==BUS2){
        total_pasajeros_trasnportados+=contbus2;
        contbus2=0;
        while(list_size[QUEUE_AEROPUERTO]>0){
            list_remove(FIRST,QUEUE_AEROPUERTO);
            contbus2++;
        }
        if(contbus2>maximo_sillas_bus) maximo_sillas_bus=contbus2;
        event_schedule(sim_time+getPromedioTrayecto(),EVENT_PARADA_BUS2_HOTEL1);
        recorrido++;
    }else{
        exit(1);
    }
}


 void salidaHotel1Bus(int bus){
 }

void salidaHotel2Bus(int bus){
    if(bus==BUS1){
        if(contbus1%2==0){
            contbus1-=contbus1/2;
        }else{
            contbus1-=(contbus1+1)/2;
        }
        event_schedule(sim_time+getPromedioTrayecto(),EVENT_PARADA_BUS1_AEROPUERTO);
        while(list_size[QUEUE_HOTEL2]>0){
            contbus1++;
            list_remove(FIRST,QUEUE_HOTEL2);
            sampst(sim_time-transfer[1],SAMPST_DEMORAS_HOTEL2);
        }
        if(contbus1>maximo_sillas_bus) maximo_sillas_bus=contbus1;
    }else if(BUS2){
        if(contbus2%2==0){
            contbus2-=contbus2/2;
        }else{
            contbus2-=(contbus2+1)/2;
        }
        event_schedule(sim_time+getPromedioTrayecto(),EVENT_PARADA_BUS2_AEROPUERTO);
        while(list_size[QUEUE_HOTEL2]>0){
            contbus2++;
            list_remove(FIRST,QUEUE_HOTEL2);
            sampst(sim_time-transfer[1],SAMPST_DEMORAS_HOTEL2);
        }
        if(contbus2>maximo_sillas_bus) maximo_sillas_bus=contbus2;
    }else{
        exit(1);
    }
}

void salida_bus_aeropuerto(int bus){
}






void init_model(){
    recorrido=0;
    total_pasajeros_trasnportados=0;
    maximo_sillas_bus=-1;
    contbus1=0;
    contbus2=0;
}

float getPromedioTrayecto(){
    return uniform(promedio_trayecto,2,STREAM_TRAYECTO);
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



//Arrivos de personas
void arrivoHotel1(){
}

void arrivoHotel2(){

}


void arrivoAeropuerto(){
}
