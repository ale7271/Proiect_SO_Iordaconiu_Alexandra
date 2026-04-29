#include <sys/stat.h>
#include<unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include<stdio.h>
#include<sys/wait.h>
#define NUME 50
#define CATEGORIE 30
#define DESCRIERE 100
#define PATH 256
#define LINK 200
#define LOG 100

typedef struct {
    float latitude;
    float longitude;
}GPS;

typedef struct {
    int id;
    char inspectorName[NUME];
    GPS gps;
    char category[CATEGORIE];
    int severity;
    time_t timestamp;
    char description[DESCRIERE];
}REPORT;


void create(const char *district)
{
    if (mkdir(district, 0750) == -1) {
        if (errno != EEXIST) {
            perror("Eroare la crearea directorului\n");
            return;
        }
    }// Setare explicita pentru siguranta
    chmod(district, 0750);

    char path[PATH];
    int f;

    //reports.dat
    snprintf(path,sizeof(path),"%s/reports.dat", district);
    f=open(path, O_CREAT | O_RDWR, 0664);
    if (f == -1)
    {
        perror("Eroare la deschiderea reports.dat");
        return;
    }
    chmod(path, 0664);
    close(f);


    //distrct.cfg
    snprintf(path, sizeof(path),"%s/district.cfg",district);
    f=open(path, O_CREAT | O_RDWR, 0640);
    char buf[]="Threshold=1\n";
    write(f,buf,strlen(buf));
    chmod(path, 0640);
    close(f);

    //logged_district
    snprintf(path, sizeof(path),"%s/logged_district",district);
    f=open(path, O_CREAT | O_RDWR, 0644);
    chmod(path, 0644);
    close(f);

    //symlink
    char link_name[LINK];
    snprintf(link_name,sizeof(link_name),"active_reports-%s",district);
    snprintf(path,sizeof(path),"%s/reports.dat",district);
    struct stat lst;
    if (lstat(link_name,&lst)==0) {
        struct stat st;
        if (stat(link_name,&st)==-1) {
            printf("Warning. Danggling link %s\n",link_name);
            unlink(link_name);
            symlink(path,link_name);
        }
    }
        else {
            symlink(path,link_name);
        }
    }


void add_report (const char *district, const char *user)
{
    REPORT r;
    char path [PATH];
    int f;
    create(district);
    printf("Itroduceti coordonatele gps:\n");
    scanf("%f",&r.gps.latitude);

    scanf("%f",&r.gps.longitude);

    printf("Introduceeti tipul raportului(road/lighting/flooding/other):");
    scanf("%s",r.category);

    printf("Serverity level (1/2/3): ");
    scanf("%d",&r.severity);

    printf("Descriere: ");
    getchar();
    fgets(r.description, DESCRIERE, stdin);


    snprintf(path,sizeof(path),"%s/reports.dat",district);
    f=open(path,O_RDWR,0664);
    if (f==-1) {
        perror("eroare deschidere fisier functie add");
        return;
    }
    struct stat st;
    fstat(f,&st);
    if (st.st_size==0) {r.id=1;}
    else {
        REPORT ultimul;
        lseek(f,-sizeof(REPORT),SEEK_END);
        read(f,&ultimul,sizeof(REPORT));
        r.id=ultimul.id+1;

    }

    strncpy (r.inspectorName,user,NUME-1);
    r.timestamp=time(NULL);

    lseek (f,0,SEEK_END);
    if (write(f,&r,sizeof(REPORT)) ==-1)
    {
        perror("Raportul nu a putut fi salvat");
    }
    else
    {
        printf("Raportul a fost salvat cu ID-ul %d\n",r.id);
    }
    close(f);

}
void add_logged_district(const char *district, const char *user, const char *role, const char *action)
{
    char path[PATH];
    char log[LOG];
    int f;
    snprintf(path,sizeof(path),"%s/logged_district",district);
    struct stat st;
    if (stat(path,&st)==-1) {
        perror("Stat failed on logged district");
        return;
    }
    if ((st.st_mode & S_IWUSR)==0) {
        fprintf(stderr,"No permison to write on logged district\n");
        return;

    }
    f=open(path,O_WRONLY | O_CREAT | O_APPEND, 0644);

    time_t now = time(NULL);
    snprintf(log,sizeof(log),"%s\t%ld\t%s\t%s\n",action,(long)now,role,user);
    write(f,log,strlen(log));
    close(f);
}

