#include "ipc/ipc.h"
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define MAXSIZE    100

void buzz(void);
void woody(void);
int closeQueue(const char* name); 

void buzz(void) {
    
    char buff[MAXSIZE];
    
    printf("Process Buzz, id: %d, listening...\n",getpid());
    ipc_init();
    ipc_listen("myIPC1");
    
    printf("Process Buzz id: %d, establishing connection...\n",getpid());
    ipc_t qout = ipc_establish("myIPC2");

    char answers[3][70] = {"Are you talking to me???","Cause i don't see nobody else here."
                      "ARE YOU TO TALKING TO ME???", "ARE YOU FUCKING TALKING TO ME?!?!?!"};
    int n, i = 0;
    while (i < 3) {
        n = ipc_read(buff,MAXSIZE);
        buff[n] = '\0';
        printf("Buzz: %s Read %d.\n",buff,n);
        ipc_write(qout, answers[i], strlen(answers[i]));
        i++;
    }

    wait(0);
    printf("Process Buzz, id :%d, closing connection...\n",getpid());
    ipc_close(qout);
    ipc_end();
}

void woody(void) {

    char buff[MAXSIZE];
    int n; 
    printf("Process Woody, id: %d, listening...\n",getpid());
    ipc_listen("myIPC2");
    printf("Process Woody, id: %d, establishing connection...\n",getpid());
    ipc_t qout = ipc_establish("myIPC1");
    if (qout == NULL) {
        return;
    }

    const char* msg1 = "MSG 1: comando estelar, me escucha?";
    const char* msg2 = "MSG 2: saludos, soy Buzz lightyear. Vengo en paz.";
    const char* msg3 = "MSG 3: al infinito y mas alla.";

    ipc_write(qout, msg1, strlen(msg1));
    n = ipc_read(buff,MAXSIZE);
    buff[n] = '\0';
    printf("Woody: %s Read %d.\n",buff,n);
       
    ipc_write(qout, msg2, strlen(msg2));
    n = ipc_read(buff,MAXSIZE);
    buff[n] = '\0';
    printf("Woody: %s Read %d.\n",buff,n);
    
    ipc_write(qout, msg3, strlen(msg3));
    n = ipc_read(buff,MAXSIZE);
    buff[n] = '\0';
    printf("Woody: %s Read %d.\n",buff,n);
  
    printf("Process Woody, id :%d, closing connection...\n",getpid()); 
    ipc_close(qout);
    ipc_end();
    return;
}


int testIPC(void) {

    closeQueue("myIPC1");
    closeQueue("myIPC2");
    printf("Staring conversation....\n");
    sleep(1);

    switch(fork()) {
        case -1:
            perror("error on fork");
            return -1;
        case 0:
            //CHILD - Woody
            woody(); 
            break;
        default:
            //PARENT - Buzz
            buzz(); 
    }
    exit(1);
    return 1;
}


int closeQueue(const char* name) {
    char posix_name[512];
    
    sprintf(posix_name, "/%s", name);
    if ( mq_unlink(posix_name) == -1 ) {
        if(errno == ENOENT) { 
            printf("No queue with name %s exists\n",name);
            return -1;
        }
    }
    return 0;
}
