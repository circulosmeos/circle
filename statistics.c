// Calculate statiscal variance of bytes in a file
// and graph them on an ASCII circle
//
// v1.0 by circulosmeos, 2015-10.
// v1.2 by circulosmeos, 2016-01.
// v2.1, v2.2 by circulosmeos, 2016-06.
// v2.3 by circulosmeos, 2016-07.
// v2.4 by circulosmeos, 2016-12.
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

int two_circles_value=0;


int main ( int argc, char *argv[] ) {

    int i, output, global_ouput;
    bool bBreakOnFirstError=false;

    /*if (argc < 2) {
        print_help();
        return 1;
    }*/

    int opt = 0;
    while ((opt = getopt(argc, argv, "z:o:bnuBrh")) != -1)
        switch(opt) {
            // help
            case 'h':
                print_help();
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
            case 'B':
                bBreakOnFirstError=true;
                break;
            case '?':
                if (isprint (optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                return 2;
            default:
                abort ();
        }

    global_ouput=0;

    if (optind == argc || argc == 1) {
        // file input is stdin
        analyze_file( "" );
    
    } else {

        for (i = optind; i < argc; i++) {
            output = analyze_file( argv[i] );
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


int analyze_file(char *szFile) {

    long long total_size = 0;
    long long bytes[MAX_VALUE+1];
    int number_of_byte_buckets = 0;

    char *SIZE_UNITS[6] = { "bytes", "kiB", "MiB", "GiB", "TiB", "PiB"};
    double readable_size=0.0;

    const unsigned int buffer_length = BUFFER_LENGTH;
    char buffer[BUFFER_LENGTH];
    size_t k;
    size_t bytes_read;
    
    //char *szFile;
    FILE *hFile;

    // sigma
    double mean;
    double sigma;
    double deviation;

    // for circle generation:
    //int MAX_X=50, MAX_Y=16;
    double complex coordinates[MAX_VALUE+1];
    signed int circle[MAX_X][MAX_Y];
    signed int circle2[MAX_X][MAX_Y];
    int i, j;


    // .................................................
    // .................................................
    // .................................................


    if (strlen(szFile)==0) {
        SET_BINARY_MODE(STDIN); // sets binary mode for stdin in Windows
        hFile = stdin;
    } else
        hFile = fopen(szFile, "rb");

    if ( hFile == NULL  ) {
        fprintf (stderr, "Could not open file '%s'\n", szFile);
        return 3;
    }

    // fill counter matrix with zeros
    for (i=0; i<(MAX_VALUE+1); ++i)
        bytes[i] = 0;

    // actually count different bytes in file
    do {
        bytes_read = fread(buffer, 1, buffer_length, hFile);
        for (k = 0; k < bytes_read; ++k)
            ++bytes[(unsigned char)buffer[k]];
        total_size += bytes_read;
    } while (bytes_read == buffer_length);

    // close file: it is not needed any more
    fclose (hFile);


    // .................................................
    // .................................................
    // .................................................


    // sigma calculation:
    sigma = 0.0;
    mean  = 0.0;
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
    


    // .................................................
    // .................................................
    // .................................................


    create_circle(coordinates);

    // fill circle's container matrix with zeros
    for (i=0; i<MAX_X; i++) {
        for (j=0; j<MAX_Y; j++) {
            circle[i][j]=CIRCLE_EMPTY_VALUE;
            if (two_circles_flag)
                circle2[i][j]=CIRCLE_EMPTY_VALUE;
        }
    }

    // finally!
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
        printf("%s%s", RESET, KWHT);

    if (two_circles_flag) {
        printf("\t    0 centered\t\t\t\t%d centered\n\n", two_circles_value);
    }

    // .................................................
    // .................................................
    // .................................................

    // print various statistics:

    // print file name, so output from batch processes is useful:
    if (strlen(szFile)==0)
        printf("file =\tstdin\n");
    else
        printf("file =\t%s\n", szFile);

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

    printf("size =\t%.2f %s,  (%lld bytes)\n", readable_size, SIZE_UNITS[i], total_size);

    if (color_flag)
        printf("%s", RESET);

    return 0;

}


void print_circle_value(signed int value) {

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
    printf ("  Use:\n  $ %s [-o {0|1|2|3}] [-Bbnruh] [-z {0-255}] [<filename>] [<filename>] ...\n\n", 
        PACKAGE_NAME);
    printf ("\t-o {0 | 1=no color | 2=numbers | 3=uncoloured numbers}\n"
        "\t-B : stop processing files on first error encountered\n"
        "\t-b : no color\n"
        "\t-n : numbers\n"
        "\t-r : restrict statistics to the byte buckets that appear \n\t     in the file, not to the 256 default value.\n"
        "\t-u : uncoloured numbers (-b -n)\n"
        "\t-h : prints this help\n"
        "\t-z {0-255} : prints a 2nd circle centered on this byte (0==127 !)\n\n"
        );

}