void permissions_to_string(mode_t mode,char *perms) {
    perms[0]=(mode & S_IRUSR) ? 'r' : '-';
    perms[1]=(mode & S_IWUSR) ? 'w' : '-';
    perms[2]=(mode & S_IXUSR) ? 'x' : '-';

    perms[3]=(mode & S_IRGRP) ? 'r' : '-';
    perms[4]=(mode & S_IWGRP) ? 'w' : '-';
    perms[5]=(mode & S_IXGRP) ? 'x' : '-';

    perms[6]=(mode & S_IROTH) ? 'r' : '-';
    perms[7]=(mode & S_IWOTH) ? 'w' : '-';
    perms[8]=(mode & S_IXOTH) ? 'x' : '-';

    perms[9]='\0';
}

void list(const char *district)
{
    char path[PATH];
    REPORT r;
    int f;
    snprintf(path, sizeof(path),"%s/reports.dat",district);
    f=open(path,O_RDWR);
    if (f==-1)
    {
        perror("Eroare deschidere raports.dat la listare\n");
        return;
    }
    while (read(f,&r,sizeof(REPORT)) > 0)
    {
        printf("ID: %d\n", r.id);
        printf("Inspector: %s\n", r.inspectorName);
        printf("GPS: latitudine=%.2f, longitudine=%.2f\n", r.gps.latitude, r.gps.longitude);
        printf("Categorie: %s\n", r.category);
        printf("Severitate: %d\n", r.severity);
        printf("Timestamp: %s", ctime(&r.timestamp));
        printf("Descriere: %s\n", r.description);
        printf("-------------------\n");
    }

    struct stat st;
    stat(path,&st);
    if (stat(path,&st) == -1)
    {
        perror("Eroare citire atribute fisier\n");
        return;
    }
    char perm[10];
    permissions_to_string(st.st_mode,perm);

    printf ("Permissions: %s",perm);
    printf("file size: %ld byte\n",st.st_size);
    printf("Last modification: %s",ctime(&st.st_mtime));
    close(f);
}



void view(const char *district, int raport_id)
{
    char path[PATH];
    REPORT r;
    int f;
    snprintf(path, sizeof(path),"%s/reports.dat",district);
    f=open(path,O_RDWR);
    if (f==-1)
    {
        perror("Eroare deschidere raports.dat la listare\n");
        return;
    }
    int gasit=0;
    while (read(f,&r,sizeof(REPORT))>0)
    {
        if (r.id==raport_id)
        {
            printf("ID: %d\n",r.id);
            printf("Inspector: %s\n",r.inspectorName);
            printf("GPS: %.2f,%.2f\n",r.gps.latitude,r.gps.longitude);
            printf("Categorie: %s\n",r.category);
            printf("Severitate: %d\n",r.severity);
            printf("Timestamp: %s",ctime(&r.timestamp));
            printf("Descriere: %s\n",r.description);
            printf("-------------------\n");
            gasit=1;
            break;
        }
    }
    if (gasit==0)
        printf("Raportul cu ID-ul %d nu exista.\n",raport_id);
    close(f);
}

int check_permission(const char *path, const char *role, char action) {
    struct stat st;
    if (stat(path, &st) == -1) return 0;

    // Managerul e user, inspectorii sunt grupul
    if (strcmp(role, "manager") == 0) {
        if (action == 'r') return (st.st_mode & S_IRUSR);
        if (action == 'w') return (st.st_mode & S_IWUSR);
    } else if (strcmp(role, "inspector") == 0) {
        if (action == 'r') return (st.st_mode & S_IRGRP);
        if (action == 'w') return (st.st_mode & S_IWGRP);
    }
    return 0;
}
void update_threshold(const char *district,int value) {
    char path[PATH];
    snprintf(path,sizeof(path),"%s/district.cfg",district);

    struct stat st;
    if (stat(path,&st)==-1) {
        perror("Stat failed on distrct.cfg");
        return;
    }
    if ( (st.st_mode & 0777)!= 0640) {
        fprintf(stderr,"Error! The permissions of district.cfg have been modified\n");
        return;
    }

    int f=open(path,O_WRONLY | O_TRUNC, 0640);
    if (f==-1) {
        fprintf(stderr,"Error opening district.cfg file!\n");
        return;
    }
    char buff[64];
    snprintf(buff,sizeof(buff),"Threshold=%d\n",value);
    write(f,buff,strlen(buff));
    close(f);
    printf("Threshold updated: %d\n",value);
}

