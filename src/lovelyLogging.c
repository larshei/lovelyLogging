/*
 * logging.c
 *
 *  Created on: 15 Sep 2020
 *      Author: larsheinrichs
 * 
 * LOGGING functionality, hardware independent.
 * Provides a log buffer and formatting functionality.
 * Provides different logging levels.
 * Requires an actual output to be configured.
 */

#include "lovelyLogging.h"

// +++ Import fifo/ring buffer ++++++++++++++++++++++++++++++++++++++++++++++++
#define LQC_BUFFER_SIZE LLOG_BUFFER_ELEMENT_COUNT
#define LQC_BUFFER_DATA_TYPE llog_entry_t
static LQC_BUFFER_DATA_TYPE log_entries[LQC_BUFFER_SIZE];
#define LQC_BUFFER_MEMORY log_entries
#define LQC_BUFFER_OVERWRITE_WHEN_FULL LQC_BUFFER_OVERWRITE_OLDEST

#include "lqc_buffer.c"
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include <stdio.h>

// --- LOG MESSAGE PRINTOUTS --------------------------------------------------
// a message for log level `serverity` will be a concatenation of
// log_level_strings[severity] + log_file_line_string + log_message_string
// \x1b[__: escape characters. ..31 red, 32 green, 33 yellow, 34 blue, 0m reset
const char* log_number_string = "%4u - ";
const char* log_level_strings[LOG_LEVEL_COUNT] = {
    "\x1b[31;1mCRITICAL:",
    "\x1b[31;1mERROR:",
    "\x1b[33;1mWARNING:",
    "\x1b[32;1mINFO:",
    "\x1b[34;1mDEBUG:"
};
const char* log_file_line_string = "\x1b[30;1m(%s:%u)";
const char* log_message_string = "\x1b[0m %s\n\r";
static int log_entry_number = 1;


int llog_add_entry(llog_severity_t severity, char* file, int line, char* msg) {
    if (severity > LLOG_VERBOSITY) {
        return 0;
    }
    llog_entry_t entry;
    entry.number = log_entry_number++;
    entry.severity = severity;
    entry.file = file;
    entry.line = line;
    entry.message = msg;
    return lqc_buffer_add_element(entry);
}

int llog_next_entry_as_string( char* buffer, int max_length ) {
    int length = 0;
    llog_entry_t entry;

    if (!lqc_buffer_is_empty()) {
        entry = lqc_buffer_read_element();
        length = llog_create_string_from_entry(buffer, max_length, entry);
    }
    *(buffer + max_length - 1) = '\0'; // \0 terminate in any case
    return length;
}

llog_entry_t llog_next_entry() {
    return lqc_buffer_read_element();
}

int llog_is_empty() {
    return lqc_buffer_is_empty();
}

void llog_reset_buffer() {
    lqc_buffer_reset();
    log_entry_number = 1;
}

unsigned int llog_create_string_from_entry(char* buffer, unsigned int max_length, llog_entry_t entry) {
    char* cursor = buffer;
    char* log_level_string;
    int remaining_length, write_result;

    int valid_severity = (entry.severity < LOG_LEVEL_COUNT) && (entry.severity >= 0);
    if (valid_severity) {
        log_level_string = log_level_strings[entry.severity];
    } else {
        log_level_string = "";
        return 0;
    }

    // snprintf returns either the number of written bytes
    // or the number of bytes it WOULD HAVE NEEDED to write everything,
    // meaning its return value is bigger than the given max count.
    // so we need to check the return value after every write.
    // Alternative:
    // Passing 4 variables + va_list to a function and using the return value
    // to indicate if the buffer end has been reached.
    #define write_to_buffer(string, ...)                  \
    remaining_length = max_length - (cursor - buffer);  \
    write_result = snprintf( cursor,                    \
                        remaining_length,               \
                        string,                         \
                        __VA_ARGS__                     \
                      );                                \
    if (write_result >= remaining_length) {             \
        return max_length;                              \
    } else {                                            \
        cursor += write_result;                         \
    }                                                   \

    write_to_buffer(log_number_string, entry.number);
    write_to_buffer(log_level_string, 0); // dummy 0, __VA_ARGS__ cant be empty
    write_to_buffer(log_file_line_string, entry.file, entry.line);
    write_to_buffer(log_message_string, entry.message);

    return (cursor - buffer);
}
