#include"structura.h"
#define MESSAGE 256
#define PID 32
int ok=1;
void handle_sigusr1(int signum) {
    char text[MESSAGE]="[REPORT]A fost adaugat un raport nou.\n";
    //1 este iesirea standard
    write(1,text,strlen(text));
}

void handle_sigint(int signum) {
    char text[MESSAGE]="[OFF]SIGINT primit.Se inchide monitorul\n";
    write(1,text,strlen(text));
    ok=0;
}

int main(void) {
    char mesaj[MESSAGE];
    int fin=open(".monitor_pid",O_RDONLY);
    if (fin!=-1) {
        char pid[PID]={0};
        int nr=read(fin,pid,sizeof(pid)-1);
        pid[nr]='\0';
        close (fin);
        if (nr>0) {
            pid_t existing=(pid_t)atoi(pid);
            if (existing>0 && kill(existing,0)==0) {
                //procesul exista
                snprintf(mesaj, sizeof(mesaj),
                    "[ERROR] Monitorul cu PID %d ruleaza deja\n", existing);
                write(STDOUT_FILENO, mesaj, strlen(mesaj));
                exit(1);

            }
            /* Procesul nu exista -> fisier orfan, raportam si continuam */
            snprintf(mesaj, sizeof(mesaj),
                "[INFO] .monitor_pid orfan (PID %d invalid), se suprascrie\n",
                existing);
            write(STDOUT_FILENO, mesaj, strlen(mesaj));

        }
    }
    //declaram 2 variabile de tipul struct sigaction pentru cele 2 moduri de actiune asupra semnalelor
    struct sigaction sa_int;
    struct sigaction sa_usr1;
    //initializam structurile
    memset(&sa_int,0,sizeof(sa_int));
    memset(&sa_usr1,0,sizeof(sa_usr1));
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
    sa_usr1.sa_flags = SA_RESTART;
    if (sigaction(SIGUSR1, &sa_usr1, NULL) == -1) {
        perror("Eroare SIGUSR1");
        exit(1);
    }
    fin=open(".monitor_pid",O_CREAT|O_WRONLY|O_TRUNC,0644);
    if(fin<0) {
        write(STDOUT_FILENO, "[ERROR] Nu pot crea .monitor_pid\n", 33);
        exit(1);
    }
    pid_t pid_new=getpid();
    char string_pid[PID];
    int n = snprintf(string_pid, sizeof(string_pid), "%d\n", pid_new);
    write(fin, string_pid, n);
    close(fin);

    snprintf(mesaj, sizeof(mesaj), "[INFO] Monitor pornit cu PID %d\n", pid_new);
    write(STDOUT_FILENO, mesaj, strlen(mesaj));

    while (ok) {
        pause();
    }
    unlink(".monitor_pid");
    return 0;
}