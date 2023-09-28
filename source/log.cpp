/**
 * @file log.cpp
 * @author GraY
 * @brief Logging functions and globals definitions.
 */

#include <stdio.h>
#include <stdlib.h>

#include "../include/log.h"

FILE *LOG_FILE = open_log();

FILE *open_log(void)
{
    atexit(close_log);

    FILE *file = fopen("stack.log", "w");
    if(file == NULL)
    {
        printf("Can`t open log-file.\n");

        return stdout;
    }

    return file;
}

void close_log(void)
{
    fflush(LOG_FILE);
    fclose(LOG_FILE);
}