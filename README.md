# lovelyLogging

![Unit Tests](https://github.com/larshei/lovelyLogging/workflows/Unit%20Tests/badge.svg)

Simple Logging abstraction for C.

- One file include, almost no setup
- Create log entries of different severity/verbosity levels
- Read raw log entries (e.g. for persisting)
- Read log entries as formated strings

## Setup

Configure `lovelyLogging_config.h` as desired and include `lovelyLogging.h` in
your application.

If you are not happy with the default string format, adjust the template
strings in lovelyLogging.c.

## Usage Example

### Create a log entry:

``` C
#include "lovelyLogging.h"

log_new(LOG_INFO, "This is an info message");
```

### Retrieve log entry as raw data
``` C
if (0 == llog_is_empty()) {
    llog_entry_t entry = llog_next_entry_raw();
    /* use entry here, e.g. store in flash memory */
}
```

### Retrieve log entry as formatted string
``` C
// provide a buffer for the formatted output string
#define BUFFER_LEN 100
char printout_buffer[BUFFER_LEN];

int bytes_written = llog_next_entry_as_string(printout_buffer, BUFFER_LEN);
```
Getting an entry as a string will return
1. a preformated string, `\n\r\0` terminated.
2. `"\0"` if the log is empty

Console output of the formated string will look similar to:

 <span style="color:green">   1 - INFO:</span><span
 style="color:grey">(path_to/my_file.c:24)</span> This is an info message

### Logging local strings

When trying to log local strings, the log messages might appear empty. This is
because lovelyLogging stores only a pointer to the message by default. The
original message created locally in the calling function is probably not
available anymore.

In this case, lovelyLogging allows to copy over messages to its own buffer to
make sure they do not get lost, at the cost of additional
`LLOG_BUFFER_ELEMENT_COUNT * (LLOG_CHAR_BUF_SIZE - 1)` bytes of memory. The
behaviour is controlled using the `LLOG_MSG_COPY_ENABLE` define.
