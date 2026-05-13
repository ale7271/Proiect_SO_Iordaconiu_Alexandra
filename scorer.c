#include"structura.h"
typedef struct {
    char nume[NUME];
    int scor;
}SCOR;
#define MAX_INSPECTORI 100
int main(inr argc,char *argv[]) {
    if (argc<2) {
        printf("Trebuie introdus cel putin 1 district");
        exit(0);
    }
    SCOR v[MAX_INSPECTORI];
    char path[PATH];
    snprintf(path,PATH,"%s/reports.dat",argv[1]);
    int fin=open(path,O_RDONLY);
    if (fin==-1) {
        printf("Nu a putut fi accesat districtul");
        exit(0);
    }
    int n=0;//Nr de inspectori
    REPORT buffer;
    while (read(fin,&buffer,sizeof(buffer))>0) {
        int ok=0;
        for (int i=0;i<n;i++) {
            if (strcmp(v[i].nume,buffer.nume)==0) {
                v[i].scor=v[i].scor+buffer.severity;
                ok=1;
                break;
            }

        }
        if (ok==0 && n<MAX_INSPECTORI) {
            strcpy(v[n].nume, buffer.inspectorName);
            v[n].scor = buffer.severity;
            n++;
        }
    }
    close(f);
    printf("Raport District: %s \n", argv[1]);
    for (int i=0;i<n;i++) {
        printf("Inspector: %s - scor total severitate: %d\n", v[i].nume, v[i].scor);
    }

    return 0;

}