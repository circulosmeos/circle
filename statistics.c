// Calculate statiscal variance of bytes in a file
// and graph them on an ASCII circle
//
// v1.0 by circulosmeos, 2015-10.
// v1.2 by circulosmeos, 2016-01.
// v2.1, v2.2 by circulosmeos, 2016-06.
// v2.3 by circulosmeos, 2016-07.
// v2.4, v2.5 by circulosmeos, 2016-12.
// v2.6 by circulosmeos, 2018-12.
// v3.0 by circulosmeos, 2021-07.
// wp.me/p2FmmK-96
// goo.gl/TNh5dq
//
// Distributed under GPL v3 or higher License.
//

#include "statistics_circle.h"


char sigma_char[MAX_SIGMA_CHAR+1] = { 
    '.', ',', '-', '~', '+', '*', 'o', 'O', '#', '@', 
    '=' 
};

bool color_flag = true;

bool numbers_flag = false;

bool two_circles_flag = false;

bool restrict_statistics = false;

int list_bytes = 0;

int two_circles_value=0;


int main ( int argc, char *argv[] ) {

    int i, output, global_ouput;
    bool bBreakOnFirstError=false;
    bool bShowGlobalFileStatistics=false;
    unsigned long long slice_size = 0ULL;
    unsigned long long slice_number = 0ULL;
    unsigned long long from_byte = 0ULL;
    unsigned long long to_byte = 0ULL;
    unsigned long long to_byte_increment = 0ULL;

    /*if (argc < 2) {
        print_help();
        return 1;
    }*/

    int opt = 0;
    while ((opt = getopt(argc, argv, "z:o:blLZnuBgs:S:f:t:T:rvh")) != -1)
        switch(opt) {
            // help
            case 'h':
                print_help();
                return 1;
            // version
            case 'v':
                printf ("\n  %s \n\n  ", PACKAGE_STRING);
                for (i=0; i<MAX_SIGMA_CHAR; i++) {
                    printf ("%c ", sigma_char[i] );
                }
                printf("  each represents 1/4*sigma\n\n");
                return 1;
            // two circles: specify which ascii value set at zero position
            case 'z':
                two_circles_flag=true;
                if (strcmp(optarg,"0")==0) {
                    two_circles_value=MAX_VALUE/2;
                } else {
                    if(!(two_circles_value = atoi(optarg))) {
                        fprintf(stderr, "-z Error: Incorrect value for second circle's center (0-255).\n");
                        return 2;
                    }
                }        
                break;
            // compatibility with old options: [0|1=no color,2=numbers,3=uncoloured numbers]
            case 'o':
                if (strcmp(optarg,"0")==0)
                    break;
                if (strcmp(optarg,"1")==0)
                    color_flag=false;
                else 
                    if (strcmp(optarg,"2")==0)
                        numbers_flag=true;
                    else 
                        if (strcmp(optarg,"3")==0) {
                            color_flag=false;
                            numbers_flag=true;
                        } else {
                            fprintf(stderr, "-o Error: Incorrect value for colours style: [0|1|2|3].\n");
                            return 2;
                        }
                break;
            // black&white circle (no colours)
            case 'b':
                color_flag=false;
                break;
            // list number of bytes
            case 'l':
                list_bytes=1;
                break;
            // list number of bytes, but not 0 valued
            case 'L':
                list_bytes=2;
                break;
            // list number of bytes, but only 0 valued
            case 'Z':
                list_bytes=3;
                break;
            // numbers with colours instead of ascii art
            case 'n':
                numbers_flag=true;
                break;
            // restrict statistics to just the byte buckets that appear in the file
            case 'r':
                restrict_statistics=true;
                break;
            // uncoloured numbers
            case 'u':
                color_flag=false;
                numbers_flag=true;
                break;
            // end process on first error encountered when analyzing multiple files
            case 'B':
                bBreakOnFirstError=true;
                break;
            // show global file statistics when using -[sS]
            case 'g':
                bShowGlobalFileStatistics=true;
                break;
            // `-s #` slice file in '# bytes'-sized slices
            case 's':
                slice_size = (unsigned long long)giveMeAnInteger( optarg );
                if ( 0ULL == slice_size ) {
                    fprintf(stderr, "-s Error: slice size must be greater than zero.\n");
                    return 2;
                }
                if ( slice_number > 0ULL ) {
                    fprintf(stderr, "-[sS] Error: cannot use -s and -S at the same time.\n");
                    return 2;
                }
                break;
            // `-S #` slice file in # slices
            case 'S':
                slice_number = (unsigned long long)giveMeAnInteger( optarg );
                if ( 0ULL == slice_number ) {
                    fprintf(stderr, "-S Error: number of slices must be greater than zero.\n");
                    return 2;
                }
                if ( slice_size > 0ULL ) {
                    fprintf(stderr, "-[sS] Error: cannot use -s and -S at the same time.\n");
                    return 2;
                }
                break;
            case 'f':
                from_byte = (unsigned long long)giveMeAnInteger( optarg );
                if ( from_byte == 0ULL ) {
                    fprintf(stderr, "-f Error: value must be greater than zero.\n");
                    return 2;
                }
                break;
            case 't':
                to_byte = (unsigned long long)giveMeAnInteger( optarg );
                if ( to_byte == 0ULL ) {
                    fprintf(stderr, "-t Error: value must be greater than zero.\n");
                    return 2;
                }
                break;
            case 'T':
                to_byte_increment = (unsigned long long)giveMeAnInteger( optarg );
                if ( to_byte_increment == 0ULL ) {
                    fprintf(stderr, "-T Error: value must be greater than zero.\n");
                    return 2;
                }
                break;
            case '?':
                if (isprint (optopt)) {
                    fprintf(stderr, "Unknown option `-%c`.\n", optopt);
                    print_help();
                } else {
                    fprintf(stderr, "Unknown option character `\\x%x`.\n", optopt);
                }
                return 2;
            default:
                abort ();
        }

    if ( true == bShowGlobalFileStatistics &&
         slice_size == 0 &&
         slice_number == 0
    ) {
        fprintf(stderr, "-g Error: this option must be used with -[sS].\n");
        return 2;
    }

    if ( to_byte_increment > 0ULL ) {
        // calculate to_byte value and dismiss to_byte_increment
        to_byte = from_byte + to_byte_increment;
    }

    if ( ( from_byte > 0ULL && to_byte > 0ULL ) &&
         to_byte <= from_byte
    ) {
        fprintf(stderr, "-[ftT] Error: -t value must be greater than -f value.\n");
        return 2;
    }

    global_ouput=0;

    if (optind == argc || argc == 1) {
        // file input is stdin

        if ( slice_number > 0ULL ) {
            fprintf(stderr, "-S Error: -S cannot be used with STDIN input.\n");
            return 2;
        }

        analyze_file( "",
                    0, slice_size,
                    bShowGlobalFileStatistics,
                    from_byte, to_byte );
    
    } else {

        for (i = optind; i < argc; i++) {

            output = analyze_file( argv[i],
                                slice_number, slice_size,
                                bShowGlobalFileStatistics,
                                from_byte, to_byte );

            if (output != 0) {
                if (bBreakOnFirstError)
                    return 1;
                else
                    global_ouput=output;                
            }

        }

    }

    if (global_ouput!=0)
        fprintf(stderr, "\nErrors were found.\n\n");

}


