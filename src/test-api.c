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
        char *out = NULL;
        size_t n_out = 0;
        const char *in = NULL;
        size_t n_in = 0;
        char **argv;
        size_t argc;
        int r;

        r = c_shquote_quote(&out, &n_out, NULL, 0);
        assert(r == C_SHQUOTE_E_NO_SPACE);

        r = c_shquote_unquote(&out, &n_out, "'", 1);
        assert(r == C_SHQUOTE_E_BAD_QUOTING);

        r = c_shquote_parse_next(&out, &n_out, &in, &n_in);
        assert(r == C_SHQUOTE_E_EOF);

        r = c_shquote_parse_argv(&argv, &argc, "foo", strlen("foo"));
        fprintf(stderr, "%d\n", r);
        assert(!r);
        assert(argc == 1);
        assert(!strcmp(argv[0], "foo"));

        free(argv);
}

int main(int argc, char **argv) {
        test_api();
        return 0;
}
