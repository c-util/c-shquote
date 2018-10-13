/*
 * Tests for Public API
 * This test, unlikely the others, is linked against the real, distributed,
 * shared library. Its sole purpose is to test for symbol availability.
 */

#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "c-shquote.h"

static void test_api(void) {
}

int main(int argc, char **argv) {
        test_api();
        return 0;
}
