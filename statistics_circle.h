// Calculate statiscal variance of bytes in a file
// and graph them on an ASCII circle
//
// v1.0 by circulosmeos, 2015-10.
// v1.2 by circulosmeos, 2016-01.
// v2.1, v2.2 by circulosmeos, 2016-06.
// v2.3 by circulosmeos, 2016-07.
// v2.4 by circulosmeos, 2016-12.
// v3.0 by circulosmeos, 2021-07.
// wp.me/p2FmmK-96
// goo.gl/TNh5dq
//
// Distributed under GPL v3 or higher License.
//

#ifndef STATISTICS_CIRCLE_H_
#define STATISTICS_CIRCLE_H_

#define _XOPEN_SOURCE 600 // getopt() compilation compatible with Solaris 9

// .................................................
// large file support (LFS) (files with size >2^31 (2 GiB) in linux, and >4 GiB in Windows)
#define _FILE_OFFSET_BITS 64    // stat, fseek
#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE     // off64_t for fseek64
#define ZERO_LL 0LL             // crafted specially to be used in FSEEK( , , SEEK_END);
// .................................................


#include <stdio.h>
//#include <stdint.h> // incompatible with Solaris
#include <inttypes.h> // compatible with Solaris
#include <errno.h>    // printf to stderr
#include <complex.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> // getopt()
#include <ctype.h>  // isprint()
#include <sys/stat.h> //stat()

#ifndef __sun
# include <getopt.h> // getopt() compatible with -std=c99
#endif

// sets binary mode for stdin in Windows
#define STDIN 0
#ifdef _WIN32
# include <io.h>
# include <fcntl.h>
# define SET_BINARY_MODE(handle) setmode(handle, O_BINARY)
#else
# define SET_BINARY_MODE(handle) ((void)0)
#endif

#define PACKAGE_NAME    "circle"
#define PACKAGE_STRING  "circle v3.0 (goo.gl/TNh5dq)"

#define BUFFER_LENGTH 4096

#define MAX_SIGMA_CHAR 10

#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KBLU  "\x1B[34m"
#define KWHT  "\x1B[37m"
#define RESET "\x1B[m"

void create_circle( double complex *coordinates );

extern const signed int CIRCLE_EMPTY_VALUE;

extern const int MAX_X, MAX_Y;

extern const int MAX_VALUE;

void print_circle_value( signed int value );

int analyze_file(
    char *szFile,
    unsigned long long slice_number,
    unsigned long long slice_size,
    bool bShowGlobalFileStatistics,
    unsigned long long from_byte,
    unsigned long long to_byte
);

void empty_circle(
    signed int circle[MAX_X][MAX_Y],
    signed int circle2[MAX_X][MAX_Y],
    bool two_circles_flag
);

void calculate_sigma(
    unsigned long long *bytes,
    double *sigma_parameter,
    double *mean_parameter,
    int *number_of_byte_buckets_parameter,
    unsigned long long total_size
);

void print_circle_on_screen(
    unsigned long long *bytes,
    double sigma,
    double mean,
    double complex *coordinates,
    signed int circle[MAX_X][MAX_Y],
    signed int circle2[MAX_X][MAX_Y],
    bool two_circles_flag,
    int two_circles_value,
    bool restrict_statistics,
    int list_bytes,
    int number_of_byte_buckets,
    char *szFile,
    unsigned long long total_size,
    unsigned long long total_bytes_read,
    unsigned long long slice_size,
    unsigned long long slice_actual_number,
    unsigned long long from_byte,
    unsigned long long to_byte
);

void print_circle_value( signed int value );

void print_help();

unsigned long long giveMeAnInteger( const char *original_input );

#endif /* STATISTICS_CIRCLE_H_ */
