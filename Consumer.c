#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_SIZE 4
#define BUFFER_SIZE 10

typedef struct {
    int buffer[BUFFER_SIZE];
    int in;
    int out;
} CircularBuffer;

int main() {
    int shmid;
    key_t key = 1234;

    if ((shmid = shmget(key, SHM_SIZE, 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    CircularBuffer *sharedBuffer = (CircularBuffer *)shmat(shmid, NULL, 0);
    if ((int)sharedBuffer == -1) {
        perror("shmat");
        exit(1);
    }

    while (1) {
        if (sharedBuffer->out != sharedBuffer->in) {
            printf("Consumed: %d\n", sharedBuffer->buffer[sharedBuffer->out]);
            sharedBuffer->out = (sharedBuffer->out + 1) % BUFFER_SIZE;
        }
        sleep(2); // Adjust the consumer speed
    }

    shmdt(sharedBuffer);
    shmctl(shmid, IPC_RMID, NULL); // Clean up shared memory
    return 0;
}
