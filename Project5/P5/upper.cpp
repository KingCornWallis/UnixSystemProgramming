#include <algorithm>
#include <string>
#include <iostream>

// Semaphore libraries
#include <signal.h>
#include <semaphore.h>

// Standerd Libraries
#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>

#include <math.h>
#include "message.h"

using namespace std;

static void *shared_memory;
static int shm_fd;
static sem_t *sem_p1;
static sem_t *sem_p2;

 
int toint(char str[])
{
    int len = strlen(str);
    int i, num = 0;
 
    for (i = 0; i < len; i++)
    {
        num = num + ((str[len - (i + 1)] - '0') * pow(10, i));
    }
    return num;
}
void to_upper_string(char * str)
{
    locale loc;
    for (size_t i=0; i <strlen(str); ++i)
        str[i] = toupper(str[i],loc);
    str[255] = '\0';
}

int main(int args, char** argv)
{
    char reversed_message[256];
    int master_pid= atoi(argv[4]);
    
    // create shared memory space
    shared_memory = (void *)0;
    struct shared_use_st *shared_stuff;

    // Open a shared memory object
    shm_fd = shm_open(argv[2], O_RDWR, 0);
    if (shm_fd == -1) {
        perror("Failed sharing memory");
        exit(1);
    }

    // Map the object into our address space.
    shared_memory = mmap(NULL, sizeof(struct shared_use_st),
                    PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shared_memory == MAP_FAILED) {
        perror("Failed mapping memory");
        exit(1);
    }
    close(shm_fd);

    shared_stuff = (struct shared_use_st *)shared_memory;

    // open the semaphore
    sem_p1 = sem_open(argv[1], O_CREAT , 0600, 1);
    sem_p2 = sem_open(argv[3], O_CREAT , 0600, 1);
    while (1)
    {
        sem_wait(sem_p1);

        // get string from shared memory
        strncpy(reversed_message, shared_stuff->data, TEXT_SIZE);

        to_upper_string(reversed_message);
        if (reversed_message[0] == '\0')
            cout<< "^D";

        cout << reversed_message << endl;

        // write to shared memory
        strncpy(shared_stuff->data, reversed_message, TEXT_SIZE);

        // synchronize
        shared_stuff->written_by_you = 0;

        // release the semaphore
        sem_post(sem_p2);

        kill(master_pid, SIGUSR1);
        if (reversed_message[0] == '\0')
            break;
    }
    return 0;
}
