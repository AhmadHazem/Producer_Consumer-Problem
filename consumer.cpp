#include <iostream>
#include <chrono>
#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <thread>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include "mysemaphore.h"
using namespace std;
using namespace this_thread;
#define SEM_KEY2 0x55522
#define SEM_KEY3 0x44551
#define MAX_SIZE_OF_COMMODITY_NAME 12
int shmid;
int shmid2;

typedef struct 
{
    char commodityName[MAX_SIZE_OF_COMMODITY_NAME];
    double commodityPrice;
 
}Commodity;

typedef struct data
{
    int front;
    int rear;
    int capacity;
    int size;
}data;

typedef struct Marketplace
{   
    char commodityName[MAX_SIZE_OF_COMMODITY_NAME];
    double commodityPrice1;
    double commodityPrice2;
    double commodityPrice3;
    double commodityPrice4;
    double AveragePrice;
    double oldAveragePrice;
    char* indicator1 = (char *) malloc(sizeof(char)*5);
    char* indicator2 = (char *) malloc(sizeof(char)*5);
    Marketplace(const char* name)
    {
        strcpy(commodityName,name);
        commodityPrice1 = 0.00;
        commodityPrice2 = 0.00;
        commodityPrice3 = 0.00;
        commodityPrice4 = 0.00;
        AveragePrice = 0.00;
        oldAveragePrice = 0.00;
        strcpy(indicator1," ");
        strcpy(indicator2," ");
    }
}Marketplace;

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
    shmctl(shmid,IPC_RMID, NULL);
    shmctl(shmid2,IPC_RMID, NULL);
    remove_semaphore(SEM_KEY2);
    remove_semaphore(SEM_KEY2);
}

void printbidding(Marketplace* m, int size)
{
    
    printf("+-------------------------------------+\n");
    printf("| Currency      |  Price   | AvgPrice |\n");
    printf("+-------------------------------------+\n");
    for (size_t i = 0; i < size; i++)
    {

        //printf("| %-14s| %7.2lf%s | %7.2lf%s |\n",m[i].commodityName,m[i].commodityPrice1,m[i].indicator1,m[i].AveragePrice,m[i].indicator2);
        printf("| %-14s|",m[i].commodityName);
        if(!strcmp(m[i].indicator1,u8"↑"))
        {
            printf("\033[;32m %7.2lf%s \033[0m",m[i].commodityPrice1,m[i].indicator1);
        }
        else if(!strcmp(m[i].indicator1,u8"↓"))
        {
            printf("\033[;31m %7.2lf%s \033[0m",m[i].commodityPrice1,m[i].indicator1);
        }
        else
        {
            
            printf("\033[;36m %7.2lf%s \033[0m",m[i].commodityPrice1,m[i].indicator1);
        }
        printf("|");
        if(!strcmp(m[i].indicator2,u8"↑"))
        {
            printf("\033[;32m %7.2lf%s \033[0m",m[i].AveragePrice,m[i].indicator2);
        }
        else if(!strcmp(m[i].indicator2,u8"↓"))
        {
            printf("\033[;31m %7.2lf%s \033[0m",m[i].AveragePrice,m[i].indicator2);
        }
        else
        {
             printf("\033[;36m %7.2lf%s \033[0m",m[i].AveragePrice,m[i].indicator2);
        }
        printf("|\n");
    }
    printf("+-------------------------------------+\n\n");
}




int isExisting(Marketplace* commodities, int size, Commodity *commodity)
{
    int count = size;
    for (size_t i = 0; i < count; i++)
    {
        if (!strcasecmp(commodities[i].commodityName,commodity->commodityName))
        {
            return i;
        }
    }
    return -1;
}

Marketplace* updateCommoditiy(Marketplace* commodities, int commodityNumber, Commodity* commodity)
{
    //printf("%lf\n",commodity->commodityPrice);
    commodities[commodityNumber].commodityPrice4 = commodities[commodityNumber].commodityPrice3;
    commodities[commodityNumber].commodityPrice3 = commodities[commodityNumber].commodityPrice2;
    commodities[commodityNumber].commodityPrice2 = commodities[commodityNumber].commodityPrice1;
    if (commodity->commodityPrice > commodities[commodityNumber].commodityPrice1)
    {
        strcpy(commodities[commodityNumber].indicator1, u8"↑");
    }
    else
    {
        strcpy(commodities[commodityNumber].indicator1, u8"↓");
    }
    commodities[commodityNumber].commodityPrice1 = commodity->commodityPrice;
    commodities[commodityNumber].AveragePrice = (commodities[commodityNumber].commodityPrice1 + commodities[commodityNumber].commodityPrice2 + commodities[commodityNumber].commodityPrice3 + commodities[commodityNumber].commodityPrice4)/4.0;
    if (commodities[commodityNumber].AveragePrice > commodities[commodityNumber].oldAveragePrice)
    {
        strcpy(commodities[commodityNumber].indicator2, u8"↑");
    }
    else
    {
        strcpy(commodities[commodityNumber].indicator2, u8"↓");
    }
    commodities[commodityNumber].oldAveragePrice = commodities[commodityNumber].AveragePrice;
    return commodities;
}

