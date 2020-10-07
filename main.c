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


#define SAMPST_DEMORAS_HOTEL2 2
#define SAMPST_DEMORAS_HOTEL1 1
#define SAMPST_DEMORAS_AEROPUERTO 3


#define STREAM_TRAYECTO 1
#define STREAM_HOTEL1 2
#define STREAM_HOTEL2 3
#define STREAM_AEROPUERTO 4


float promedio_trayecto, promedio_arrivo_aeropuerto, promedio_arrivo_hotel;
int numero_recorridos_totales, recorrido, maximo_sillas_bus,sillas_ocupadas_bus1, sillas_ocupadas_bus2,
    contbus1, contbus2, pasajeros_aeropuerto_B1, pasajeros_aeropuerto_B2, pasajeros_bajada_H2_B1,
        pasajeros_bajada_H2_B2, total_pasajeros_trasnportados;

//Contadores-acumuladores

FILE *infile, *outfile;

void leer_parametros();
void init_model();
void arrivoAeropuerto();
void arrivoHotel1();
void arrivoHotel2();
void inicio_simulacion();//Salida inicial del bus 1 del aeropuerto eventos 4 y 5
void salida_bus_aeropuerto(int bus);//Las primera vez que sale cada bus del aeropuerto(No descarga gente)
void parada_bus_aeropuerto(int bus);//Salida de los buses desde el aeropuerto para el segundo trayecto Programa eventos 6 y 8
void salidaHotel1Bus(int bus);//Paradas de los buses en el hotel 1. Programa eventos 7 y 9
void salidaHotel2Bus(int bus);//Paradas de los buses en el hotel 2: Programa eventos 10 y 11
void informe(void);
void report();
float getPromedioTrayecto();


