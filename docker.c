/*
goal :
    - start a bus
    - start regulator
    - keep a register of bus (shared memory)
*/
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

#include "bus.h"

#define SHM_NAME "/bus_shm"

#define MAX_DOCKER 10
#define MAX_BUS MAX_DOCKER*3*2 

struct Register {
    pthread_mutex_t mutex;

    struct Bus buses[MAX_BUS];

    int n_dockers[3]; /* [A, P, V]  */ // how many dockers for each type
    int dockers[3][MAX_DOCKER];        // bus idx for every docker

    int next_id; // next id to assign to a bus

};

pid_t new_bus(struct Register *shm_ptr) {
    // --- Create random bus
    pthread_mutex_lock(&shm_ptr->mutex);
    shm_ptr->next_id++;
    int id = shm_ptr->next_id;
    pthread_mutex_unlock(&shm_ptr->mutex);
    struct Bus b = {
        id,
        (rand() % 1000) / 100.0f, // time from station [0, 10]seconfs
        rand() % 3,
        rand() % 1000,
        rand() % 50,
        rand() % N_PLACE_IN_BUS,
        (rand() % 1000) / 100.0f
    };

    pid_t pid = fork();
    if (pid < 0) { 
        perror("fork"); 
        exit(EXIT_FAILURE); 
    }
    if (pid == 0) { 
        start_bus(b);
        exit(EXIT_SUCCESS);
    }
    return pid;
}

int main() {
    int n_pid = 0;
    pid_t *pids = malloc(n_pid * sizeof(pid_t));

    // --- Create register
    printf("LOG-MAIN: creating register\n");
    struct Register *reg = malloc(sizeof(struct Register));
    reg->n_dockers[0] = 8;
    reg->n_dockers[1] = 7;
    reg->n_dockers[2] = 10;
    reg->next_id = 1;
    // init mutex
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);

    pthread_mutex_init(&reg->mutex, &attr);
    pthread_mutexattr_destroy(&attr);

    // --- Create shared memory
    int sh_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (sh_fd == -1) { perror("err shm_open"); }

    ftruncate(sh_fd, sizeof(struct Register));

    struct Register *shm_ptr = mmap(0, sizeof(struct Register), PROT_READ | PROT_WRITE, MAP_SHARED, sh_fd, 0);
    if (shm_ptr == MAP_FAILED) { perror("err mmap"); }
    pthread_mutex_lock(&shm_ptr->mutex);
    memcpy(shm_ptr, reg, sizeof(struct Register));
    pthread_mutex_unlock(&shm_ptr->mutex);

    free(reg);
    // --- Create regulator


    // --- Launch bus
    printf("LOG-MAIN: Launch bus by typing a char and qui with 'q'\n");
    while (1){
        char c;
        if (scanf(" %c", &c) == 1) { // space not to loop 2 times
            // quit if q
            if (c == 'q') {
                break;
            }
            printf("LOG-MAIN: Launching bus\n");
            // start the bus
            pid_t p = new_bus(shm_ptr);
            n_pid += 1;
            pids = realloc(pids, n_pid * sizeof(pid_t));
            pids[n_pid - 1] = p;
        }
    }

    // --- Destroy the shared_memory
    printf("LOG-MAIN: Cleaning\n");
    pthread_mutex_destroy(&shm_ptr->mutex);
    if (munmap(shm_ptr, sizeof(struct Register)) == -1) { perror("err munmap"); }
    if (shm_unlink(SHM_NAME) == -1) { perror("err shm_unlink"); }
    close(sh_fd);
    // --- Wait for childs to finish
    for (size_t i = 0; i < n_pid; i++) {
        waitpid(pids[i], NULL, 0);
    }
    free(pids);
    return 0;
}