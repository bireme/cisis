#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#ifdef WIN32
#include <io.h>
#include <sys\stat.h>
#else /* WIN32 */
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif /* WIN32 */

struct timeval begin;
struct timeval end;
double t1;
double t2;
int handle;

int chrono_init(char * fname) {
    if (fname) {
        handle = open(fname, O_CREAT|O_APPEND|O_WRONLY,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
        if (handle == -1) {
            printf("chrono_init/file open error\n");
            return 0;
        }
    }

    return 1;
}

int chrono_end() {
    if (handle) {
        return close(handle);
    }

    return 0;
}

void chrono_start() {
    char mess[2048];

    gettimeofday(&begin, NULL);
}

long chrono_stop(char * message) {
    long dif;
    char mess[2048];

    gettimeofday(&end, NULL);

    dif = (long)(((end.tv_sec - begin.tv_sec) * 1000000) + (end.tv_usec - begin.tv_usec))/1000;

//printf("debug sec=%ld msec=%ld\n", (long)((end.tv_sec - begin.tv_sec) * 1000000), (long)(end.tv_usec - begin.tv_usec));
    if (!message) {
        message = "";
    }
    sprintf(mess, "%s[%ld]\n", message, dif);

    begin.tv_sec = end.tv_sec;
    begin.tv_usec = end.tv_usec;

    if (handle) {
        if (write(handle, mess, strlen(mess)) == 1) {
            printf("chrono_stop/write file error\n");
            return 0;
        }
    } else {
        printf(mess);
    }

//printf(mess);

    return dif;
}
