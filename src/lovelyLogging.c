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
    *(buffer + max_length) = '\0'; // \0 terminate in any case
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

int llog_create_string_from_entry(char* buffer, unsigned int max_length, llog_entry_t entry) {
    char* cursor = buffer;
    char* log_level_string;

    int valid_severity = (entry.severity < LOG_LEVEL_COUNT) && (entry.severity >= 0);
    log_level_string = valid_severity ? log_level_strings[entry.severity] : "";
    
    cursor += snprintf( cursor,
                        max_length - (cursor - buffer), 
                        log_number_string,
                        entry.number
                      );
    cursor += snprintf( cursor,
                        max_length - (cursor - buffer),
                        log_level_string
                      );
    cursor += snprintf( cursor,
                        max_length - (cursor - buffer), 
                        log_file_line_string,
                        entry.file,
                        entry.line
                      );
    cursor += snprintf( cursor,
                        max_length - (cursor - buffer), 
                        log_message_string,
                        entry.message
                      );

    return (cursor - buffer);
}
