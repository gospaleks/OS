#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define KEY 10101
#define DUZINA_LINIJE 256

struct BaferPoruke
{
    long tip;
    char linija[DUZINA_LINIJE];
};