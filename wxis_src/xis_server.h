#ifndef XIS_SERVER_H
#define XIS_SERVER_H

typedef struct {
    char* buffer;
    int totalSize;
    int textSize;
    int freeSize;
} OUT_BUFFER;

int writeBuff(char *mess,
              OUT_BUFFER *outBuff,
              int append);
void flushBuff(OUT_BUFFER *outBuff);
#endif /* XIS_SERVER_H */
