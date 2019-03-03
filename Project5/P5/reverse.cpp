// Message Queue Libraries
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "message.h"

// Semaphore libraries
#include <signal.h>
#include <semaphore.h>
#include <sys/wait.h>

// Standerd Libraries
#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

static mqd_t mq_fd; 
static void *shared_memory;
static int shm_fd;
static sem_t *sem_p1;
static sem_t *sem_p2;

void clean_up(const char * mq_name, const char * shm_name, const char * sem1_name, const char * sem2_name);
void clean_up_mq(const char * mq_name) ;
void clean_up_shm(const char * shm_name);

void reverse_string(char* str) 
{ 
    int n = strlen(str); 
    for (int i = 0; i < n / 2; i++) 
        swap(str[i], str[n - i - 1]);
    str[n] = '\0'; 
} 

int main(int args, char** argv)
{
    char received_msg[256];
    struct mq_attr attr;
    attr.mq_flags   = 0;
    attr.mq_maxmsg = 1;
    attr.mq_msgsize = 256;
    mq_fd = mq_open(argv[3], O_RDONLY | O_CREAT, 0666, &attr);
    
    // create shared memory space
    shared_memory = (void *)0;
    struct shared_use_st *shared_stuff;

    // Open a shared memory object
    shm_fd = shm_open(argv[2], O_RDWR, 0);
    if (shm_fd == -1)
    {
        perror("Failed sharing memory");
        exit(1);
    }

    // Map the object into our address space.
    shared_memory = mmap(NULL, sizeof(struct shared_use_st),
                    PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if(shared_memory == MAP_FAILED)
    {
        perror("Failed mapping memory");
        exit(1);
    }
    close(shm_fd);
    shared_stuff = (struct shared_use_st *)shared_memory;
    
    // open the semaphore
    sem_p1 = sem_open(argv[1], O_CREAT , 0600, 1);    
    sem_p2 = sem_open(argv[4], O_CREAT , 0600, 1);    

    while (1) 
    {
        mq_receive(mq_fd,received_msg,sizeof(received_msg),NULL);
        reverse_string(received_msg);

        sem_wait(sem_p2);

        // write to shared memory object
        strncpy(shared_stuff->data, received_msg, TEXT_SIZE);

        // synchronize
        shared_stuff->written_by_you = 1;

        // release the semaphore
        sem_post(sem_p1);

        if (received_msg[0] == '\0')
        break;
    }
    return 0;
}

void clean_up(const char * mq_name, const char * shm_name, const char * sem1_name, const char * sem2_name)
{
    clean_up_shm(shm_name);
    // clean semaphore
    sem_close(sem_p1);
    sem_unlink(sem1_name);
    sem_close(sem_p2);
    sem_unlink(sem2_name);
}

void clean_up_mq(const char * mq_name) 
{
    mq_close(mq_fd);
    mq_unlink(mq_name);
}

void clean_up_shm(const char * shm_name) 
{   
    munmap(shared_memory, sizeof(shared_use_st));
}