int analyze_file(
    char *szFile,
    unsigned long long slice_number,
    unsigned long long slice_size,
    bool bShowGlobalFileStatistics,
    unsigned long long from_byte,
    unsigned long long to_byte
) {

    unsigned long long total_size = 0ULL;
    unsigned long long bytes[MAX_VALUE+1];
    unsigned long long global_bytes[MAX_VALUE+1];
    int number_of_byte_buckets = 0;

    unsigned int buffer_length = BUFFER_LENGTH;
    char buffer[BUFFER_LENGTH];
    size_t k = 0;
    size_t file_size = 0;
    size_t bytes_read = 0;
    size_t total_bytes_read = 0;
    
    //char *szFile;
    FILE *hFile;

    // sigma
    double mean = 0.0;
    double sigma = 0.0;

    // 'coordinates' contains the integer coordinates that correspond
    // to each byte bucket in a MAX_X * MAX_Y square containing the ASCII circle:
    double complex coordinates[MAX_VALUE+1];
    // 'circle' represents the MAX_X * MAX_Y square containing the ASCII circle,
    // which will be filled in the corresponding coordinates with
    // appropriate chars representing sigma values:
    signed int circle[MAX_X][MAX_Y];
    signed int circle2[MAX_X][MAX_Y];
    int i;


    // .................................................


    if (strlen(szFile)==0) {
        SET_BINARY_MODE(STDIN); // sets binary mode for stdin in Windows
        hFile = stdin;
    } else {
        hFile = fopen(szFile, "rb");
    }

    if ( hFile == NULL  ) {
        fprintf (stderr, "Could not open file '%s'\n", szFile);
        return 3;
    }

    // calculate positions for each byte bucket,
    // to represent an ASCII circle:
    create_circle(coordinates);


    if ( 0 != strlen(szFile) ) {

        // obtain file size
        struct stat st;
        stat(szFile, &st);
        file_size = st.st_size;

        // use file_size to calculate a valid slice_size
        // ( if -[sS] was indicated )
        if ( slice_number > 0ULL ||
             slice_size > 0ULL
        ) {
            // calculate slice_size to use always this value from now on
            if ( slice_number > 0ULL ) {
                unsigned long long sliceable_size = file_size - from_byte;
                if ( to_byte > 0ULL ) {
                    sliceable_size = to_byte - from_byte;
                }
                slice_size = (unsigned long long)((double)sliceable_size / (double)slice_number);
                // adjust slice_size to produce the indicated number of slices (slice_number)
                // at the cost of making the last slice slightly smaller:
                while ( slice_size * slice_number <= sliceable_size ) {
                    slice_size++;
                }
            }

            if ( slice_size == 0ULL ) {
                fprintf(stderr, "-S Error: slice number too big for this file.\n");
                return 2;
            }
        }

        // seek to from_byte position
        if ( from_byte > 0ULL ) {
            if ( from_byte > file_size ) {
                fprintf(stderr, "Error: file '%s' smaller than `-f %llu`.\n", szFile, from_byte);
                return 2;
            }
            if ( 0 != fseeko( hFile, from_byte - 1, SEEK_SET) ) {
                fprintf(stderr, "Error: cannot seek file '%s' to position '%llu'.\n", szFile, from_byte);
                return 2;
            }
            total_bytes_read = from_byte - 1;
        }

    }

    if ( 0 == strlen(szFile) ) {
        // adjust buffer_length with STDIN if from_byte
        // before entering the processing loop
        if ( from_byte > 0ULL ) {
            buffer_length = BUFFER_LENGTH;
            do {

                if ( ( total_bytes_read + BUFFER_LENGTH ) >= from_byte ) {
                    buffer_length = from_byte - total_bytes_read - 1;
                }

                bytes_read = fread(buffer, 1, buffer_length, hFile);
                total_bytes_read += bytes_read;

            } while ( total_bytes_read < from_byte &&
                    buffer_length > 0 &&
                    bytes_read > 0 );
        }
        // from_byte may has been too big for STDIN input
        if ( from_byte > 0ULL &&
             total_bytes_read < (from_byte - 1)
        ) {
            fprintf(stderr, "Error: data input smaller (%llu bytes) than -f %llu\n", total_size, from_byte);
            return 2;
        }
    }

    // process file
    // ( with slices if -[sS] was indicated )
    do {

        buffer_length = BUFFER_LENGTH;
        total_size = 0ULL;

        // fill counter matrix with zeros
        for (i=0; i<(MAX_VALUE+1); ++i) {
            bytes[i] = 0ULL;
        }

        // actually count different bytes in file
        do {

            // adjust buffer_length if slice_size
            if ( slice_size > 0ULL ) {
                if ( total_size + BUFFER_LENGTH > slice_size ) {
                    buffer_length = slice_size - total_size;
                }
            }

            // adjust buffer_length if to_byte
            if ( to_byte > 0ULL ) {
                if ( total_bytes_read + total_size + buffer_length > to_byte ) {
                    buffer_length = to_byte - (total_bytes_read + total_size);
                }
            }

            bytes_read = fread(buffer, 1, buffer_length, hFile);

            //
            // count bytes !!!
            //
            for (k = 0; k < bytes_read; ++k)
                ++bytes[(unsigned char)buffer[k]];

            total_size += bytes_read;

        } while ( bytes_read == buffer_length &&
                ( ( slice_size > 0ULL )? ( total_size < slice_size ) : true ) &&
                ( to_byte == 0ULL || ( total_bytes_read + total_size < to_byte ) )
                );

        // sigma calculation
        calculate_sigma(
            bytes,
            &sigma,
            &mean,
            &number_of_byte_buckets,
            total_size
        );
    
        // fill circle's container matrix with zeros
        empty_circle(
            circle,
            circle2,
            two_circles_flag
        );

        // print circle with associated statistics on screen
        print_circle_on_screen(
            bytes,
            sigma,
            mean,
            coordinates,
            circle,
            circle2,
            two_circles_flag,
            two_circles_value,
            restrict_statistics,
            list_bytes,
            number_of_byte_buckets,
            szFile,
            total_size,
            total_bytes_read,
            slice_size,
            from_byte,
            to_byte
        );

        total_bytes_read += total_size;

        if ( true == bShowGlobalFileStatistics ) {
            // increase global_bytes count with values just read
            for (i=0; i<(MAX_VALUE+1); ++i) {
                global_bytes[i] += bytes[i];
            }
        }

    } while ( ( ( 0 != strlen(szFile) )?
                // regular file:
                ( total_bytes_read < file_size ):
                // STDIN:
                ( ! feof( hFile ) ) )
                &&
                ( to_byte == 0ULL || total_bytes_read < to_byte )
            );


    // close file: it is not needed any more
    fclose (hFile);


    if ( true == bShowGlobalFileStatistics ) {
        // print also global_bytes count summary:

        // sigma calculation
        calculate_sigma(
            global_bytes,
            &sigma,
            &mean,
            &number_of_byte_buckets,
            total_bytes_read
        );
    
        // fill circle's container matrix with zeros
        empty_circle(
            circle,
            circle2,
            two_circles_flag
        );

        // print circle with associated statistics on screen
        print_circle_on_screen(
            global_bytes,
            sigma,
            mean,
            coordinates,
            circle,
            circle2,
            two_circles_flag,
            two_circles_value,
            restrict_statistics,
            list_bytes,
            number_of_byte_buckets,
            szFile,
            total_bytes_read,
            total_bytes_read,
            0ULL,
            0ULL,
            0ULL
        );
    }


    return 0;

}


