#ifndef LOG_H
#define LOG_H

/**
 * @file log.h
 * @author GraY
 * @brief Functions for logging.
 */

#include <stdio.h>


#ifndef LOG_CPP
extern FILE *LOG_FILE;
#endif

/**
 * @brief Macro for logging.
 */
#define LOG(...) fprintf(LOG_FILE, __VA_ARGS__)

/**
 * @brief Macro for checking condition and writing result into log file.
 */
#define ASSERT(condition, action)   if(!(condition))\
                                    {\
                                        LOG("%s:%s:%d\n"\
                                            "\tAssertion cathced at ASSERT(" #condition "...);\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);\
                                        action;\
                                    }
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
