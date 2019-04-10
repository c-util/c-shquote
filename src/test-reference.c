/*
 * Tests for glib and /bin/sh compatibility
 */

#undef NDEBUG
#include <assert.h>
#include <c-stdaux.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "c-shquote.h"

static void test_quote_one(const char *string) {
        char c_string[1024] = {};
        char *out = c_string;
        size_t n_out = sizeof(c_string);
        gchar *glib_string;
        int r;

        glib_string = g_shell_quote(string);
        c_assert(glib_string);

        r = c_shquote_quote(&out, &n_out, string, strlen(string));
        c_assert(!r);

        c_assert(strlen(glib_string) == sizeof(c_string) - n_out);
        c_assert(!strncmp(glib_string, c_string, strlen(glib_string)));

        g_free(glib_string);
}

static void test_unquote_one(const char *string) {
        char c_string[1024];
        char *out = c_string;
        size_t n_out = sizeof(c_string);
        gchar *glib_string;
        GError *error = NULL;
        int r;

        glib_string = g_shell_unquote(string, &error);
        r = c_shquote_unquote(&out, &n_out, string, strlen(string));

        if (r || !glib_string) {
                c_assert(!glib_string);
                c_assert(r == C_SHQUOTE_E_BAD_QUOTING);
                c_assert(error->code == G_SHELL_ERROR_BAD_QUOTING);
                g_error_free(error);
        } else {
                c_assert(strlen(glib_string) == sizeof(c_string) - n_out);
                c_assert(!strncmp(glib_string, c_string, strlen(glib_string)));

                g_free(glib_string);
        };
}

static void test_parse_one(const char *in) {
        gchar **g_argv;
        gint g_argc;
        gboolean success;
        char **c_argv;
        size_t c_argc;
        GError *error = NULL;
        int r;

        success = g_shell_parse_argv(in, &g_argc, &g_argv, &error);
        r = c_shquote_parse_argv(&c_argv, &c_argc, in, strlen(in));

        if (r || !success) {
                c_assert(!success);
                if (!r) {
                        c_assert(c_argc == 0);
                        c_assert(error->code == G_SHELL_ERROR_EMPTY_STRING);
                } else {
                        c_assert(r == C_SHQUOTE_E_BAD_QUOTING);
                        c_assert(error->code == G_SHELL_ERROR_BAD_QUOTING);
                }
                g_error_free(error);
        } else {
                c_assert(g_argc == c_argc);

                for (int i = 0; i < g_argc; ++i)
                        c_assert(!strcmp(g_argv[i], c_argv[i]));

                free(c_argv);
                g_strfreev(g_argv);
        }
}

static void test_quote(void) {
        test_quote_one("");
        test_quote_one("foo");
        test_quote_one("'");
        test_quote_one("''");
        test_quote_one("foo'bar");
}

static void test_unquote(void) {
        test_unquote_one("");
        test_unquote_one("foo");
        test_unquote_one("\\");
        test_unquote_one("\\\n");
        test_unquote_one("\"\\\"\\\\\\$\\`\\\n\"''");
}

static void test_parse(void) {
        test_parse_one("foo");
        test_parse_one(" foo");
        test_parse_one("foo\n");
        test_parse_one("a\t\nb");
        test_parse_one("a\t\\\n''\"\"\nb");
        test_parse_one("#foobar foo");
        test_parse_one("foobar #foo");
        test_parse_one("/usr/libexec/at-spi-bus-launcher");
}

int main(int argc, char *argv[]) {
        test_quote();
        test_unquote();
        test_parse();
        return 0;
}