// fill circle's container matrix with zeros
void empty_circle(
    signed int circle[MAX_X][MAX_Y],
    signed int circle2[MAX_X][MAX_Y],
    bool two_circles_flag
) {

    int i = 0;
    int j = 0;

    // fill circle's container matrix with zeros
    for (i=0; i<MAX_X; i++) {
        for (j=0; j<MAX_Y; j++) {
            circle[i][j]=CIRCLE_EMPTY_VALUE;
            if (two_circles_flag)
                circle2[i][j]=CIRCLE_EMPTY_VALUE;
        }
    }

}


// sigma calculation
void calculate_sigma(
    unsigned long long *bytes,
    double *sigma_parameter,
    double *mean_parameter,
    int *number_of_byte_buckets_parameter,
    unsigned long long total_size
) {

    int i = 0;

    int number_of_byte_buckets = 0;
    double mean = 0.0;
    double sigma = 0.0;

    // 1. mean value:
    if ( restrict_statistics ) {
        for (i=0; i<=MAX_VALUE; i++) {
            mean += (double)bytes[i];
            if (bytes[i]>0LL) number_of_byte_buckets++;
        }
        if (number_of_byte_buckets>0)
            mean /= (double)number_of_byte_buckets;
    } else {
        mean = (double)total_size / (double)(MAX_VALUE+1);
    }
    // 2. sigma value:
    if ( restrict_statistics ) {
        for (i=0; i<=MAX_VALUE; i++) {
            if (bytes[i]>0) sigma += pow( (double)bytes[i] - mean, 2.0);
        }
        if (number_of_byte_buckets>0)
            sigma = sqrt( sigma/(double)(number_of_byte_buckets) );
    } else {
        for (i=0; i<=MAX_VALUE; i++) {
            sigma += pow( (double)bytes[i] - mean, 2.0);
        }
        sigma = sqrt( sigma/(double)(MAX_VALUE+1) );
    }

    *sigma_parameter = sigma;
    *mean_parameter = mean;
    *number_of_byte_buckets_parameter = number_of_byte_buckets;

}


