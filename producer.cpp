#include <stdio.h>
#include <random>
#include <math.h>
#include <bits/stdc++.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <ctime>
#include <signal.h>
#include <errno.h>
#include <chrono>
#include <string>
#include <string.h>
#include "mysemaphore.h"

#define SEM_KEY 0x11111
#define SEM_KEY2 0x22222
#define SEM_KEY3 0x54123
#define MAX_SIZE_OF_COMMODITY_NAME 11
using namespace std;
int shmid;
int shmid2;
typedef struct Commodity
{
    char commodityName[MAX_SIZE_OF_COMMODITY_NAME];
    double commodityPrice;
    Commodity(char* name, double price)
    {
        strcpy(commodityName,name);
        commodityPrice = price;
    }
}Commodity;

typedef struct data
{
    int front;
    int rear;
    int capacity;
    int size;
}data;

void remove_semaphore(key_t key) {
   int semid;
   int retval;
   semid = semget(key, 1, 0);
      if (semid < 0) {
         perror("Remove Semaphore: Semaphore GET: ");
         return;
      }
   retval = semctl(semid, 0, IPC_RMID);
   if (retval == -1) {
      perror("Remove Semaphore: Semaphore CTL: ");
      return;
   }
   return;
}

void signalhandler(int num)
{  
    printf("Memory Destoried\n");
    shmctl(shmid2,IPC_RMID, NULL);
    shmctl(shmid,IPC_RMID, NULL);
    remove_semaphore(SEM_KEY);
    remove_semaphore(SEM_KEY2);
    remove_semaphore(SEM_KEY3);
}

double setPrice(double priceMean, double standardDeviation)
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator (seed);
    normal_distribution<double> distribution(priceMean,standardDeviation);
    double num = distribution(generator);
    return num;
}

void printlog(int status, char* name, double price, int sleept)
{
    time_t t = time(NULL);
	struct tm tm = *localtime(&t);
    struct timespec spec;
    clock_t currenttime = clock_gettime(CLOCK_MONOTONIC,&spec);
    if (status == 1)
    {
        fprintf(stderr,"[%d/%02d/%02d %02d:%02d:%02d.%ld] %s: generating new value %lf\n",
        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,(spec.tv_nsec % 1000),name,price);
    }
    else if (status == 2)
    {
        fprintf(stderr,"[%d/%02d/%02d %02d:%02d:%02d.%ld] %s: trying to get the mutex\n",
        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,(spec.tv_nsec % 1000),name);
    }
    else if (status == 3)
    {
        fprintf(stderr,"[%d/%02d/%02d %02d:%02d:%02d.%ld] %s: placing %lf in the shared buffer\n",
        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, (spec.tv_nsec % 1000),name,price);
    }
    else
    {
        fprintf(stderr,"[%d/%02d/%02d %02d:%02d:%02d.%ld] %s: sleeping for %d ms\n",
        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, (spec.tv_nsec % 1000),name,sleept);
    }
}

