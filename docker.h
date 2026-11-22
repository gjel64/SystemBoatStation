#ifndef DOCKER_H
#define DOCKER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h> 
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/wait.h>

#include "bus.h"

#define SHM_NAME "/bus_shm"

#define MAX_DOCKER 10
#define MAX_BUS MAX_DOCKER*3*2 

struct Register {
    pthread_mutex_t mutex;

    int n_bus; // how many bus are in the register
    struct Bus buses[MAX_BUS];

    int n_dockers[3]; /* [A, P, V]  */ // how many dockers for each type
    int dockers[3][MAX_DOCKER];        // bus idx for every docker

    int next_id; // next id to assign to a bus

};

pid_t new_bus(struct Register *shm_ptr);

#endif /* DOCKER_H */
