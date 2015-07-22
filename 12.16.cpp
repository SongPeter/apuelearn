/**********************************************************
* Author        : SYY
* Email         : food.tea@hotmail.com
* Last modified : 2015-07-22 06:08
* Filename      : 12.16.cpp
* Description   : 
* *******************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
int quitflage;
sigset_t mask;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  waitloc = PTHREAD_COND_INITIALIZER;
void * thr_f(void *arg)
{
    int err,signo;
    for(;;)
    {
        err = sigwait(&mask,&signo);
        if(err != 0)
        {
            printf("signal wait failed:%d\n",err);
        }
        switch(signo)
        {
            case SIGINT:
            printf("\ninterrupt\n");
            break;
            case SIGQUIT:
            pthread_mutex_lock(&lock);
            quitflage = 1;
            pthread_mutex_unlock(&lock);
            pthread_cond_signal(&waitloc);
            printf("\n quit\n");
            return (0);
            default:
            printf("unexpected signal %d \n",signo);
            exit(1);
        }
    }
}
int main(void)
{
    int err;
    sigset_t oldmask;
    pthread_t tid;
    sigemptyset(&mask);
    sigaddset(&mask,SIGINT);
    sigaddset(&mask,SIGQUIT);
    pthread_sigmask(SIG_BLOCK,&mask,&oldmask);
    pthread_create(&tid,NULL,thr_f,NULL);
    pthread_mutex_lock(&lock);
    while(quitflage == 0)
    {
        pthread_cond_wait(&waitloc,&lock);
    }
    pthread_mutex_unlock(&lock);
    quitflage = 0;
    sigprocmask(SIG_SETMASK,&oldmask,NULL);
    exit(0);
}