/* Funcția 1: Sparge condiția în field, operator și valoare */
int parse_condition(const char *input, char *field, char *op, char *value) {
    if (!input) return 0;

    // Căutăm primul ':'
    const char *first_colon = strchr(input, ':');
    if (!first_colon) return 0;

    // Căutăm al doilea ':'
    const char *second_colon = strchr(first_colon + 1, ':');
    if (!second_colon) return 0;

    // Extragem field
    size_t field_len = first_colon - input;
    strncpy(field, input, field_len);
    field[field_len] = '\0';

    // Extragem operatorul (==, !=, <, <=, >, >=)
    size_t op_len = second_colon - (first_colon + 1);
    strncpy(op, first_colon + 1, op_len);
    op[op_len] = '\0';

    // Extragem valoarea (tot ce a mai rămas)
    strcpy(value, second_colon + 1);

    return 1;
}

/* Funcția 2: Verifică dacă înregistrarea respectă condiția */
int match_condition(REPORT *r, const char *field, const char *op, const char *value) {
    if (strcmp(field, "severity") == 0) {
        int val = atoi(value);
        if (strcmp(op, "==") == 0) return r->severity == val;
        if (strcmp(op, "!=") == 0) return r->severity != val;
        if (strcmp(op, "<") == 0)  return r->severity < val;
        if (strcmp(op, "<=") == 0) return r->severity <= val;
        if (strcmp(op, ">") == 0)  return r->severity > val;
        if (strcmp(op, ">=") == 0) return r->severity >= val;
    }
    else if (strcmp(field, "category") == 0) {
        if (strcmp(op, "==") == 0) return strcmp(r->category, value) == 0;
        if (strcmp(op, "!=") == 0) return strcmp(r->category, value) != 0;
    }
    else if (strcmp(field, "inspector") == 0) {
        if (strcmp(op, "==") == 0) return strcmp(r->inspectorName, value) == 0;
        if (strcmp(op, "!=") == 0) return strcmp(r->inspectorName, value) != 0;
    }
    else if (strcmp(field, "timestamp") == 0) {
        time_t val = (time_t)atoll(value);
        if (strcmp(op, "==") == 0) return r->timestamp == val;
        if (strcmp(op, "!=") == 0) return r->timestamp != val;
        if (strcmp(op, "<") == 0)  return r->timestamp < val;
        if (strcmp(op, "<=") == 0) return r->timestamp <= val;
        if (strcmp(op, ">") == 0)  return r->timestamp > val;
        if (strcmp(op, ">=") == 0) return r->timestamp >= val;
    }

    // Returnăm 0 dacă field-ul sau operatorul sunt invalide
    return 0;
}

void remove_report(const char *district,int raport_id) {
    char path[PATH];
    snprintf(path, sizeof(path),"%s/reports.dat",district);
    int f=open(path,O_RDWR);
    if (f==-1) {
        perror("Eroare deschidere raports.dat la stergere\n");
        return;

    }
    struct stat st;
    if (fstat(f,&st)==-1) {
        perror("Eroare fstat\n");
        return;
    }
    REPORT r;
    int index_de_sters=-1;
    int i=0;

    while (read(f,&r,sizeof(REPORT))>0)
    {
        if (r.id==raport_id)
        {
            index_de_sters=i;
            break;
        }
        i=i+1;
    }

    if (index_de_sters == -1)
    {
        printf("Raportul cu ID %d nu a fost găsit în districtul %s!\n", raport_id, district);
        close(f);
        return;
    }

    int total=st.st_size/sizeof(REPORT);

    for (i=index_de_sters+1;i<total;i++)
    {
        lseek(f,i*sizeof(REPORT),SEEK_SET);
        read(f,&r,sizeof(REPORT));

        lseek(f,(i - 1)*sizeof(REPORT), SEEK_SET);
        write(f,&r,sizeof(REPORT));
    }

    if (ftruncate(f,(total-1)*sizeof(REPORT))==-1)
    {
        perror("Eroare la stergere ultimul rand (cel dublificat)\n");
    }
    close(f);
}

void filter(const char *district,int argc,char *argv[]) {
    char path[PATH];
    snprintf(path, sizeof(path),"%s/reports.dat",district);
    int f=open(path,O_RDONLY);
    if (f==-1) {
        perror("Deschidere fisier prentu functia filter");
        return ;
    }
    REPORT r;
    while (read(f,&r,sizeof(REPORT))>0) {
        int ok=1;
        for (int i=7;i<argc;i++) {
            char field[50],op[50],value[50];
            parse_condition(argv[i],field,op,value);
            if (match_condition(&r,field,op,value)==0) {
                ok=0;
                break;
            }
        }
        if (ok==1) {
            printf("ID: %d\n", r.id);
            printf("Inspector: %s\n", r.inspectorName);
            printf("GPS: %.2f, %.2f\n", r.gps.latitude, r.gps.longitude);
            printf("Categorie: %s\n", r.category);
            printf("Severitate: %d\n", r.severity);
            printf("Timestamp: %s", ctime(&r.timestamp));
            printf("Descriere: %s\n", r.description);
            printf("-------------------\n");
        }
    }
    close(f);
}