// print circle with associated statistics on screen
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
    unsigned long long from_byte,
    unsigned long long to_byte
) {

    int i = 0;
    int j = 0;

    char *SIZE_UNITS[6] = { "bytes", "kiB", "MiB", "GiB", "TiB", "PiB"};
    double readable_size=0.0;

    double deviation = 0.0;

    // fills circle's container matrix with ASCII art chars
    for (i=0; i<=MAX_VALUE; i++) {

        if (bytes[i] != 0) { //value has been seen in file
        
            // assign character size from sigma deviation
            if (sigma>0)
                deviation = ((double)(bytes[i]) - mean) / sigma * 4.0;
            else 
                deviation = 0.0;

            if (fabs(deviation) >= (double)(MAX_SIGMA_CHAR-1)) {
                if (deviation>0.0)
                    deviation=+(double)(MAX_SIGMA_CHAR-1);
                else
                    deviation=-(double)(MAX_SIGMA_CHAR-1);
            }

            circle[(int)creal(coordinates[i])]
                [(int)cimag(coordinates[i])] = (int)deviation;

        } else { //value hasn't been seen in file

            // mark it with reserved MAX_SIGMA_CHAR value, to be recognized and treated later
            circle[(int)creal(coordinates[i])]
                [(int)cimag(coordinates[i])] = MAX_SIGMA_CHAR;
        
        }
    }

    // if second circle is graphed, it will have the zero displaced
    // so human bias is less likely to occur
    if (two_circles_flag) {
        for (i=0; i<=MAX_VALUE; i++) {
            circle2[(int)creal(coordinates[ (i+two_circles_value)%(MAX_VALUE+1) ])]
                [(int)cimag(coordinates[ (i+two_circles_value)%(MAX_VALUE+1) ])] = 
                    circle[(int)creal(coordinates[i])]
                        [(int)cimag(coordinates[i])];
        }
    }

    // actually graph circle on screen with ASCII art chars and colours
    for (j=0; j<MAX_Y; j++) {
        
        for (i=0; i<MAX_X; i++) {
            print_circle_value( circle[i][j] );
        }

        if (two_circles_flag) {
            for (i=0; i<MAX_X; i++) {
                print_circle_value( circle2[i][j] );
            }
        }
        
        printf("\n");    
    }

    // reset colour use in terminal screen
    if (color_flag)
        printf("%s", RESET);

    if (two_circles_flag) {
        printf("\t    0 centered\t\t\t\t%d centered\n\n", two_circles_value);
    }

    // .................................................

    // print various statistics:

    // print file name, so output from batch processes is useful:
    if (strlen(szFile)==0) {
        printf("file =\tstdin");
    } else {
        printf("file =\t%s", szFile);
    }

    if ( slice_size > 0 ||
         from_byte > 0 ||
         to_byte > 0 ) {
        printf( "\t, [%llu-%llu] bytes", total_bytes_read + 1, total_bytes_read + total_size );
    }
    printf("\n");

    if ( !restrict_statistics || number_of_byte_buckets == (MAX_VALUE+1) ) {
        printf("mean =\t%.3f\n", mean);
    } else {
        printf("mean =\t%.3f (%d/%d byte buckets)\n", mean, number_of_byte_buckets, (MAX_VALUE+1));
    }
    printf("sigma= \t%.3f  ", sigma );

    if (mean>0.0) {
        printf("( CV= %.4f%% )\n", (sigma/mean*100.0) );
    } else {
        printf("( CV= 0%% )\n");
    }

    readable_size = total_size;
    for (i=0; readable_size>1024.0; i++)
        readable_size/=1024.0;

    printf("size =\t%.2f %s,  (%lld bytes)\n\n", readable_size, SIZE_UNITS[i], total_size);

    // print counted values of each byte if list_bytes > 0
    if ( list_bytes > 0 ) {
        for (i=0; i<=MAX_VALUE; i++) {
            if ( list_bytes == 1 || ( list_bytes == 2 && bytes[i] > 0LL ) ) {
                if (color_flag && bytes[i] > 0LL)
                    printf("%s", ((double)bytes[i] >= mean)?KGRN:KRED );
                printf("[%d] = %lld\n", i, bytes[i] );
                if (color_flag && bytes[i] > 0LL)
                    printf("%s", RESET);
            } else {
                if ( list_bytes == 3 && bytes[i] == 0LL )
                    printf("[%d] = %lld\n", i, bytes[i] );
            }
        }
        printf("\n");
    }

}


