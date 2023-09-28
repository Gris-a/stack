#ifndef LOG_H
#define LOG_H

/**
 * @file log.h
 * @author GraY
 * @brief Functions for logging.
 */

#include <stdio.h>

/**
 * @brief Opens log file in "w" mode.
 * @return FILE* pointer to FILE for logging.
 */
FILE *open_log(void);

/**
 * @brief Called at exit for log-file closing.
 */
void close_log(void);

#endif //LOG_H