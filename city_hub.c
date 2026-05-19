#include "structura.h"
#define BUFFER 1024
#define DISTRICT_MAX 10
#define COMANDA 256
pid_t pid_hub_mon=-1;
void start_monitor() {
    if (pid_hub_mon > 0) {
        printf("[city_hub] Exista deja un monitor pornit(PID %d).\n"
               "Iesi din aplicatie (comanda 'exit') pentru a-l opri inainte de a reporni.\n", pid_hub_mon);
        return;
    }
    pid_t hub_mon=fork();
    pid_hub_mon=hub_mon;
    if (hub_mon<0) {
        perror("Fork hub_mon esuat in functia start_monitor");
        return ;
    }
    if (hub_mon==0) {
        int pipefd[2];
        if (pipe(pipefd)<0) {
            perror("Pipe failed");
            exit(1);
        }
        pid_t monitor_pid=fork();
        if (monitor_pid<0) {
            perror("Fork monitor_pid esuat ");
            exit(1);
        }
        if (monitor_pid==0) {
            close(pipefd[0]);//inchidem capatul de citire
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);//inchidem capatul de scriere al monitorului
            execlp("./monitor_reports","monitor_reports",NULL);
            const char *err = "[ERROR] execlp monitor_reports failed\n";
            write(STDOUT_FILENO, err, strlen(err));
            exit(1);
        }
        close(pipefd[1]);//inchidem capatul de scriere al hub_mon

        char buffer[BUFFER];
        int citit;
        int eroare=0;
        while ((citit=read(pipefd[0],buffer,BUFFER-1))>0) {
            buffer[citit]='\0';
            if (strstr(buffer, "[ERROR]") != NULL) {
                printf("\n %s", buffer);
                eroare = 1; // Salvăm starea de eroare
            } else if (strstr(buffer, "[OFF]") != NULL) {
                printf("\n %s", buffer);
            } else if (strstr(buffer, "[REPORT]") != NULL) {
                printf("\n %s", buffer);
            } else {
                printf("\n %s", buffer);
            }
            fflush(stdout);
        }
        close(pipefd[0]);
        waitpid(monitor_pid, NULL, 0);//pentru a nu ramane monitor_pid zombie
        if (eroare == 1) {
            printf("Executia monitorului a esuat.\n");
        } else {
            printf("Monitorul s-a inchis constrolat.\n");
        }
        exit(0);
    }
    pid_hub_mon = hub_mon;
    printf("hub_mon pornit (PID %d).\n", hub_mon);
}


void stop_monitor() {
    int fin = open(".monitor_pid", O_RDONLY);
    if (fin == -1) {
        printf("Monitorul nu este pornit (nu am gasit .monitor_pid).\n");
        return;
    }

    char string_pid[20];
    memset(string_pid, 0, sizeof(string_pid));
    int nr = read(fin, string_pid, sizeof(string_pid) - 1);
    close(fin);

    if (nr > 0) {
        pid_t pid = atoi(string_pid);
        // Trimitem SIGINT (semnalul pe care monitorul stie sa il gestioneze)
        if (pid>0 && (kill(pid, SIGINT) == 0)) {
            printf("Semnal de oprire trimis catre monitor (PID: %d).\n", pid);
            fflush(stdout);
        } else {
            perror("Eroare la oprirea monitorului");
        }
    }
    if (pid_hub_mon > 0) {
        waitpid(pid_hub_mon, NULL, 0);
        pid_hub_mon = -1; // Eliberam slotul, monitorul a murit complet
    }
}

void calculate_scores(char *argv) {
    if (argv==NULL) {
        printf("Trebuie introsud cel putin un district pentru a putea fi calculat scorul\n");
        return;
    }
    int nr=0;
    int pipefds[DISTRICT_MAX][2];
    pid_t pids[DISTRICT_MAX];
    char *token=strtok(argv," \n");
    while (token!=NULL && nr<DISTRICT_MAX) {
        if (pipe(pipefds[nr])==-1) {
            perror("Pipe scorer failed");
            token=strtok(NULL," \n");
            continue;
        }
        pids[nr]=fork();
        if (pids[nr]<0) {
            perror("Fork scorer failed");
            close(pipefds[nr][0]);
            close(pipefds[nr][1]);
            token=strtok(NULL," \n");
            continue;
        }
        if (pids[nr]==0) {
            close(pipefds[nr][0]);//inchidem capatul de citire
            dup2(pipefds[nr][1],STDOUT_FILENO);
            close(pipefds[nr][1]);

            execlp("./scorer","scorer",token,NULL);
            perror("execlp scorer failed");
            exit(1);
        }
        close(pipefds[nr][1]);
        nr++;
        token=strtok(NULL," \n");
    }
    for (int i=0;i<nr;i++) {
        char buffer[BUFFER];
        int citit;

        while ((citit= read(pipefds[i][0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[citit] = '\0';
            printf("%s", buffer);
        }
        close(pipefds[i][0]);
        waitpid(pids[i], NULL, 0); // Asteptam ca procesul muncitor sa se termine complet
    }
}

int main(void) {
    char input[COMANDA];
    printf("Terminal city_hub.Comenzi disponibile:\n--exit -> pentru a iesi din city_hub\n--start_monitor -> porneste monitor_reports\n--calculate_scores -> calculeaza scorul de severitate dintr-un district pentru fiecare  inspector\n[INFO] calculate_scores necesita minim un district ca argument altfel functionalitatea nu va putea fi utilizata\n");
    while (1) {
        printf("city_hub>");
        if (fgets(input,COMANDA,stdin)==NULL) {
            break;
        }
        char *comanda=strtok(input," \n");
        if (comanda==NULL)continue;
        char *argv=strtok(NULL,"");
        if (strcmp(comanda,"exit")==0) {
            stop_monitor();
            break;
        }else if (strcmp(comanda,"start_monitor")==0) {
            start_monitor();
        }else if (strcmp(comanda,"calculate_scores")==0) {
            calculate_scores(argv);
        }else printf("Comanda %s este invalida",comanda);
    }
    return 0;
}