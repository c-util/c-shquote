/*
 * POSIX Shell Compatible Argument Parser
 *
 * For highlevel documentation of the API see the header file and the docbook
 * comments.
 */

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "c-shquote.h"
#include "c-shquote-private.h"

int c_shquote_append_str(char **outp,
                         size_t *n_outp,
                         const char *in,
                         size_t n_in) {
        if (n_in > *n_outp)
                return C_SHQUOTE_E_NO_SPACE;

        memcpy(*outp, in, n_in);

        *outp += n_in;
        *n_outp -= n_in;

        return 0;
}

int c_shquote_append_char(char **outp,
                          size_t *n_outp,
                          char in) {
        return c_shquote_append_str(outp, n_outp, &in, 1);
}

void c_shquote_skip_str(const char **strp,
                        size_t *n_strp,
                        size_t len) {
        assert(len <= *n_strp);

        *strp += len;
        *n_strp -= len;
}

void c_shquote_skip_char(const char **strp,
                         size_t *n_strp) {
        c_shquote_skip_str(strp, n_strp, 1);
}

int c_shquote_consume_str(char **outp,
                          size_t *n_outp,
                          const char **inp,
                          size_t *n_inp,
                          size_t len) {
        int r;

        if (len > *n_inp)
                return -ENOTRECOVERABLE;

        r = c_shquote_append_str(outp, n_outp, *inp, len);
        if (r)
                return r;

        c_shquote_skip_str(inp, n_inp, len);

        return 0;
}

int c_shquote_consume_char(char **outp,
                           size_t *n_outp,
                           const char **inp,
                           size_t *n_inp) {
        return c_shquote_consume_str(outp, n_outp, inp, n_inp, 1);
}

size_t c_shquote_strnspn(const char *string,
                        size_t n_string,
                        const char *accept) {
        bool buffer[UCHAR_MAX] = {};

        for ( ; *accept; ++accept)
                buffer[(unsigned char)*accept] = true;

        for (size_t i = 0; i < n_string; ++i)
                if (!buffer[(unsigned char)string[i]])
                        return i;

        return n_string;
}

size_t c_shquote_strncspn(const char *string,
                          size_t n_string,
                          const char *reject) {
        bool buffer[UCHAR_MAX] = {};

        if (strlen(reject) == 1) {
                const char *p;

                p = memchr(string, reject[0], n_string);
                if (!p)
                        return n_string;
                else
                        return p - string;
        }

        for ( ; *reject; ++reject)
                buffer[(unsigned char)*reject] = true;

        for (size_t i = 0; i < n_string; ++i)
                if (buffer[(unsigned char)string[i]])
                        return i;

        return n_string;
}

void c_shquote_discard_comment(const char **inp,
                               size_t *n_inp) {
        size_t len;

        assert(**inp == '#');

        /* Skip up-to, but excluding, the next newline. */
        len = c_shquote_strncspn(*inp, *n_inp, "\n");
        c_shquote_skip_str(inp, n_inp, len);
}

void c_shquote_discard_whitespace(const char **inp,
                                  size_t *n_inp) {
        size_t len;

        /* Skip until the next non-whitespace character. */
        len = c_shquote_strnspn(*inp, *n_inp, " \t\n");
        c_shquote_skip_str(inp, n_inp, len);
}

int c_shquote_unescape_char_quoted(char **outp,
                                   size_t *n_outp,
                                   const char **inp,
                                   size_t *n_inp) {
        const char *in = *inp;
        size_t n_in = *n_inp;
        int r;

        if (n_in == 0 || *in != '\\')
                return -ENOTRECOVERABLE;

        if (n_in == 1)
                return C_SHQUOTE_E_BAD_QUOTING;

        switch (in[1]) {
        case '"':
        case '\\':
        case '`':
        case '$':
        case '\n':
                /*
                 * Skip the '\' and append the escaped character
                 * verbatim.
                 */
                c_shquote_skip_char(&in, &n_in);

                r = c_shquote_consume_char(outp, n_outp, &in, &n_in);
                if (r)
                        return r;

                break;
        default:
                /*
                 * Append both the '\' and the following (non-escaped)
                 * character verbatim.
                 */
                r = c_shquote_consume_str(outp, n_outp, &in, &n_in, 2);
                if (r)
                        return r;

                break;
        }

        *inp = in;
        *n_inp = n_in;
        return 0;
}

