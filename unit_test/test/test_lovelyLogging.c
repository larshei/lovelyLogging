#include "unity.h"

#include "lovelyLogging.h"
#include "stdint.h"

#define MAX_STRING_LEN 50
#define POST_VALUE 0x55555555
struct test {
    char string_buffer[MAX_STRING_LEN];
    uint32_t post_string_value;
} test;


void setUp () {
    memset(test.string_buffer, 0, MAX_STRING_LEN);
    llog_reset_buffer();
}

void tearDown () {
}

void test_add_and_retrieve_entry_simple(void) {
    llog_add_entry(LOG_INFO, "file", 24, "my message");
    llog_get_next_entry_as_string(test.string_buffer, MAX_STRING_LEN);
    TEST_ASSERT_EQUAL_STRING("\x1B[32;1mINFO:\x1B[30;1m(file:24)\x1B[0m my message\r\n", test.string_buffer);
}

void test_string_too_long() {
    test.post_string_value = POST_VALUE;
    llog_add_entry(LOG_INFO, "file", 24, "This message is significantly too long");
    llog_get_next_entry_as_string(test.string_buffer, MAX_STRING_LEN);
    // Did not overwrite any data located next to the char array:
    TEST_ASSERT_EQUAL(POST_VALUE, test.post_string_value);
}

void test_add_multiple_then_read() {
    llog_add_entry(LOG_INFO, "file", 24, "my message1");
    llog_add_entry(LOG_INFO, "file", 24, "my message2");
    llog_add_entry(LOG_INFO, "file", 24, "my message3");
    llog_get_next_entry_as_string(test.string_buffer, MAX_STRING_LEN);
    TEST_ASSERT_EQUAL_STRING("\x1B[32;1mINFO:\x1B[30;1m(file:24)\x1B[0m my message1\r\n", test.string_buffer);
    llog_get_next_entry_as_string(test.string_buffer, MAX_STRING_LEN);
    TEST_ASSERT_EQUAL_STRING("\x1B[32;1mINFO:\x1B[30;1m(file:24)\x1B[0m my message2\r\n", test.string_buffer);
    llog_get_next_entry_as_string(test.string_buffer, MAX_STRING_LEN);
    TEST_ASSERT_EQUAL_STRING("\x1B[32;1mINFO:\x1B[30;1m(file:24)\x1B[0m my message3\r\n", test.string_buffer);
}

void test_read_from_empty() {
    llog_get_next_entry_as_string(test.string_buffer, MAX_STRING_LEN);
    TEST_ASSERT_EQUAL_STRING("", test.string_buffer);
}
