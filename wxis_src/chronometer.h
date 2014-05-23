#ifndef CHRONO_H
#define CHRONO_H

int chrono_init(char * fname);
int chrono_end();
void chrono_start();
long chrono_stop(char * message);

#endif /* CHRONO_H */