Marketplace * insertcomodities(Marketplace* m)
{
    Marketplace n("ALUMININUM");
    m[0] = n; 
    Marketplace n2("COPPER");
    m[1] = n2;
    Marketplace n4("COTTON");
    m[2] = n4;
    Marketplace n1("CRUDEOIL");
    m[3] = n1;
    Marketplace n3("GOLD");
    m[4] = n3;
    Marketplace n10("LEAD");
    m[5] = n10;
    Marketplace n5("METHANOIL");
    m[6] = n5;
    Marketplace n7("NATURALGAS");
    m[7] = n7;  
    Marketplace n6("NICKEL");
    m[8] = n6;
    Marketplace n8("SILVER");
    m[9] = n8;
    Marketplace n9("ZINC");
    m[10] = n9;
    return m;    
}



int main(int argv, char** argc)
{
    // int argv = 2;
    // char argc[10][10];
    strcpy(argc[1],"20");
    if (argv != 2)
    {
        printf("Error\n");
        exit(0);
    }
    system("touch broker.txt");
    system("touch m.txt");
    signal(SIGINT,signalhandler);
    //vector <Marketplace> commodities; 
    Marketplace* commodities = (Marketplace *)malloc(sizeof(Marketplace)*11);
    commodities =insertcomodities(commodities);
    key_t key = ftok("broker.txt",'A');
    int shmid = shmget(key,sizeof(Commodity)*stoi(argc[1]),0666|IPC_CREAT);
    Commodity *cmsg = (Commodity*) shmat(shmid,(void*)0,0);
    key_t key2 = ftok("m.txt",'B');
    int shmid2 = shmget(key2,sizeof(data), 0666 | IPC_CREAT);
    data *d = (data*) shmat(shmid2,(void*)0,0);
    d->capacity = stoi(argc[1]);
    d->front = d->size = 0;
    d->rear = d->capacity - 1;
     /*____________________________________________________________________*/
        struct sembuf sem_buf2;
        struct semid_ds buf2;
        int retval2;
        int semid2 = semget(SEM_KEY2,1, IPC_CREAT | IPC_EXCL | 0666);
        if (semid2 >= 0)
        {
            //semaphore created success
            sem_buf2.sem_op = 1;
            sem_buf2.sem_num = 0;
            sem_buf2.sem_flg = 0;
            retval2 = semop(semid2, &sem_buf2, 1);
            if (retval2 == -1)   
            {
                perror("Semaphore Operation: ");
                exit(0);
            }
        }
        else if (errno == EEXIST)
        {
            //connect other process semaphore
            semid2 = semget(SEM_KEY2, 1, 0);
            if (semid2 < 0)
            {
            perror("Semaphore was not connected");
            exit(0);
            }
            sem_buf2.sem_op = 1;
            sem_buf2.sem_num = 0;
            sem_buf2.sem_flg = SEM_UNDO;
            retval2 = semop(semid2,&sem_buf2,1);
            if (retval2 == -1)
            {
                perror("Sempahore Operation :");
                exit(0);
            }
        }
        /*__________________________________________________________________*/
        struct sembuf sem_buf3;
        struct semid_ds buf3;
        int retval3;
        int semid3 = semget(SEM_KEY3,1, IPC_CREAT | IPC_EXCL | 0666);
        if (semid3 >= 0)
        {
            //semaphore created success
            sem_buf3.sem_op = 1;
            sem_buf3.sem_num = 0;
            sem_buf3.sem_flg = 0;
            retval3 = semop(semid3, &sem_buf3, 1);
            if (retval3 == -1)   
            {
                perror("Semaphore Operation: ");
                exit(0);
            }
        }
        else if (errno == EEXIST)
        {
            //connect other process semaphore
            semid3 = semget(SEM_KEY3, 1, 0);
            if (semid3 < 0)
            {
            perror("Semaphore was not connected");
            exit(0);
            }
            sem_buf3.sem_op = 1;
            sem_buf3.sem_num = 0;
            sem_buf3.sem_flg = SEM_UNDO;
            retval3 = semop(semid3,&sem_buf3,1);
            if (retval3 == -1)
            {
                perror("Sempahore Operation :");
                exit(0);
            }
        }
    while (1)
    {
        
        if (d->size == 0)
        {
            printf("Buffer was empty\n");
            locksemaphore(sem_buf3,buf3,semid3);
            while(d->size == 0)
            {

            }
        }
        Commodity * cptr = (Commodity *)malloc(sizeof(Commodity));
        strcpy(cptr->commodityName,cmsg[d->front].commodityName);
        cptr->commodityPrice = cmsg[d->front].commodityPrice;
        d->front = (d->front + 1) % d->capacity;
        d->size = d->size - 1;
        unlocksemaphore(sem_buf2,buf2,semid2);
        int commoditynumber = isExisting(commodities,11,cptr);
        commodities = updateCommoditiy(commodities,commoditynumber,cptr);
        printbidding(commodities,11);
        printf("\n");
        this_thread::sleep_for(chrono::milliseconds(200));
        system("clear");
    }
}