int main()
{
    leer_parametros();
    init_simlib();
    init_model();
    while(recorrido<numero_recorridos_totales){
        timing();
        printf("*%f\n",sim_time);
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
/*
            case EVENT_SALIDA_BUS_1_AEROPUERTO:
                salida_bus_aeropuerto(BUS1);
            break;

            case EVENT_SALIDA_BUS_2_AEROPUERTO:
                salida_bus_aeropuerto(BUS2);
            break;
*/
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
    report();
}

void parada_bus_aeropuerto(int bus){
    if(bus==BUS1){
        total_pasajeros_trasnportados+=contbus1;
        pasajeros_aeropuerto_B1=list_size[QUEUE_AEROPUERTO];
        contbus1=pasajeros_aeropuerto_B1;
        while(list_size[QUEUE_AEROPUERTO]>0){
            list_remove(FIRST,QUEUE_AEROPUERTO);
            sampst(sim_time-transfer[1],SAMPST_DEMORAS_AEROPUERTO);
        }
        if(contbus1>maximo_sillas_bus) maximo_sillas_bus=contbus1;
        event_schedule(sim_time+getPromedioTrayecto(),EVENT_PARADA_BUS1_HOTEL1);
    }else if(bus==BUS2){
        total_pasajeros_trasnportados+=contbus2;
        pasajeros_aeropuerto_B2=list_size[QUEUE_AEROPUERTO];
        contbus2=pasajeros_aeropuerto_B2;
        while(list_size[QUEUE_AEROPUERTO]>0){
            list_remove(FIRST,QUEUE_AEROPUERTO);
            sampst(sim_time-transfer[1],SAMPST_DEMORAS_AEROPUERTO);
        }
        if(contbus2>maximo_sillas_bus) maximo_sillas_bus=contbus2;
        event_schedule(sim_time+getPromedioTrayecto(),EVENT_PARADA_BUS2_HOTEL1);
        recorrido++;
    }else{
        exit(1);
    }
}


 void salidaHotel1Bus(int bus){
     int mitad=0;
     if(bus==BUS1){
        if(contbus1==1){
            mitad=0;
            pasajeros_bajada_H2_B1=0;
        }else if(pasajeros_aeropuerto_B1%2==0){
            mitad=pasajeros_aeropuerto_B1/2;
            pasajeros_bajada_H2_B1=mitad;
        }else{
            mitad-=(pasajeros_aeropuerto_B1+1)/2;
            pasajeros_bajada_H2_B1=(pasajeros_aeropuerto_B1-1)/2;
        }
        contbus1-=mitad;
        event_schedule(sim_time+getPromedioTrayecto(),EVENT_PARADA_BUS1_HOTEL2);
        while(list_size[QUEUE_HOTEL1]>0){
            contbus1++;
            list_remove(FIRST,QUEUE_HOTEL1);
            sampst(sim_time-transfer[1],SAMPST_DEMORAS_HOTEL1);
        }
        if(contbus1>maximo_sillas_bus) maximo_sillas_bus=contbus1;
    }else if(BUS2){
        if(contbus2==1){
            mitad=0;
            pasajeros_bajada_H2_B2=0;
        }else if(contbus2%2==0){
            mitad=pasajeros_aeropuerto_B2/2;
            pasajeros_bajada_H2_B2=mitad;
        }else{
            mitad-=(pasajeros_aeropuerto_B2+1)/2;
            pasajeros_bajada_H2_B2=(pasajeros_aeropuerto_B2-1)/2;
        }
        contbus2-=mitad;
        event_schedule(sim_time+getPromedioTrayecto(),EVENT_PARADA_BUS2_HOTEL2);
        while(list_size[QUEUE_HOTEL1]>0){
            contbus2++;
            list_remove(FIRST,QUEUE_HOTEL1);
            sampst(sim_time-transfer[1],SAMPST_DEMORAS_HOTEL1);
        }
        if(contbus2>maximo_sillas_bus) maximo_sillas_bus=contbus2;
    }else{
        exit(1);
    }
 }

void salidaHotel2Bus(int bus){
    if(bus==BUS1){
        contbus1-=pasajeros_bajada_H2_B1;
        event_schedule(sim_time+getPromedioTrayecto(),EVENT_PARADA_BUS1_AEROPUERTO);
        while(list_size[QUEUE_HOTEL2]>0){
            contbus1++;
            list_remove(FIRST,QUEUE_HOTEL2);
            sampst(sim_time-transfer[1],SAMPST_DEMORAS_HOTEL2);
        }
        if(contbus1>maximo_sillas_bus) maximo_sillas_bus=contbus1;
    }else if(BUS2){
        contbus2-=pasajeros_bajada_H2_B2;
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
    if(bus==BUS1){

    }else if(bus==BUS2){

    }else{
        exit(0);
    }
}



//Arrivos de personas
void arrivoHotel1(){
    transfer[1]=sim_time;
    list_file(FIRST,QUEUE_HOTEL1);
    event_schedule(sim_time+expon(promedio_arrivo_hotel,STREAM_HOTEL1),EVENT_ARRIVO_HOTEL_1);
}

void arrivoHotel2(){
    transfer[1]=sim_time;
    list_file(FIRST,QUEUE_HOTEL2);
    event_schedule(sim_time+expon(promedio_arrivo_hotel,STREAM_HOTEL2),EVENT_ARRIVO_HOTEL_2);

}


void arrivoAeropuerto(){
    transfer[1]=sim_time;
    list_file(FIRST,QUEUE_AEROPUERTO);
    event_schedule(sim_time+expon(promedio_arrivo_aeropuerto,STREAM_AEROPUERTO),EVENT_ARRIVO_AEROPUERTO);
}

void report(void) /* Report generator function. */
{
/* Get and write out estimates of desired measures of performance. */
fprintf(outfile,"\nTiempo total de simulación: %f horas\n",sim_time/60.0);
fprintf(outfile,"Número de sillas necesarias: %d\n", maximo_sillas_bus);
out_sampst(outfile, SAMPST_DEMORAS_HOTEL1,SAMPST_DEMORAS_HOTEL1);
out_sampst(outfile, SAMPST_DEMORAS_HOTEL2,SAMPST_DEMORAS_HOTEL2);
out_sampst(outfile, SAMPST_DEMORAS_AEROPUERTO,SAMPST_DEMORAS_AEROPUERTO);
/*
fprintf(outfile, "\nQueue length (1) and server utilization (2):\n");
out_filest(outfile, LIST_QUEUE, LIST_SERVER);*/
//fprintf(outfile, "\nTime simulation ended:%12.3f minutes\n", sim_time);
}

void init_model(){
    recorrido=0;
    total_pasajeros_trasnportados=0;
    maximo_sillas_bus=-1;
    contbus1=0;
    contbus2=0;
    pasajeros_aeropuerto_B1=0;
    pasajeros_aeropuerto_B2=0;

    event_schedule(expon(promedio_arrivo_hotel,STREAM_HOTEL1),EVENT_ARRIVO_HOTEL_1);
    event_schedule(expon(promedio_arrivo_hotel,STREAM_HOTEL2),EVENT_ARRIVO_HOTEL_2);
    event_schedule(expon(promedio_arrivo_aeropuerto,STREAM_AEROPUERTO),EVENT_ARRIVO_AEROPUERTO);

    event_schedule(0+getPromedioTrayecto(),EVENT_PARADA_BUS1_HOTEL1);
    event_schedule(0+getPromedioTrayecto()+30,EVENT_PARADA_BUS2_HOTEL1);
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

