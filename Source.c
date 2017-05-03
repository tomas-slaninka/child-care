#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <assert.h>

typedef struct{
        sem_t mutex;
        sem_t multiplex, x, x2, x3;
        pthread_mutex_t m;
        int id, ch;
}DATA;

void * child( void * inp ){
        DATA * data = (DATA*) inp;
        int id = data->id;
        fprintf(stderr, "Dieta '%d'\n", id);
        pthread_mutex_unlock(&data->m);
        while(1){
                sem_wait(&data->multiplex);
                        sem_wait(&data->x2);
                        data->ch++;
                        fprintf(stderr, "Dieta '%d' sa hra, pocet deti [%d]\n", id, data->ch);
                        sem_post(&data->x2);
                        sleep(2);
                        sem_wait(&data->x2);
                        data->ch--;
                        sem_post(&data->x2);
                sem_post(&data->multiplex);
                sleep(1);
        }
        return NULL;
}

void * adult( void * inp ){
        DATA * data = (DATA*) inp;
        int id = data->id;
        fprintf(stderr, "Dospely '%d'\n", id);
        pthread_mutex_unlock(&data->m);
        while( 1){
//              sem_wait(&data->x3);
                sem_post(&data->multiplex);
                sem_post(&data->multiplex);
                sem_post(&data->multiplex);
                        sem_wait(&data->x);
                        fprintf(stderr, "Dozor '%d', pocet deti [%d]\n", id, data->ch);
                        sleep(1);
                        sem_post(&data->x);
                sem_wait(&data->mutex);
                        sem_wait(&data->multiplex);
                        sem_wait(&data->multiplex);
                        sem_wait(&data->multiplex);
                sem_post(&data->mutex);
//              sem_post(&data->x3);
        }
        return NULL;
}

int main(int argc, char *argv[]){
        printf("\nZaciatok\n");
        DATA * data = (DATA*) malloc(sizeof(DATA));
        data->ch = 0;
        sem_init(&data->mutex, 0, 1);
        sem_init(&data->x, 0, 1);
        sem_init(&data->x2, 0, 1);
        sem_init(&data->x3, 0, 1);
        sem_init(&data->multiplex, 0, 0);
        pthread_mutex_init(&data->m, 0);

        int r = 0;
        int d = 0;
        pthread_t tidr[2];
        pthread_t tidd[5];

        for(; r < 2; r++){
                pthread_mutex_lock(&data->m);
                data->id = r;
                pthread_create(&tidr[r], NULL, adult, (void*) data);
        }

        for(; d < 5; d++){
                pthread_mutex_lock(&data->m);
                data->id = d + 2;
                pthread_create(&tidd[d], NULL, child, (void*) data);
        }
        r = 0;
        d = 0;
        for(; r < 2; r++){
                pthread_join(tidr[r],0);
        }
        for(; d < 5; d++){
                pthread_join(tidd[d],0);
        }
        printf("\n-----Koniec Hlavneho Vlakna-----\n");
        return EXIT_SUCCESS;
}