void remove_district(const char *district) {
    char link_name[LINK];
    snprintf(link_name, sizeof(link_name),"%s/reports.dat",district);
    unlink(link_name);
    pid_t pid=fork();
    if (pid<0) {
        perror("Eroare fork\n");
        exit(1);
    }
    if (pid==0) {
        execlp("rm","rm","-rf",district,NULL);
        perror("Exec a esuat");
        exit(1);
    }
    wait(NULL);
    printf("District %s removed \n",district);

}


int main(int argc, char *argv[]) {
    if (argc<7) {
        perror("Argumente insuficiente");
        exit(-1);
    }
    char *role=argv[2];
    char *user=argv[4];
    char *command=argv[5];
    char *district=argv[6];
    if (strcmp(role, "inspector") != 0 && strcmp(role, "manager") != 0) {
        printf("Eroare: rol necunoscut '%s'.\n", role);
        return 1;
    }


    // Caile fisierelor
    char path_reports[PATH];
    char path_cfg[PATH];
    char path_log[PATH];
    snprintf(path_reports, sizeof(path_reports), "%s/reports.dat",     district);
    snprintf(path_cfg,     sizeof(path_cfg),     "%s/district.cfg",    district);
    snprintf(path_log,     sizeof(path_log),     "%s/logged_district", district);

    if (strcmp(command, "--add") == 0) {
        create(district);

        if (!check_permission(path_reports, role, 'w')) {
            printf("Eroare: %s nu are drept de scriere\n", role);
            return 1;
        }

        add_report(district, user);
        add_logged_district(district, user, role, "add");
    } else if (strcmp(command, "--list") == 0) {
        if (!check_permission(path_reports, role, 'r')) {
            printf("Eroare: %s nu are drept de citire\n", role);
            return 1;
        }
        list(district);
        add_logged_district(district, user, role, "list");

    } else if (strcmp(command, "--view") == 0) {
        if (argc < 8) {
            printf("--view necesita un <report_id>.\n");
            return 1;
        }
        if (!check_permission(path_reports, role, 'r')) {
            printf("Eroare: %s nu are drept de citire\n", role);
            return 1;
        }
        int report_id = atoi(argv[7]);
        view(district, report_id);
        add_logged_district(district, user, role, "view");
    }else if (strcmp(command,"--update_threshold")==0) {

            if ( strcmp(role,"manager") !=0) {
                fprintf(stderr, "Error: only manager can update threshold\n");
                exit(1);
            }
            if (argc<8) {
                printf("Command %s should contain value of the new threshold.\n",command);
                exit(1);
            }
            if (!check_permission(path_cfg, role, 'w')) {
                printf("Eroare: %s nu are drept de scriere\n", role);
                return 1;
            }
            update_threshold(district,atoi(argv[7]));
            add_logged_district(district,user,role,"update_threshold");
        }else if (strcmp(command, "--remove_report") == 0) {
            if ( strcmp(role,"manager") !=0) {
                fprintf(stderr, "Error: only manager role\n");
                exit(1);
            }
            if (argc < 8) {
                printf("--remove_report necesita un id <raport_id>.\n");
                return 1;
            }
            if (!check_permission(path_reports, role, 'w')) {
                printf("Eroare: %s nu are drept de scriere\n", role);
                 return 1;
            }
            int report_id = atoi(argv[7]);
            remove_report(district, report_id);
            add_logged_district(district,user,role,"remove_report");


    }else if (strcmp(command,"--filter") == 0) {
        if (argc < 8) {
            printf("date insuficiente pentru operatia de filter\n");
            return 1;
        }
        if (!check_permission(path_reports, role, 'r')) {
            printf("Eroare: %s nu are drept de citire\n", role);
            return 1;
        }
        if ( strcmp(role,"manager") ==0) {
            add_logged_district(district,user,role,"filter");
        }
        filter(district,argc,argv);
    }else if (strcmp(command, "--remove_district") == 0) {
        if ( strcmp(role,"manager") !=0) {
            perror("Manager role only");
            exit(1);
        }
        remove_district(district);
    }
    return 0;
}