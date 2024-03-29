// Calculate statiscal variance of bytes in a file
// and graph them on an ASCII circle
//
// v1.0 by circulosmeos, 2015-10.
// v1.2 by circulosmeos, 2016-01.
// v2.1, v2.2 by circulosmeos, 2016-06.
// v2.3 by circulosmeos, 2016-07.
// v3.0, v3.1 by circulosmeos, 2021-07, 2021-08.
// wp.me/p2FmmK-96
// goo.gl/TNh5dq
//
// Distributed under GPL v3 or higher License.
//

#include "statistics_circle.h"

const signed int CIRCLE_EMPTY_VALUE=255;

const int MAX_X=35, MAX_Y=16;

const int MAX_VALUE=255;

// Stores in each byte bucket coordinates[MAX_VALUE + 1]
// the integer coordinates that correspond to it in the
// MAX_X * MAX_Y square containing the ASCII circle.
void create_circle(double complex *coordinates)
{

    //int MAX_VALUE=255;
    //int MAX_X=50, MAX_Y=16;
    int INIT_X=MAX_X/2, INIT_Y=MAX_Y/2;
    signed int circle[MAX_X][MAX_Y];
    //double complex coordinates[MAX_VALUE+1];
    //signed int CIRCLE_EMPTY_VALUE=255;
    double x ,y, r, inc_r, angle, inc_angle, proportion;
    int i, j, k;

    angle=0.0;
    inc_angle=5.0;
    r=1.0;
    inc_r=0.0184;
    proportion=0.5;
    x=INIT_X;
    y=INIT_Y;

    for (i=0; i<MAX_X; i++) {
        for (j=0; j<MAX_Y; j++) {
            circle[i][j]=CIRCLE_EMPTY_VALUE;
        }
    }

    circle[(int)x][(int)y]=0;
    coordinates[0] = (int)x + (int)y*I;
    x++;
    circle[(int)x][(int)y]=1;
    coordinates[1] = (int)x + (int)y*I;

    for (i=2; i<=MAX_VALUE; i++) {
        //printf ("%d=", i);
        do {
            k++;
            angle+=inc_angle;
            r+= inc_r;
            x+= -(cos(angle)) * r;
            y+= +(sin(angle)) * r * proportion;
            if (x>MAX_X || y>MAX_Y || x<0 || y<0) {
                fprintf (stderr, "ERROR! %.1f, %.1f", x, y );
                break;
            }
            if ( circle[(int)x][(int)y] == CIRCLE_EMPTY_VALUE ) {
                circle[(int)x][(int)y]=i;
            }
        } while (circle[(int)x][(int)y] != i);
        coordinates[i] = (int)x + (int)y*I;
    }

    //printf("\n%d",k);

}