int c_shquote_unescape_char_unquoted(char **outp,
                                     size_t *n_outp,
                                     const char **inp,
                                     size_t *n_inp) {
        const char *in = *inp;
        size_t n_in = *n_inp;
        int r;

        if (n_in == 0 || *in != '\\')
                return -ENOTRECOVERABLE;

        c_shquote_skip_char(&in, &n_in);

        if (n_in > 0) {
                if (*in == '\n') {
                        c_shquote_skip_char(&in, &n_in);
                } else {
                        r = c_shquote_consume_char(outp, n_outp, &in, &n_in);
                        if (r)
                                return r;
                }
        }

        *inp = in;
        *n_inp = n_in;
        return 0;
}

int c_shquote_unquote_single(char **outp,
                             size_t *n_outp,
                             const char **inp,
                             size_t *n_inp) {
        const char *in = *inp;
        size_t n_in = *n_inp;
        size_t len;
        int r;

        if (n_in == 0 || *in != '\'')
                return -ENOTRECOVERABLE;

        c_shquote_skip_char(&in, &n_in);

        len = c_shquote_strncspn(in, n_in, "'");
        if (len == n_in)
                return C_SHQUOTE_E_BAD_QUOTING;

        r = c_shquote_consume_str(outp, n_outp, &in, &n_in, len);
        if (r)
                return r;

        c_shquote_skip_char(&in, &n_in);

        *inp = in;
        *n_inp = n_in;
        return 0;
}

int c_shquote_unquote_double(char **outp,
                             size_t *n_outp,
                             const char **inp,
                             size_t *n_inp) {
        char *out = *outp;
        size_t n_out = *n_outp;
        const char *in = *inp;
        size_t n_in = *n_inp;
        int r;

        if (n_in == 0 || *in != '\"')
                return -ENOTRECOVERABLE;

        c_shquote_skip_char(&in, &n_in);

        while (n_in > 0) {
                size_t len;

                switch (*in) {
                case '\\':
                        r = c_shquote_unescape_char_quoted(&out, &n_out, &in, &n_in);
                        if (r)
                                return r;

                        break;
                case '\"':
                        c_shquote_skip_char(&in, &n_in);

                        goto out;
                default:
                        /*
                         * Consume until the next escape sequence or the next double
                         * quote. If none exists, consume the rest of the string.
                         */
                        len = c_shquote_strncspn(in, n_in, "\\\"");
                        r = c_shquote_consume_str(&out, &n_out, &in, &n_in, len);
                        if (r)
                                return r;
                }
        }

        return C_SHQUOTE_E_BAD_QUOTING;
out:
        *outp = out;
        *n_outp = n_out;
        *inp = in;
        *n_inp = n_in;
        return 0;
}

_public_ int c_shquote_quote(char **outp,
                             size_t *n_outp,
                             const char *in,
                             size_t n_in) {
        char *out = *outp;
        size_t n_out = *n_outp;
        int r;

        r = c_shquote_append_char(&out, &n_out, '\'');
        if (r)
                return r;

        while (n_in > 0) {
                size_t len;

                if (*in == '\'') {
                        const char *escape = "'\\''";

                        c_shquote_skip_char(&in, &n_in);

                        r = c_shquote_append_str(&out, &n_out, escape, strlen(escape));
                        if (r)
                                return r;
                } else {
                        /*
                         * Consume until the next single quote. If none exists,
                         * consume the rest of the string.
                         */
                        len = c_shquote_strncspn(in, n_in, "'");
                        r = c_shquote_consume_str(&out, &n_out, &in, &n_in, len);
                        if (r)
                                return r;
                }
        }

        r = c_shquote_append_char(&out, &n_out, '\'');
        if (r)
                return r;

        *outp = out;
        *n_outp = n_out;
        return 0;
}

