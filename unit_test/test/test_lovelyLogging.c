#include "unity.h"

#include "lovelyLogging.h"
#include "stdint.h"
#include "string.h"

#define MAX_STRING_LEN 50
#define POST_VALUE 0x55555555
struct test {
    char string_buffer[MAX_STRING_LEN];
    uint32_t post_string_value;
} test;

// fixed test values !
int severity = LOG_INFO;
int line = 1000;
char* filename = "file";
char* message = "my message";


void setUp () {
    memset(test.string_buffer, 0, MAX_STRING_LEN);
    llog_reset_buffer();
}

void tearDown () {
}

void test_empty_log_is_empty(void) {
    TEST_ASSERT_TRUE(llog_is_empty());
}

void test_one_entry_log_is_not_empty(void) {
    llog_add_entry(severity, filename, line, message);
    TEST_ASSERT_FALSE(llog_is_empty());
}

void test_log_entry_after_add_and_read(void) {
    llog_add_entry(severity, filename, line, message);
    llog_next_entry();
    TEST_ASSERT_TRUE(llog_is_empty());
}


void test_add_retrieve_entry_raw(void) {
    llog_add_entry(severity, filename, line, message);
    llog_entry_t entry = llog_next_entry();

    TEST_ASSERT_EQUAL(severity, entry.severity);
    TEST_ASSERT_EQUAL(line, entry.line);
    TEST_ASSERT_EQUAL(filename, entry.file);
    TEST_ASSERT_EQUAL(message, entry.message);
}

void test_add_and_retrieve_entry_string(void) {
    llog_add_entry(severity, filename, line, message);
    llog_next_entry_as_string(test.string_buffer, MAX_STRING_LEN);
    TEST_ASSERT_EQUAL_STRING("\x1B[32;1mINFO:\x1B[30;1m(file:1000)\x1B[0m my message\n\r", test.string_buffer);
}

void test_string_too_long() {
    test.post_string_value = POST_VALUE;
    llog_add_entry(severity, filename, line, "This message is significantly too long for the buffer!");
    llog_next_entry_as_string(test.string_buffer, MAX_STRING_LEN);
    // Did not overwrite any data located next to the char array:
    TEST_ASSERT_EQUAL(POST_VALUE, test.post_string_value);
}

void test_add_multiple_then_read() {
    llog_add_entry(severity, filename, line, "message1");
    llog_add_entry(severity, filename, line, "message2");
    llog_add_entry(severity, filename, line, "message3");
    llog_next_entry_as_string(test.string_buffer, MAX_STRING_LEN);
    TEST_ASSERT_EQUAL_STRING("\x1B[32;1mINFO:\x1B[30;1m(file:1000)\x1B[0m message1\n\r", test.string_buffer);
    llog_next_entry_as_string(test.string_buffer, MAX_STRING_LEN);
    TEST_ASSERT_EQUAL_STRING("\x1B[32;1mINFO:\x1B[30;1m(file:1000)\x1B[0m message2\n\r", test.string_buffer);
    llog_next_entry_as_string(test.string_buffer, MAX_STRING_LEN);
    TEST_ASSERT_EQUAL_STRING("\x1B[32;1mINFO:\x1B[30;1m(file:1000)\x1B[0m message3\n\r", test.string_buffer);
}

void test_read_from_empty() {
    llog_next_entry_as_string(test.string_buffer, MAX_STRING_LEN);
    TEST_ASSERT_EQUAL_STRING("", test.string_buffer);
}
