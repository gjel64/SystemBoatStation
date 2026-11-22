/*
bus that's goal is to go to the train station, pick persons, and go back
*/
#include "bus.h"

int start_bus(struct Bus b) {

    printf("LOG-BUS: Bus started\n");

    // --- send info to serv
    printf("LOG-BUS: sending udp info to serv %s:%d\n", SERVER_IP, PORT_UDP);
    struct sockaddr_in addr_udp = {0};
    addr_udp.sin_family = AF_INET;
    if (inet_pton(AF_INET, SERVER_IP, &addr_udp.sin_addr) <= 0 ) {perror("err ip addr");}
    addr_udp.sin_port = htons(PORT_UDP);

    int sfd_udp = socket(AF_INET, SOCK_DGRAM, 0);
    if (sfd_udp < 0) {
        perror("socket creation failed");
    }

    while (b.t_from_station > 0.0) {    
        int id = b.id;
        if (sendto(sfd_udp, &id, sizeof(int), 0, 
            (struct sockaddr *)&addr_udp, sizeof(addr_udp)) < 0) {perror("error sending udp");}  
              
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
    if (sfd_tcp < 0) {
        perror("socket creation failed");
    }

    if (connect(sfd_tcp, (const struct sockaddr *) &addr_tcp, sizeof(addr_tcp)) < 0) {perror("err connecting tcp addr");}
    if (send(sfd_tcp, &b.id, sizeof(int), 0) < 0)  {perror("err sending tcp");}

    printf("LOG-BUS: wainting to receive the dock number\n");
    ssize_t s;
    char buffer[BUFFER_SIZE] = {0};
    do {
        s = recv(sfd_tcp, buffer, BUFFER_SIZE, 0);
    } while(s < 0);

    int dock_n = atoi(buffer);
    int time_to_dock = rand() % 5;

    // --- Go to dock
    printf("LOG-BUS: go to the dock %d\n", dock_n);
    while (time_to_dock > 0) {
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

    char msg = CODE_ASK_O;
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
