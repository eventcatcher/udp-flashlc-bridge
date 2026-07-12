/*
 * Minimal getopt implementation for platforms that do not provide one.
 *
 * Copyright (c) 2026 udp-flashlc-bridge contributors
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "getopt.h"

#include <stdio.h>
#include <string.h>

char* optarg = NULL;
int opterr = 1;
int optind = 1;
int optopt = 0;

int getopt(int argc, char* const argv[], const char* optstring)
{
    static const char* next = NULL;
    const char* option;
    int optionChar;

    optarg = NULL;

    if (optind < 1)
        optind = 1;

    if (next == NULL || *next == '\0') {
        const char* current;

        if (optind >= argc)
            return -1;

        current = argv[optind];
        if (current == NULL || current[0] != '-' || current[1] == '\0')
            return -1;

        if (0 == strcmp(current, "--")) {
            optind++;
            return -1;
        }

        next = current + 1;
    }

    optionChar = (unsigned char)*next++;
    option = strchr(optstring, optionChar);
    optopt = optionChar;

    if (optionChar == ':' || option == NULL) {
        if (opterr && optstring[0] != ':')
            fprintf(stderr, "unknown option -- %c\n", optionChar);

        if (*next == '\0') {
            optind++;
            next = NULL;
        }

        return '?';
    }

    if (option[1] == ':') {
        if (*next != '\0') {
            optarg = (char*)next;
            optind++;
            next = NULL;
        } else if (optind + 1 < argc) {
            optarg = argv[++optind];
            optind++;
            next = NULL;
        } else {
            if (opterr && optstring[0] != ':')
                fprintf(stderr, "option requires an argument -- %c\n", optionChar);

            optind++;
            next = NULL;

            return (optstring[0] == ':') ? ':' : '?';
        }
    } else if (*next == '\0') {
        optind++;
        next = NULL;
    }

    return optionChar;
}
