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


int unlocksemaphore(struct sembuf* sem_buf, struct semid_ds* buf, int semid)
{
    int retval;
    if (sem_buf->sem_op == 0)
    {
        sem_buf->sem_op = sem_buf->sem_op + 1;
        retval = semop(semid,sem_buf,1);
        if (retval == -1)
        {
            perror("Semaphore locked :");
            exit(0);
        }
    }
    return sem_buf->sem_op;
}

int locksemaphore(struct sembuf* sem_buf, struct semid_ds* buf, int semid)
{
    int retval;
    if (sem_buf->sem_op == 1)
    {
        sem_buf->sem_num = 0;
        sem_buf->sem_op = sem_buf->sem_op - 1;
        sem_buf->sem_flg = SEM_UNDO;
    }
    else
    {
        retval = semop(semid,sem_buf,1);
        if (retval == -1)
        {
                    perror("Semaphore locked :");
                    exit(0);
        }
    }
    return sem_buf->sem_op;
}


int createsemaphore(struct sembuf* sem_buf, struct semid_ds buf, key_t key, int capacity)
{
    int retval;
    int semid = semget(key,1, IPC_CREAT | IPC_EXCL | 0666);
    if (semid >= 0)
    {
        //semaphore created success
        sem_buf->sem_op = 1;
        sem_buf->sem_num = 0;
        sem_buf->sem_flg = 0;
        retval = semop(semid, sem_buf, 1);
        if (retval == -1)   
        {
            perror("Semaphore Operation: ");
            exit(0);
        }
        }
        else if (errno == EEXIST)
        {
            //connect other process semaphore
            semid = semget(key, 1, 0);
            if (semid < 0)
            {
            perror("Semaphore was not connected");
            exit(0);
            }
            sem_buf->sem_op = 0;
            sem_buf->sem_num = 0;
            sem_buf->sem_flg = SEM_UNDO;
            retval = semop(semid,sem_buf,1);
            if (retval == -1)
            {
                perror("Sempahore Operation :");
                exit(0);
            }
        }
    return semid;
}