**circle**: statistics ASCII circle for analysing byte entropy in files.   

**circle** reads all bytes in a file, and counts and arranges them from 0x00 to 0xff. Then it calculates the Standard deviation (sigma) of the data, and arranges the 256 resulting buckets in a circle, in such a way that the distance from the centre is proportional to the byte value (with 0x00 at the center, and ...0xf0-0xff bytes the farthest from it). The char that represents each byte is proportional to the deviation from the mean, in fractions of the standard deviation.   

From v2.6, the list of different bytes counted can be printed using options `-l`, `-L`, `-Z`. Values over the mean will be printed green, and below it, they will be red.
   
Installation
============

**circle** can be installed (as **bytes-circle**) in **Ubuntu** and **Debian** using official repositories. Follow [these instructions](http://serverfault.com/questions/550855/how-to-add-debian-testing-repository-to-apt-get) for adding repositories to previous versions' linux distributions, and then just install as usual: 

    $ sudo apt-get install bytes-circle

There is a PPA repository available for Ubuntu (contains versions from *precise* to *zesty*):

    $ sudo add-apt-repository ppa:roberto.s.galende/bytes-circle
    $ sudo apt-get update
    $ sudo apt-get install bytes-circle

There're [executable files for various OS available here](https://drive.google.com/folderview?id=0B1L_hFrWJfRhODE3RE5fNGNaWWM), including versions for Windows, HP-UX and Solaris.

If using [R](https://cran.r-project.org/), there's a port here: [https://github.com/circulosmeos/bytescircle](https://github.com/circulosmeos/bytescircle).

Compilation
===========

Compilation:

    $ gcc statistics.c circle.c -o circle -lm    

or simply   

    $ make   

Examples
========

A picture is worth a thousand words: let's *circle* a plain text file: the [GPL v3 license](https://github.com/circulosmeos/circle/blob/master/gplv3.txt):   
   
![](https://circulosmeos.files.wordpress.com/2015/10/circle-gplv3-plaintext.png)
    
Blue chars indicate bytes that are absent from the sample. Green represents variations above the mean, and red below it, with the size of the char directly proportional to the absolute magnitude of the deviation in fractions of sigma: from "," representing 1/4 of sigma to "@" representing 9/4 = 2.25 sigma or above. The complete list of chars is from 0 to 9:   
   
    { '.', ',', '-', '~', '+', '*', 'o', 'O', '#', '@' };   
   
A case of use can be found here: [http://wp.me/p2FmmK-96](http://wp.me/p2FmmK-96).   
   
A random file looks like:
   
![](https://circulosmeos.files.wordpress.com/2015/10/circle-urandom.png)
    
Options for non-colored consoles (in this case chars represent increments of 0.5 sigma and zero is char '*') and for using numbers instead of ASCII art are also available:   
   
    $ bytes-circle -h   
   
      circle v2.6 (goo.gl/TNh5dq)

      Show statistics about bytes contained in a file,
      as a circle graph of deviations from sigma.

      Use:
      $ circle [-o {0|1|2|3}] [-BblLZnruvh] [-z {0-255}] [<filename>] [<filename>] ...

            -o {0 | 1=no color | 2=numbers | 3=uncoloured numbers}
            -B : stop processing files on first error encountered
            -b : no color
            -l : list number of bytes counted, from 0 to 255
            -L : list number of bytes counted, excluding zero valued
            -Z : list number of bytes counted, but only zero valued
            -n : numbers
            -r : restrict statistics to the byte buckets that appear
                 in the file, not to the 256 default value.
            -u : uncoloured numbers (-b -n)
            -v : prints version
            -h : prints this help
            -z {0-255} : prints a 2nd circle centered on this byte (0==127 !)

License
=======

Licensed as [GPL v3](http://www.gnu.org/licenses/gpl-3.0.en.html) or higher.   
