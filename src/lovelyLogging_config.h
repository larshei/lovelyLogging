/*
 * logging_config.h
 *
 *  Created on: 15 Sep 2020
 *      Author: Lars Heinrichs
 */

#ifndef LOGGING_LOVELYLOGGING_CONFIG_H_
#define LOGGING_LOVELYLOGGING_CONFIG_H_

// --- LOG BEHAVIOUR CONFIG ---------------------------------------------------
// see llog_severity_t enum, all levels <= LOG_LEVEL will be printed.
#define LLOG_VERBOSITY LOG_DEBUG 

// Buffer size for logging. each log message is one element
#define LLOG_BUFFER_ELEMENT_COUNT 15

// Buffer output array size, number of max allowed chars per message
#define LLOG_CHAR_BUF_SIZE 200

// Should messages be copied by lovelyLogging and stored in its own buffer?
// This is useful if the log messages is a locally allocated message, otherwise
// the message might be printed empty.
// Requires LLOG_CHAR_BUF_SIZE*(LLOG_BUFFER_ELEMENT_COUNT-1) bytes of
// additional memory.
#define LLOG_MSG_COPY_ENABLE 1

// when buffer is full:
// 0: dont overwrite, deny new entries
// !0: overwrite oldest entry
#define LLOG_OVERWRITE_OLDEST 0

#endif /* LOGGING_LOVELYLOGGING_CONFIG_H_ */
