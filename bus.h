
#define SERVER_IP "127.0.0.1"
#define PORT_UDP 6001
#define PORT_TCP 6000
#define BUFFER_SIZE 1024
#define N_PLACE_IN_BUS 30

#define CODE_ASK_E '1' // ask to enter
#define CODE_ASK_O '2' // ask to leave
#define CODE_ACK_O '3' // can leave
#define CODE_FIN_O '4' // can leave

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


struct Bus {
    int id;
    float t_from_station; // time from station

    int type; /* 0, 1, 2 (V, A, P)*/
    int plate; /* plate number [0, 1000]*/
    int priority; /* higher = more priority */

    int n_pers;
    float predicted_station_time;
};


int start_bus(struct Bus b);