void print_circle_value(
    signed int value
) {

    char *color;

    if ( value != CIRCLE_EMPTY_VALUE ) {
        
        if ( value < 0 )
            color=KRED;
        else
            color=KGRN;

        // treat special MAX_SIGMA_CHAR value: it means unseen values
        if (value == MAX_SIGMA_CHAR)
            color=KBLU;

        if (!numbers_flag) {
            if (color_flag)
                printf( "%s%c", color, sigma_char[ abs(value) ] );
            else {
                if (value != MAX_SIGMA_CHAR)
                    printf( "%c", sigma_char[ (value/2) + (int)(MAX_SIGMA_CHAR/2) ] );
                else
                    printf( "%c", sigma_char[ MAX_SIGMA_CHAR ] );
            }
        } else {
            if (color_flag)
                if (value != MAX_SIGMA_CHAR)
                    printf( "%s%c", color, 48+abs(value) );
                else
                    printf( "%s%c", color, sigma_char[ MAX_SIGMA_CHAR ] );
            else {
                if (value != MAX_SIGMA_CHAR)
                    printf( "%c", ( value>=0 ? (48+abs(value)) : (96+abs(value)) ) );
                else
                    printf( "%c", sigma_char[ MAX_SIGMA_CHAR ] );
            }
        }
    } else {
        printf( " " );
    }

}


