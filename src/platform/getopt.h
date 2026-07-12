/*
 * Minimal getopt implementation for platforms that do not provide one.
 *
 * SPDX-License-Identifier: ISC
 */

#ifndef UDP_FLASHLC_BRIDGE_PLATFORM_GETOPT_H
#define UDP_FLASHLC_BRIDGE_PLATFORM_GETOPT_H

#ifdef __cplusplus
extern "C" {
#endif

extern char* optarg;
extern int opterr;
extern int optind;
extern int optopt;

int getopt(int argc, char* const argv[], const char* optstring);

#ifdef __cplusplus
}
#endif

#endif
