/*
 * logging.h
 *
 *  Created on: 15 Sep 2020
 *      Author: larsheinrichs
 */

#ifndef LOGGING_LOVELYLOGGING_H_
#define LOGGING_LOVELYLOGGING_H_


#include "lovelyLogging_config.h"

// --- LOG VERBOSITY LEVELS ---------------------------------------------------
// dont touch this :)
#define LOG_LEVELS(...) typedef enum llog_severity_t {__VA_ARGS__, LOG_LEVEL_COUNT} llog_severity_t;
// High to low priority
LOG_LEVELS(
    LOG_CRITICAL,
    LOG_ERROR,
    LOG_WARNING,
    LOG_INFO,
    LOG_DEBUG
)

#ifndef LLOG_VERBOSITY
#define LLOG_VERBOSITY LOG_LEVEL_COUNT
#endif

typedef struct {
    llog_severity_t severity;
    int line;
    int number;
    char* file;
    char* message;
} llog_entry_t;

#define llog_new(severity, msg) llog_add_entry(severity, __FILE__, __LINE__, msg)

void llog_reset_buffer();
int llog_is_empty();
int llog_add_entry(llog_severity_t severity, char* file, int line, char* msg);
int llog_next_entry_as_string( char* buffer, int max_length);
llog_entry_t llog_next_entry_raw();
unsigned int llog_create_string_from_entry(char* buffer, unsigned int max_length, llog_entry_t entry);

#endif /* LOGGING_LOVELYLOGGING_H_ */
