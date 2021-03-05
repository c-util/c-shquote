/*
 * Tests for Basic Parsing Operations
 * This test does some basic parsing operations and verifies their correctness.
 */

#undef NDEBUG
#include <assert.h>
#include <c-stdaux.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "c-shquote.h"

static void test_quote(void) {
        char buf[1024];
        char *out;
        size_t n_out;
        int r;

        out = NULL;
        n_out = 0;
        r = c_shquote_quote(&out, &n_out, "a", 1);
        c_assert(r == C_SHQUOTE_E_NO_SPACE);

        out = buf;
        n_out = sizeof(buf);
        r = c_shquote_quote(&out, &n_out, "\"", 1);
        c_assert(!r);
        c_assert(out == buf + 3);
        c_assert(n_out == sizeof(buf) - 3);
        c_assert(!memcmp(buf, "'\"'", 3));

        out = buf;
        n_out = sizeof(buf);
        r = c_shquote_quote(&out, &n_out, "'", 1);
        c_assert(!r);
        c_assert(out == buf + 6);
        c_assert(n_out == sizeof(buf) - 6);
        c_assert(!memcmp(buf, "''\\'''", 6));
}

static void test_unquote(void) {
        const char *string = "a\\\n\\b\"\\\"\\$c\\d'\"'e\"''f'";
        char buf[1024];
        char *out = buf;
        size_t n_out = sizeof(buf);
        int r;

        r = c_shquote_unquote(&out, &n_out, string, strlen(string));
        c_assert(!r);

        c_assert(!memcmp(buf, "ab\"$c\\d'e\"f", 10));
}

static void test_reverse(void) {
        const char *string = "a\'b\'\'\'\"c\\\"\"";
        char buf1[1024], buf2[1024];
        char *out;
        const char *in;
        size_t n_out, n_in;
        int r;

        out = buf1;
        n_out = sizeof(buf1);

        r = c_shquote_quote(&out, &n_out, string, strlen(string));
        c_assert(!r);

        in = buf1;
        n_in = sizeof(buf1) - n_out;
        out = buf2;
        n_out = sizeof(buf2);

        r = c_shquote_unquote(&out, &n_out, in, n_in);
        c_assert(!r);

        c_assert(!memcmp(buf2, string, strlen(string)));
}

static void test_parse(void) {
        const char *string = " a ''\"\" 'b c'\nd#e #f\ng\\\nh \t\n";
        char buf[strlen(string)];
        char *out, **argv;
        size_t n_out, argc;
        const char *in = string;
        size_t n_in = strlen(string);
        int r;

        out = buf;
        n_out = sizeof(buf);
        r = c_shquote_parse_next(&out, &n_out, &in, &n_in);
        c_assert(!r);
        c_assert(n_in == strlen(string) - 3);
        c_assert(n_out == sizeof(buf) - 1);
        c_assert(!memcmp(buf, "a", 1));

        r = c_shquote_parse_next(&out, &n_out, &in, &n_in);
        c_assert(!r);
        c_assert(n_in == strlen(string) - 8);
        c_assert(n_out == sizeof(buf) - 1);
        c_assert(!memcmp(buf, "a", 1));

        r = c_shquote_parse_next(&out, &n_out, &in, &n_in);
        c_assert(!r);
        c_assert(n_in == strlen(string) - 14);
        c_assert(n_out == sizeof(buf) - 4);
        c_assert(!memcmp(buf, "ab c", 4));

        r = c_shquote_parse_next(&out, &n_out, &in, &n_in);
        c_assert(!r);
        c_assert(n_in == strlen(string) - 18);
        c_assert(n_out == sizeof(buf) - 7);
        c_assert(!memcmp(buf, "ab cd#e", 7));

        r = c_shquote_parse_next(&out, &n_out, &in, &n_in);
        c_assert(!r);
        c_assert(n_in == strlen(string) - 28);
        c_assert(n_out == sizeof(buf) - 9);
        c_assert(!memcmp(buf, "ab cd#egh", 9));

        r = c_shquote_parse_next(&out, &n_out, &in, &n_in);
        c_assert(C_SHQUOTE_E_EOF);
        c_assert(n_in == strlen(string) - 28);
        c_assert(n_out == sizeof(buf) - 9);
        c_assert(!memcmp(buf, "ab cd#egh", 9));

        r = c_shquote_parse_argv(&argv, &argc, string, strlen(string));
        c_assert(!r);
        c_assert(argc == 5);

        c_assert(!strcmp(argv[0], "a"));
        c_assert(!strcmp(argv[1], ""));
        c_assert(!strcmp(argv[2], "b c"));
        c_assert(!strcmp(argv[3], "d#e"));
        c_assert(!strcmp(argv[4], "gh"));

        free(argv);
}

int main(void) {
        test_quote();
        test_unquote();
        test_reverse();
        test_parse();
        return 0;
}
