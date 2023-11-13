#ifndef LOG_H
#define LOG_H

/**
 * @file log.h
 * @author GraY
 * @brief Functions for logging.
 */

#include <stdio.h>

#define PROTECT

#ifndef LOG_CPP
extern FILE *LOG_FILE; //< Automaticly externs log-file when included.
#endif

#define LOG(...) fprintf(LOG_FILE, __VA_ARGS__)
#define LOGS(string) fputs(string, LOG_FILE)

#ifdef PROTECT
/**
 * @brief ifdef PROTECT checks condition and writes result into log-file.
 */
#define ASSERT(condition, action)   if(!(condition))\
                                    {\
                                        LOG ("%s:%s:%d: Assertion cathced at ASSERT(", __FILE__, __PRETTY_FUNCTION__, __LINE__);\
                                        LOGS(#condition);\
                                        LOG (", ...);\n");\
                                        \
                                        action;\
                                    }
#else
#define ASSERT(...)
#endif

/**
 * @brief Opens log-file in "wb" mode with no buffering.
 * @return FILE* pointer to FILE for logging.
 * returns stderr if can`t open log-file.
 */
FILE *open_log(void);

/**
 * @brief Called automaticly by atexit() for log-file closing.
 */
void close_log(void);

#endif //LOG_H