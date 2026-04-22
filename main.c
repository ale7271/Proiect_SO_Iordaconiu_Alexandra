#include <stdio.h>
<<<<<<< HEAD

int main(void) {
    printf("Hello, World!\n");
    return 0;
=======
#include <sys/stat.h>
#include<unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
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
    symlink(path, link_name);
}


void add_report (const char *district, const char *user)
{
    REPORT r;
    char path [PATH];
    int f;
    create(district);
    printf("Itroduceti coordonatele gps:");
    scanf("%f",&r.gps.latitude);

    //printf("y= ");
    scanf("%f",&r.gps.longitude);

    printf("Introduceeti tipul raportului(road/lighting/flooding/other):");
    scanf("%s",r.category);

    printf("Serverity level (1/2/3): ");
    scanf("%d",&r.severity);

    printf("Descriere:");
    printf("Descriere: ");
    getchar();
    fgets(r.description, DESCRIERE, stdin);


    snprintf(path,sizeof(path),"%s/reports.dat",district);
    f=open(path,O_RDWR,0664);
    struct stat st;
    stat(path,&st);
    r.id=st.st_size/sizeof(REPORT)+1;


    strncpy (r.inspectorName,user,NUME-1);
    r.timestamp=time(NULL);

    lseek (f,0,SEEK_END);
    if (write(f,&r,sizeof(REPORT)) ==-1)
    {
        perror("Raportul nu a putut fi salvat");
    }
    else
    {
        printf("Raportul a fost salvat cu ID-ulL %d\n",r.id);
    }

}
void add_logged_district(const char *district, const char *user, const char *role, const char *action)
{
    char path[PATH];
    char log[LOG];
    int f;
    snprintf(path,sizeof(path),"%s/logged_district",district);
    f=open(path,O_WRONLY | O_CREAT | O_APPEND, 0644);

    time_t now = time(NULL);
    snprintf(log,sizeof(log),"%s\t%ld\t%s\t%s",action,(long)now,role,user);
    write(f,log,sizeof(log));
    close(f);
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
        printf("GPS: %.2f, %.2f\n", r.gps.latitude, r.gps.longitude);
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
    perm[0] = st.st_mode & S_IRUSR ? 'r' : '-';
    perm[1] = st.st_mode & S_IWUSR ? 'w' : '-';
    perm[2] = st.st_mode & S_IXUSR ? 'x' : '-';

    perm[3] = st.st_mode & S_IRGRP ? 'r' : '-';
    perm[4] = st.st_mode & S_IWGRP ? 'w' : '-';
    perm[5] = st.st_mode & S_IXGRP ? 'x' : '-';

    perm[6] = st.st_mode & S_IROTH ? 'r' : '-';
    perm[7] = st.st_mode & S_IWOTH ? 'w' : '-';
    perm[8] = st.st_mode & S_IXOTH ? 'x' : '-';
    perm[9] = '\0';

    printf ("Permissions: %s",perm);
    printf("file size: %ld byte\n",st.st_size);
    printf("Lasr modification: %s",ctime(&st.st_mtime));

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
    lseek(f,(raport_id-1)*sizeof(REPORT),SEEK_SET);
    if (read(f,&r,sizeof(REPORT)) > 0)
    {
        printf("ID: %d\n", r.id);
        printf("Inspector: %s\n", r.inspectorName);
        printf("GPS: %.2f, %.2f\n", r.gps.latitude, r.gps.longitude);
        printf("Categorie: %s\n", r.category);
        printf("Severitate: %d\n", r.severity);
        printf("Timestamp: %s", ctime(&r.timestamp));
        printf("Descriere: %s\n", r.description);
        printf("-------------------\n");
    }
    else
        printf("Raportul cu ID-ul: %d nu exista\n",raport_id);
}

int check_permission (const char *path, const char *role, char actiune) {
    struct stat st;
    if (stat (path,&st) == -1)
    {
        perror ("Eroare stat\n");
        return 0;
    }
    char perm[10];
    perm[0] = st.st_mode & S_IRUSR ? 'r' : '-';
    perm[1] = st.st_mode & S_IWUSR ? 'w' : '-';
    perm[2] = st.st_mode & S_IXUSR ? 'x' : '-';
    perm[3] = st.st_mode & S_IRGRP ? 'r' : '-';
    perm[4] = st.st_mode & S_IWGRP ? 'w' : '-';
    perm[5] = st.st_mode & S_IXGRP ? 'x' : '-';
    perm[6] = st.st_mode & S_IROTH ? 'r' : '-';
    perm[7] = st.st_mode & S_IWOTH ? 'w' : '-';
    perm[8] = st.st_mode & S_IXOTH ? 'x' : '-';
    perm[9] = '\0';

    if (strcmp(role,"manager")==0) {
        if (actiune=='r')
            return perm[0]=='r';
        if (actiune=='w')
            return perm[1]=='w';
    }
    else
    {
        if (actiune=='r')
            return perm[3]=='r';
        if (actiune=='w')
            return perm[4]=='w';
    }
    return 0;
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

    // Verificare symlink cu lstat()
    char link_name[LINK];
    snprintf(link_name, sizeof(link_name), "active_reports-%s", district);
    struct stat lst;
    if (lstat(link_name, &lst) == 0) {
        if (S_ISLNK(lst.st_mode)) {
            char target[PATH];
            snprintf(target, sizeof(target), "%s/reports.dat", district);
            struct stat st;
            if (stat(target, &st) == -1) {
                printf("symlink '%s' a pierdut legatura!\n", link_name);
            }
        }
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

        return 0;
    }
>>>>>>> bbdeeed (functie add,list si view)
}