#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<signal.h>

#define MESSAGE 100
#define PID 20
int ok=1;
void handle_sigusr1(int signum) {
    char text[100]="A fost adaugar un raport nou.\n";
    //1 este iesirea standard
    write(1,text,strlen(text));
}

void handle_sigint(int signum) {
    char text[100]="SIGINIT primit.Se inchide monitorul\n";
    write(1,text,strlen(text));
    ok=0;
}

int main(void) {
    //declaram 2 variabile de tipul struct sigaction pentru cele 2 moduri de actiune asupra semnalelor
    struct sigaction sa_int;
    struct sigaction sa_usr1;
    //dau functiile corespunzatoare in structura
    sa_int.sa_handler=handle_sigint;
    sa_usr1.sa_handler=handle_sigusr1;
    //initializam o masca care nu blocheaza nici nu semnal deoarece nu este scopul programului
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags=0;
    if (sigaction(SIGINT,&sa_int,NULL)!=0) {
        perror("Eroare SIGINT");
        exit(1);
    }
    sigemptyset(&sa_usr1.sa_mask);
    sa_usr1.sa_flags = 0;
    if (sigaction(SIGUSR1, &sa_usr1, NULL) == -1) {
        perror("Eroare SIGUSR1");
        exit(1);
    }
    int fin=open(".monitor_pid",O_CREAT|O_WRONLY|O_TRUNC,0644);
    if(fin<0) {
        perror("Eroare la creare .monitor_pid");
        exit(1);
    }
    pid_t pid=getpid();
    char string_pid[PID];
    snprintf(string_pid,sizeof(string_pid),"%d\n",pid);
    write(fin,string_pid,strlen(string_pid));
    close(fin);
    printf("pid=%d\n",pid);
    while (ok) {
        pause();
    }
    unlink(".monitor_pid");
    printf(".monitor_pid a fost sters\n");
    return 0;
}