void print_help() {

    printf ("\n  %s \n", PACKAGE_STRING);
    printf ("\n  Show statistics about bytes contained in a file,\n  as a circle graph of deviations from sigma.\n\n");
    printf ("  Use:\n  $ %s [-o {0|1|2|3}] [-BbglLZnruvh] [-[sS] #[kmgtpe]] [-z {0-255}] [<filename>] [<filename>] ...\n\n",
        PACKAGE_NAME);
    printf (
        "\t-o : show sigma as {0 | 1=no color | 2=numbers | 3=uncoloured numbers}\n"
        "\t-B : stop processing files on first error encountered\n"
        "\t-b : no color\n"
        "\t-g : show summary values for file, when using `-[sS]`\n"
        "\t-l : list number of bytes counted, from 0 to 255\n"
        "\t-L : list number of bytes counted, excluding zero valued\n"
        "\t-Z : list number of bytes counted, but only zero valued\n"
        "\t-n : show sigma as numbers\n"
        "\t-r : restrict statistics to the byte buckets that appear \n\t     in the file, not to the 256 default value.\n"
        "\t-s#: slice file in # byte-size slices. SI suffixes supported.\n"
        "\t-S#: slice file in # slices. SI suffixes supported.\n"
        "\t-u : show sigma as uncoloured numbers (equivalent to `-b -n`)\n"
        "\t-v : prints version\n"
        "\t-h : prints this help\n"
        "\t-z {0-255} : prints a 2nd circle centered on this byte (0==127 !)\n\n"
        );

}


// obtain an integer from a string that may used decimal point,
// with valid suffixes: kmgtpe (powers of 10) and KMGTPE (powers of 2),
// and valid prefixes: "0" (octal), "0x" or "0X" (hexadecimal).
// Examples:
// "83m" == 83*10^6, "9G" == 9*2^30, "0xa" == 10, "010k" = 8000, "23.5k" = 23500
// INPUT:
// char *original_input: string containing the data (only read)
// OUTPUT:
// 64 bit long integer number
unsigned long long giveMeAnInteger( const char *original_input ) {

    unsigned i = 0;
    char *PowerSuffixes = "kmgtpeKMGTPE";
    unsigned PowerSuffixesLength = strlen(PowerSuffixes)/2;
    char *input = NULL;
    unsigned long long result = 1;

    if ( NULL == original_input )
        return 0LLU;

    input = malloc( strlen(original_input) +1 );
    memcpy(input, original_input, strlen(original_input) +1);

    if ( strlen(input) > 1 ) {
        // look for suffixes of size

        for ( i=0; i<strlen(PowerSuffixes); i++ ) {
            if ( (char *)strchr(input, PowerSuffixes[i]) == (char *)(input + strlen(input) -1) ) {
                if ( i >= PowerSuffixesLength ) {
                    result = pow( 2.0, 10.0 + 10.0*(double)(i - PowerSuffixesLength) );
                } else {
                    result = pow( 10.0, 3.0 + 3.0*(double)i );
                }
                input[strlen(input) -1] = '\0';
                break;
            }
        }

    }

    if ( strlen(input) > 1 ) {
        // look fo prefixes of base

        if ( input[0] == '0' ) {
            if ( strlen(input) > 2 &&
                 input[1] == '.' ) {
                // this is a float-point number
                ;
            } else {
                // hexadecimal or octal number:
                if ( strlen(input) > 2 &&
                     ( input[1] == 'x' || input[1] == 'X' ) ) {
                    // hexadecimal
                    result = strtoll( input +2, NULL, 16 ) * result;
                } else {
                    // octal
                    result = strtoll( input +1, NULL, 8 ) * result;
                    // Note: 0 is zero decimal and zero octal :-)
                }
                input[0] = '1';
                input[1] = '\0';
            }
        }

    }

    result = (unsigned long long)(strtod(input, NULL) * (double)result);

    if ( NULL != input )
        free( input );

    return result;

}