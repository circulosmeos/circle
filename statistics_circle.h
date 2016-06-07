// Calculate statiscal variance of bytes in a file
// and graph them on an ASCII circle
//
// v1.0 by circulosmeos, 2015-10.
// v1.2 by circulosmeos, 2016-01.
// v2.0 by circulosmeos, 2016-06.
// wp.me/p2FmmK-96
// goo.gl/TNh5dq
//
// Distributed under GPL v3 License.
//

#ifndef STATISTICS_CIRCLE_H_
#define STATISTICS_CIRCLE_H_

// .................................................
// large file support (LFS) (files with size >2^31 (2 GiB) in linux, and >4 GiB in Windows)
#define _FILE_OFFSET_BITS 64	// stat, fseek
#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE		// off64_t for fseek64
#define ZERO_LL 0LL				// crafted specially to be used in FSEEK( , , SEEK_END);
// .................................................


#include <stdio.h>
//#include <stdint.h> // incompatible with Solaris
#include <inttypes.h> //   compatible with Solaris
#include <complex.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> // getopt()
#include <getopt.h> // getopt() compatible with -std=c99
#include <ctype.h>  // isprint()

#define PROGRAM_NAME "circle"

#define BUFFER_LENGTH 4096

#define MAX_SIGMA_CHAR 10

#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KBLU  "\x1B[34m"
#define KWHT  "\x1B[37m"
#define RESET "\033[0m"

void create_circle(double complex *coordinates);

extern const signed int CIRCLE_EMPTY_VALUE;

extern const int MAX_X, MAX_Y;

extern const int MAX_VALUE;

void print_circle_value(signed int value);

int analyze_file(char *szFile);

void print_help();

#endif /* STATISTICS_CIRCLE_H_ */
