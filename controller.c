#include "docker.h"
#include "bus.h"


void print_bus(struct Bus b) {
    printf("Bus %d, time_from_station: %.2f, type: %d, plate: %d, priority: %d, n_pers: %d, predicted_station_time: %.2f\n", b.id, b.t_from_station, b.type, b.plate, b.priority, b.n_pers, b.predicted_station_time);
}




int main() {
    int sh_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    if (sh_fd == -1) { perror("err shm_open"); }

    struct Register *shm_ptr = mmap(0, sizeof(struct Register), PROT_READ , MAP_SHARED, sh_fd, 0);
    if (shm_ptr == MAP_FAILED) { perror("err mmap"); }

    while(1) {
        printf("\033[2J\033[H");
        printf("%d buses\n", shm_ptr->n_bus);
        if (shm_ptr->n_bus == 0) {
            printf("No bus in the register\n");
        }
        else {
            for (int i=0; i<shm_ptr->n_bus; i++) {
                struct Bus b = shm_ptr->buses[i];
                print_bus(b);
            }
        }
        sleep(1);
    }
    return 0;
}