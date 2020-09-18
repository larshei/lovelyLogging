/* ============== FIFO BUFFER ============================================== */
/* After writing another small fifo buffers the 100th time, I created this gist.
 * It is a QUICK AND DIRTY solution to get the standard functions of a fifo, namely
 * - add and read elements
 * - check if empty
 * - reset
 *
 * ============== USAGE ======================================================
 * 1. #define the LQC_BUFFER_DATA_TYPE, LQC_BUFFER_SIZE and LQC_BUFFER_MEMORY.
 * 2. Optionally: define LQC_BUFFER_OVERWRITE_WHEN_FULL for ring buffer like behaviour
 * 3. include this .c-file in your .c-file (yep, thats dirty).
 * All functions are static, so in theory, you could include this in every 
 * file with a different setup and embrace the code duplication.
 *
 * ============== EXAMPLE ===================================================
 * #define LQC_BUFFER_DATA_TYPE int
 * #define LQC_BUFFER_SIZE 10
 * LQC_BUFFER_DATA_TYPE buffer[LQC_BUFFER_SIZE];
 * #define LQC_BUFFER_MEMORY buffer
 * #define LQC_BUFFER_OVERWRITE_WHEN_FULL 0
 * 
 * #include "lqc_buffer_buffer.c"
 
 * ============== ADDITIONAL INFO ===========================================
 * ALL OF THE DEFINES ABOVE WILL BE UNDEFINED IN lqc_buffer_buffer.c
 *
 * If you need LQC_BUFFER_SIZE in your program, use a 2-step approach:
 * #define MY_BUF_SIZE  10
 * #define LQC_BUFFER_SIZE MY_BUF_SIZE
 * use MY_BUF_SIZE in your program.
 * ========================================================================== */

// fifo buffer code
#ifndef LQC_BUFFER_DATA_TYPE
#error "Need to define LQC_BUFFER_DATA_TYPE (data type of elements stored)"
#endif
#ifndef LQC_BUFFER_MEMORY
#error "Need to define LQC_BUFFER_MEMORY (pointer to your buffer memory/array)"
#endif
#ifndef LQC_BUFFER_SIZE
#error "Need to define LQC_BUFFER_SIZE (number of LQC_BUFFER_DATA_TYPE elements that can be stored in LQC_BUFFER_MEMORY"
#endif
#ifndef LQC_BUFFER_OVERWRITE_WHEN_FULL
#define LQC_BUFFER_OVERWRITE_WHEN_FULL 0
#endif

// ----------------------------------------------------------------------------
typedef struct {
    LQC_BUFFER_DATA_TYPE* head;
    LQC_BUFFER_DATA_TYPE* tail;
    LQC_BUFFER_DATA_TYPE* first;
    int max_element_count;
    int is_empty;
} lqc_buffer_t;
// ----------------------------------------------------------------------------
static lqc_buffer_t _lqc_buffer_buffer = {
    .head = LQC_BUFFER_MEMORY,
    .tail = LQC_BUFFER_MEMORY,
    .first = LQC_BUFFER_MEMORY,
    .max_element_count = sizeof(LQC_BUFFER_MEMORY) / sizeof(LQC_BUFFER_DATA_TYPE),
    .is_empty = 1
};
// ----------------------------------------------------------------------------
static void lqc_buffer_wrap(LQC_BUFFER_DATA_TYPE** head_or_tail) {
    int needs_wrap = *head_or_tail >= _lqc_buffer_buffer.first + _lqc_buffer_buffer.max_element_count;
    if ( needs_wrap ) {
        *head_or_tail = _lqc_buffer_buffer.first;
    }
}
// ----------------------------------------------------------------------------
static void lqc_buffer_reset() {
    _lqc_buffer_buffer.head = _lqc_buffer_buffer.first;
    _lqc_buffer_buffer.tail = _lqc_buffer_buffer.first;
    _lqc_buffer_buffer.is_empty = 1;
}
// ----------------------------------------------------------------------------
static int lqc_buffer_add_element(LQC_BUFFER_DATA_TYPE element) {
    if ((_lqc_buffer_buffer.is_empty == 0) && (_lqc_buffer_buffer.head == _lqc_buffer_buffer.tail)) {
        #if defined(LQC_BUFFER_OVERWRITE_WHEN_FULL) && (LQC_BUFFER_OVERWRITE_WHEN_FULL != 0)
            _lqc_buffer_buffer.tail++;
            lqc_buffer_wrap(&_lqc_buffer_buffer.tail);
        #else
            return 1;
        #endif
    }
    *_lqc_buffer_buffer.head++ = element;
    lqc_buffer_wrap(&_lqc_buffer_buffer.head);

    _lqc_buffer_buffer.is_empty = 0;
    return 0;
}
// ----------------------------------------------------------------------------
static int lqc_buffer_is_empty() {
    return _lqc_buffer_buffer.is_empty;
}
// ----------------------------------------------------------------------------
static LQC_BUFFER_DATA_TYPE lqc_buffer_read_element() {
    // check lqc_buffer_is_empty() before reading! As the data type stored in the
    // buffer is not known, there is no default value that can be used to mark
    // the read result als "invalid because empty".
    #warning "!! lqc_buffer_read_element() does not check if the fifo is empty! Call lqc_buffer_is_empty() and read only if not empty !!"

    LQC_BUFFER_DATA_TYPE entry;
    entry = *_lqc_buffer_buffer.tail++;

    lqc_buffer_wrap(&_lqc_buffer_buffer.tail);

    _lqc_buffer_buffer.is_empty = _lqc_buffer_buffer.tail == _lqc_buffer_buffer.head;
    return entry;
}
// ----------------------------------------------------------------------------
#undef LQC_BUFFER_SIZE
#undef LQC_BUFFER_DATA_TYPE
#undef LQC_BUFFER_MEMORY
#undef LQC_BUFFER_OVERWRITE_WHEN_FULL
// ----------------------------------------------------------------------------
// =========== end of fifo buffer code ===========
