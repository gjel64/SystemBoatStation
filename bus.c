/*
bus that's goal is to go to the train station, pick persons, and go back
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

#define SERVER_IP "127.0.0.1"
#define PORT_UDP 6001
#define PORT_TCP 6000
#define BUFFER_SIZE 1024
#define N_PLACE_IN_BUS 30

#define CODE_ASK_E '1' // ask to enter
#define CODE_ASK_O '2' // ask to leave
#define CODE_ACK_O '3' // can leave
#define CODE_FIN_O '4' // can leave

struct Bus {
    float t_from_station; // time from station

    int type; /* 0, 1, 2 (V, A, P)*/
    int plate; /* plate number [0, 1000]*/
    int priority; /* higher = more priority */

    int n_pers;
    float predicted_station_time;
};


int main() {

    printf("LOG-BUS: Bus started\n");

    // --- Create random bus
    struct Bus b = {
        (rand() % 1000) / 100.0f, // time from station [0, 10]seconfs
        rand() % 3,
        rand() % 1000,
        rand() % 50,
        rand() % N_PLACE_IN_BUS,
        (rand() % 1000) / 100.0f
    };

    // --- send info to serv
    printf("LOG-BUS: sending udp info to serv\n");
    struct sockaddr_in addr_udp = {0};
    addr_udp.sin_family = AF_INET;
    if (inet_pton(AF_INET, SERVER_IP, &addr_udp.sin_addr) <= 0 ) {perror("err ip addr");}
    addr_udp.sin_port = htons(PORT_UDP);

    int sfd_udp = socket(AF_INET, SOCK_DGRAM, 0);
    while (b.t_from_station > 0.0) {    
        if (sendto(sfd_udp, &b, sizeof(b), 0, (struct sockaddr *)&addr_udp, sizeof(addr_udp)) < 0) {perror("error sending udp");}  
              
        // wait 1 sec
        sleep(1);
        b.t_from_station -= 1;
        if (b.t_from_station < 0.0) {b.t_from_station = 0.0;}
    }

    printf("LOG-BUS: arrived to docker\n");
    // --- Send that we just arrive and wait to receive response
    struct sockaddr_in addr_tcp = {0};
    addr_tcp.sin_family = AF_INET;
    if (inet_pton(AF_INET, SERVER_IP, &addr_tcp.sin_addr) <= 0 ) {perror("err ip addr");}
    addr_tcp.sin_port = htons(PORT_TCP);

    int sfd_tcp = socket(AF_INET, SOCK_STREAM, 0);

    if (connect(sfd_tcp, (const struct sockaddr *) &addr_tcp, sizeof(addr_tcp)) < 0) {perror("err connecting tcp addr");}
    char msg = CODE_ASK_E;
    if (send(sfd_tcp, &msg, sizeof(msg), 0) < 0)  {perror("err sending tcp");}

    printf("LOG-BUS: wainting to receive the dock number\n");
    ssize_t s;
    int buffer[BUFFER_SIZE] = {0};
    do {
        s = recv(sfd_tcp, buffer, BUFFER_SIZE, 0);
    } while(s < 0);

    int dock_n = buffer[0];
    int time_to_dock = rand() % 5;

    // --- Go to dock
    printf("LOG-BUS: go to the dock %d\n", dock_n);
    while (time_to_dock < 0) {
        sleep(1);
        time_to_dock -= 1;
    }

    // --- Depose persons and pick up somes
    printf("LOG-BUS: pick up and depose persons\n");
    b.n_pers -= rand() % b.n_pers;
    b.n_pers += rand() % (N_PLACE_IN_BUS - b.n_pers);
    sleep(1);

    // --- Ask to leave
    printf("LOG-BUS: ask to leave\n");

    msg = CODE_ASK_O;
    if (send(sfd_tcp, &msg, sizeof(msg), 0) < 0)  {perror("err sending tcp 2");}

    // --- Wait response
    printf("LOG-BUS: waiting response\n");
    s = -1;
    char bufferc[BUFFER_SIZE] = {0};
    do {
        s = recv(sfd_tcp, &bufferc, BUFFER_SIZE, 0);
    } while(s < 0 || bufferc[0] != CODE_ACK_O);

    // --- the bus has leaved
    msg = CODE_FIN_O;
    if (send(sfd_tcp, &msg, sizeof(msg), 0) < 0)  {perror("err sending tcp 2");}
    printf("LOG-BUS: Leaved, End\n");

    close(sfd_tcp);
    close(sfd_udp);
    return 0;
}
