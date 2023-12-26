#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_SIZE 4
#define BUFFER_SIZE 10

typedef struct
{
    int buffer[BUFFER_SIZE];
    int in;
    int out;
} CircularBuffer;

int main()
{
    int shmid;
    key_t key = 1234;

    if ((shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666)) < 0)
    {
        perror("shmget");
        exit(1);
    }

    CircularBuffer *sharedBuffer = (CircularBuffer *)shmat(shmid, NULL, 0);
    if ((int)sharedBuffer == -1)
    {
        perror("shmat");
        exit(1);
    }

    sharedBuffer->in = 0;
    sharedBuffer->out = 0;

    while (1)
    {
        if ((sharedBuffer->in + 1) % BUFFER_SIZE != sharedBuffer->out)
        {
            int item = rand() % 100; // Generate a random item for the buffer
            sharedBuffer->buffer[sharedBuffer->in] = item;
            printf("Produced: %d\n", item);
            sharedBuffer->in = (sharedBuffer->in + 1) % BUFFER_SIZE;
        }
        sleep(1); // Adjust the producer speed
    }

    shmdt(sharedBuffer);
    return 0;
}