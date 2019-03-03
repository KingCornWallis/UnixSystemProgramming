// POSIX libararies
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

// Message Queue libraries
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>
#include "message.h"
#include <unistd.h>
#include <sys/mman.h>

// Semaphore libraries
#include <signal.h>
#include <semaphore.h>

// Standerd libraries
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define MQ_NAME "/mikas_mq"
#define SHM_NAME "/mikas_shm"
#define SEM1_NAME "/mikas_sem1" 
#define SEM2_NAME "/mikas_sem2" 

using namespace std;

static mqd_t mq_fd;
static int shm_fd; 
static void *shared_memory;
static sem_t *sem_p1;
static sem_t *sem_p2;
static struct shared_use_st *shared_stuff;
char shared_message[256];
int await = 1;

void clean_up(); 
void clean_up_mq();
void clean_up_shm();


void tostring(char str[], int num)
{
    int i, rem, len = 0, n;
 
    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
}

void my_handler(int signum)
{
    if (signum == SIGUSR1)
    {
        await=0;
    }
}

int main()
{
    //message queue

    struct mq_attr mq_attribute;
    mq_attribute.mq_flags = 0;
    mq_attribute.mq_maxmsg = 1;
    mq_attribute.mq_msgsize = 256;

    if ((mq_fd = mq_open(MQ_NAME,
		O_CREAT | O_WRONLY, 
		0666, &mq_attribute)) == -1)
    {
        perror("Failed creating the message queue");
        exit(1);
    }


    // shared memory segment, 
    shared_memory = (void *)0;

    // Open a shared memory object
    shm_fd = shm_open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL, 0666);
    if (shm_fd == -1)
    {
        clean_up_mq();
        perror("Failed opening the shared memory");
        exit(1);
    }

    if(ftruncate(shm_fd, sizeof(struct shared_use_st)) == -1)
    {
        perror("ftruncate");
        exit(1);
    }

    // Map the object into our address space.
    shared_memory = mmap(NULL, sizeof(struct shared_use_st),PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shared_memory == MAP_FAILED)
    {
        clean_up_mq();
        clean_up_shm();
        perror("mmap");
        exit(1);
    }

    close(shm_fd);
    shared_stuff = (struct shared_use_st *)shared_memory;
    shared_stuff-> written_by_you = 0;

    // open semaphore
    sem_p1 = sem_open(SEM1_NAME, O_CREAT | O_EXCL, 0600, 1);
    sem_p2 = sem_open(SEM2_NAME, O_CREAT | O_EXCL, 0600, 1);

    const int running = 1; 
    const int child_process = 0; 

    sem_wait(sem_p1);

    int master_pid = getpid();
    int p_id = fork();
    if (p_id == -1)
    {    
        clean_up();
        perror("Failed forking the process");
        exit(1);
    }  
    else if (p_id == child_process)
    {

        int p_id = fork();
        if (p_id == -1)
        {    
            clean_up();
            perror("Failed forking the process");
            exit(1);
        }  
        else if (p_id == child_process)
        {
            if (execl("reverse", "reverse", SEM1_NAME, SHM_NAME, MQ_NAME, SEM2_NAME, (char* )0) == -1)
            {
                clean_up();
                perror("Failed executing reverse the process");
                exit(1);
            }
        }
        else 
        {
            char master_pid_str [20];
            tostring(master_pid_str, master_pid);
            if (execl("upper", "upper", SEM1_NAME, SHM_NAME, SEM2_NAME, master_pid_str,(char* )0) == -1)
            {
                clean_up();
                perror("Failed executing  upper the process");
                exit(1);
            }
        }
    }
    if(p_id > child_process) 
    {
        while(running)
        {
            string input_message = "";
            int sent_message_id;

            cout << "> ";
            getline(cin, input_message);
            if(input_message.length() == 0 && !cin.eof())
                continue;
            
            strcpy(shared_message, input_message.c_str());
            shared_message[255] = '\0';

            sent_message_id = mq_send(mq_fd, shared_message, sizeof(shared_message), 1); 
            if (sent_message_id == -1)
            {
                cout << "Message queue is full\n";
                break;
            }

            signal(SIGUSR1, my_handler);
            while (await == 1);
            await = 1;
            if(cin.eof())
                break;
            if(input_message.length() == 0)
                continue;
        }
        clean_up();
        return 0;
    }

}

void clean_up()
{
    clean_up_mq();
    clean_up_shm();
    // clean semaphore
    sem_close(sem_p1);
    sem_unlink(SEM1_NAME);
    sem_close(sem_p2);
    sem_unlink(SEM2_NAME);
}

void clean_up_mq() 
{
    mq_close(mq_fd);
    mq_unlink(MQ_NAME);
}

void clean_up_shm() 
{   
    munmap(shared_memory, sizeof(shared_use_st));
    shm_unlink(SHM_NAME);
}
