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
#include<signal.h>
#define NUME 50
#define CATEGORIE 30
#define DESCRIERE 100
#define PATH 256
#define LINK 200
#define LOG 200

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