_public_ int c_shquote_unquote(char **outp,
                               size_t *n_outp,
                               const char *in,
                               size_t n_in) {
        char *out = *outp;
        size_t n_out = *n_outp;
        int r;

        while (n_in > 0) {
                size_t len;

                switch (*in) {
                case '\'':
                        r = c_shquote_unquote_single(&out, &n_out, &in, &n_in);
                        if (r)
                                return r;

                        break;
                case '\"':
                        r = c_shquote_unquote_double(&out, &n_out, &in, &n_in);
                        if (r)
                                return r;

                        break;
                case '\\':
                        r = c_shquote_unescape_char_unquoted(&out, &n_out, &in, &n_in);
                        if (r)
                                return r;

                        break;
                default:
                        /*
                         * Consume until the next escape character. If none
                         * exists, consume the rest of the string.
                         */
                        len = c_shquote_strncspn(in, n_in, "\'\"\\");
                        r = c_shquote_consume_str(&out, &n_out, &in, &n_in, len);
                        if (r)
                                return r;
                }
        }

        *outp = out;
        *n_outp = n_out;
        return 0;
}

_public_ int c_shquote_parse_next(char **outp,
                                  size_t *n_outp,
                                  const char **inp,
                                  size_t *n_inp) {
        char *out = *outp;
        size_t n_out = *n_outp;
        const char *in = *inp;
        size_t n_in = *n_inp;
        bool quoted = false;
        int r;

        while (n_in > 0) {
                size_t len;

                switch (*in) {
                case '\'':
                        r = c_shquote_unquote_single(&out, &n_out, &in, &n_in);
                        if (r)
                                return r;

                        quoted = true;

                        break;
                case '\"':
                        r = c_shquote_unquote_double(&out, &n_out, &in, &n_in);
                        if (r)
                                return r;

                        quoted = true;

                        break;
                case '\\':
                        r = c_shquote_unescape_char_unquoted(&out, &n_out, &in, &n_in);
                        if (r)
                                return r;

                        break;
                case ' ':
                case '\t':
                case '\n':
                        c_shquote_discard_whitespace(&in, &n_in);

                        if (n_out != *n_outp || quoted)
                                goto out;

                        break;
                case '#':
                        if (n_out == *n_outp && !quoted) {
                                c_shquote_discard_comment(&in, &n_in);
                        } else {
                                r = c_shquote_consume_char(&out, &n_out, &in, &n_in);
                                if (r)
                                        return r;
                        }

                        break;
                default:
                        /*
                         * Consume until the next escape character. If none
                         * exists, consume the rest of the string.
                         */
                        len = c_shquote_strncspn(in, n_in, "\'\"\\# \t\n");
                        r = c_shquote_consume_str(&out, &n_out, &in, &n_in, len);
                        if (r)
                                return r;

                        break;
                }
        }

out:
        if (n_out == *n_outp && !quoted)
                return C_SHQUOTE_E_EOF;

        *outp = out;
        *n_outp = n_out;
        *inp = in;
        *n_inp = n_in;
        return 0;
}

_public_ int c_shquote_parse_argv(char ***argvp,
                                  size_t *argcp,
                                  const char *input,
                                  size_t n_input) {
        char buf[n_input];
        char *out = buf;
        size_t n_out = sizeof(buf);
        const char *in = input;
        size_t n_in = n_input;
        size_t argc = 0;
        char **argv;
        int r;

        /*
         * Verify the correctness of the input, and count the number
         * of tokens in the output.
         */
        for (;;) {
                r = c_shquote_parse_next(&out, &n_out, &in, &n_in);
                if (r) {
                        if (r == C_SHQUOTE_E_EOF)
                                break;

                        return r;
                }

                ++argc;
                *out = '\0';
                ++out;
                --n_out;
        }

        n_out = out - buf;

        argv = malloc(sizeof(char*) * argc + n_out);
        if (!argv)
                return -ENOMEM;

        out = (char*)argv + (sizeof(char*) * argc);
        n_in = n_input;
        in = input;

        for (size_t i = 0; i < argc; ++i) {
                argv[i] = out;

                r = c_shquote_parse_next(&out, &n_out, &in, &n_in);
                if (r)
                        return r;

                *out = '\0';
                ++out;
                --n_out;
        }

        *argvp = argv;
        *argcp = argc;
        return 0;
}
