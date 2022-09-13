#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <string.h>

#define BUFFER_SIZE 10
#define CONSUMERS 4
#define TIME_INTERVAL 1

sem_t semEmpty;
sem_t semFull;

pthread_mutex_t mutexBuffer;

int buffer[BUFFER_SIZE]; 
int count = 0;

void* producer(void* args) {
    while(1) {
        // Produce
        int x = rand() % 100;
        sleep(TIME_INTERVAL);

        // Add to the buffer
        sem_wait(&semEmpty);
        pthread_mutex_lock(&mutexBuffer);
        buffer[count] = x;
        count++;
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semFull);
    }
}

void* consumer(void* args) {
    while(1) {
        // Remove from buffer
        sem_wait(&semFull);
        pthread_mutex_lock(&mutexBuffer);
        int y = buffer[count - 1];
        count--;
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semEmpty);

        // Consume
        printf("Got %d\n", y);
    }
}

int main() {
    srand(time(NULL));
    pthread_t th[CONSUMERS + 1];
    pthread_mutex_init(&mutexBuffer, NULL);
    sem_init(&semEmpty, 0, BUFFER_SIZE);
    sem_init(&semFull, 0, 0);

    int i;
    for(i = 0; i < CONSUMERS + 1; i++) {
        if(i == 0) {
            if(pthread_create(&th[i], NULL, &producer, NULL) != 0) {
                perror("Failed to create producer thread");
            }
        } else {
            if(pthread_create(&th[i], NULL, &consumer, NULL) != 0) {
                perror("Failed to create consumer thread");
            }
        }
    }

    for(i = 0; i < CONSUMERS + 1; i++) {
        if(pthread_join(th[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }

    sem_destroy(&semEmpty);
    sem_destroy(&semFull);
    pthread_mutex_destroy(&mutexBuffer);

    return 0;
}