int main(int argc, char** argv)
{
    // char argv[50][50];
    // int argc = 6;
    // strcpy(argv[1],"GOLD");
    // strcpy(argv[2],"7.1");
    // strcpy(argv[3],"0.5");
    // strcpy(argv[4],"200");
    // strcpy(argv[5],"40");
    signal(SIGTSTP,signalhandler);
    if (argc == 6)
    {
        system("touch broker.txt");
        system("touch m.txt");
        key_t key2 = ftok("m.txt",'B');
        data *d;
        shmid2 = shmget(key2,sizeof(data), 0666 | IPC_CREAT | IPC_EXCL);
        if (shmid2 < 0)
        {
            if (errno == EEXIST)
            {
                shmid2 = shmget(key2,sizeof(data), 0666);
                // shmctl(shmid2,IPC_RMID, NULL);
            }
            if (shmid2 < 0)
            {
               
                perror("Memory was not attached");
                exit(0);                
            }
        }
        d = (data*) shmat(shmid2,(void*)0,0);
        
        /*___________________________CONSUMER WAIT SEMAPHORE__________________*/
        struct sembuf sem_buf2;
        struct semid_ds buf2;
        int retval2;
        int semid2 = semget(SEM_KEY2,1,0);
        // if (semid2 >= 0)
        // {
        //     //semaphore created success
        //     sem_buf2.sem_op = -1;
        //     sem_buf2.sem_num = 0;
        //     sem_buf2.sem_flg = 0;
        //     printf("Producer created\n");
        //     retval2 = semop(semid2, &sem_buf2, 1);
        //     if (retval2 == -1)   
        //     {
        //         perror("Semaphore Operation: ");
        //         exit(0);
        //     }
        //     }
        //     else if (errno == EEXIST)
        //     {
        //         //connect other process semaphore
        //         semid2 = semget(SEM_KEY2, 1, 0);
        //         if (semid2 < 0)
        //         {
        //             perror("Semaphore Operation: ");
        //             exit(0);
        //         }
        //         sem_buf2.sem_op = -1;
        //         sem_buf2.sem_num = 0;
        //         //sem_buf2.sem_flg = SEM_UNDO;
        //         printf("Producer attached\n");
        //         retval2 = semop(semid2, &sem_buf2, 1);
        //         if (retval2 == -1)   
        //         {
        //             perror("Semaphore Operation: ");
        //             exit(0);
        //         }
        //     }
            
        /*___________________________________________________________________*/
        key_t key = ftok("broker.txt",'A');
        shmid = shmget(key,sizeof(Commodity)*stoi(argv[5]), 0666 | IPC_CREAT | IPC_EXCL);
        if (shmid == -1)
        {
            if (errno == EEXIST)
            {
                
                shmid = shmget(key,sizeof(Commodity)*stoi(argv[5]), 0666);
                // shmctl(shmid,IPC_RMID,NULL);
            }
            if (shmid < 0)
            {
                 
                perror("Memory was not attached");
                exit(0);                
            }
        }
        Commodity* cmsg = (Commodity*) shmat(shmid,(void *) 0,0);
        Commodity c(argv[1],0.00);
        //_______________________________________________________
        struct sembuf sem_buf3;
        struct semid_ds buf3;
        int retval3;
        int semid3 = semget(SEM_KEY3,1, 0);
                sem_buf3.sem_op = d->size;
                sem_buf3.sem_num = 0;
                sem_buf3.sem_flg = SEM_UNDO;
                retval3 = semop(semid3,&sem_buf3,1);
                if (retval3 == -1)
                {
                    perror("Sempahore Operation :");
                    exit(0);
                }
        // if (semid3 >= 0)
        // {
        //     //semaphore created success
        //     sem_buf3.sem_op = 0;
        //     sem_buf3.sem_num = 0;
        //     sem_buf3.sem_flg = 0;
        //     retval3 = semop(semid3, &sem_buf3, 1);
        //     if (retval3 == -1)   
        //     {
        //         perror("Semaphore Operation: ");
        //         exit(0);
        //     }
        //     }
        //     else if (errno == EEXIST)
        //     {
        //         //connect other process semaphore
        //         semid3 = semget(SEM_KEY3, 1, 0);
        //         if (semid3 < 0)
        //         {
        //         perror("Semaphore was not connected");
        //         exit(0);
        //         }
        //         sem_buf3.sem_op = 0;
        //         sem_buf3.sem_num = 0;
        //         sem_buf3.sem_flg = SEM_UNDO;
        //         retval3 = semop(semid3,&sem_buf3,1);
        //         if (retval3 == -1)
        //         {
        //             perror("Sempahore Operation :");
        //             exit(0);
        //         }
            
        //     }
            /*____________________________________________________________________*/
        struct sembuf sem_buf;
        struct semid_ds buf;
        int retval;
        int semid = semget(SEM_KEY,1, IPC_CREAT | IPC_EXCL | 0666);
        if (semid >= 0)
        {
            //semaphore created success
            sem_buf.sem_op = 1;
            sem_buf.sem_num = 0;
            sem_buf.sem_flg = 0;
            retval = semop(semid, &sem_buf, 1);
            if (retval == -1)   
            {
                perror("Semaphore Operation: ");
                exit(0);
            }
            }
            else if (errno == EEXIST)
            {
                //connect other process semaphore
                semid = semget(SEM_KEY, 1, 0);
                if (semid < 0)
                {
                perror("Semaphore was not connected");
                exit(0);
                }
                sem_buf.sem_op = 0;
                sem_buf.sem_num = 0;
                sem_buf.sem_flg = SEM_UNDO;
                retval = semop(semid,&sem_buf,1);
                if (retval == -1)
                {
                    perror("Sempahore Operation :");
                    exit(0);
                }
            
            }
        while (1)
        {
            c.commodityPrice = setPrice(stod(argv[2]),stod(argv[3]));
            printlog(1,c.commodityName,c.commodityPrice,stoi(argv[4]));
            printlog(2,c.commodityName,c.commodityPrice,stoi(argv[4]));
            /*__________________lock full buffer_____________________________*/
            if (d->size >= d->capacity)
            {
                printf("Buffer is full\n");
            
            sem_buf2.sem_num = 0;
            sem_buf2.sem_op = -1; /* Allocating the resources */
            sem_buf2.sem_flg = SEM_UNDO;\
            retval2 = semop(semid2, &sem_buf2, 1);
            if (retval2 == -1) {
                printf("Here\n");
                perror("Semaphore Locked: ");
                exit(0);
            }
            }
            /*____________________________________________________________*/
            sem_buf.sem_num = 0;
            sem_buf.sem_op = -1; /* Allocating the resources */
            sem_buf.sem_flg = SEM_UNDO;
            retval = semop(semid, &sem_buf, 1);
            if (retval == -1)
            {
                perror("Semaphore Locked: ");
                exit(0); 
            }           
            d->rear = (d->rear + 1) % d->capacity;
            cmsg[d->rear] = c;
            d->size = d->size + 1;
            sem_buf.sem_op = 1; /* Releasing the resource */
            retval = semop(semid, &sem_buf, 1);            
            /*___________________Unlock empty buffer______________________*/
            sem_buf3.sem_op = 1; /* Releasing the resource */
            retval3 = semop(semid3, &sem_buf3, 1);
            if (retval3 == -1)
            {
                perror("Semaphore is locked :");
                exit(0);
            }
            /*_____________________________________________________________*/
            printlog(3,c.commodityName,c.commodityPrice,stoi(argv[4]));
            printlog(4,c.commodityName,c.commodityPrice,stoi(argv[4]));
            this_thread::sleep_for(chrono::milliseconds(stoi(argv[4])));
        }
    }
    else
    {
        perror("Invalid number of inputs");
        exit(0);
    }
}