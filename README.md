**circle**: statistics ascii circle for analysing byte entropy in files.   

**circle** reads all bytes in a file, and counts and arranges them from 0x00 to 0xff. Them it calculates the Standard deviation (sigma) of the data, and arranges the 256 resulting buckets in a circle, in such a way that the distance from the centre is proportional to the byte value (with 0x00 at the center, and ...0xf0-0xff bytes the farthest from it). The char that represents each byte is proportional to the deviation from the mean, in fractions of the standard deviation.   
   
Compilation:

$ gcc [-lm](http://stackoverflow.com/questions/5005363/undefined-reference-to-sin) statistics.c circle.c -o circle   
or simply   

    $ make   

There're [executable files for various OS available here](https://drive.google.com/folderview?id=0B1L_hFrWJfRhODE3RE5fNGNaWWM).

A picture is worth a thousand words: let's *circle* a plain text file: the [GPL v3 license](https://github.com/circulosmeos/circle/blob/master/gplv3.txt):   
   
![](https://circulosmeos.files.wordpress.com/2015/10/circle-gplv3-plaintext.png)
    
Blue chars indicate bytes that are absent from the sample. Green represents variations above the mean, and red below it, with the size of the char directly proportional to the absolute magnitude of the deviation in fractions of sigma: from "," representing 1/4 of sigma to "@" representing 9/4 = 2.25 sigma or above. The complete list of chars is from 0 to 9:   
   
    { '.', ',', '-', '~', '+', '*', 'o', 'O', '#', '@' };   
   
A case of use can be found here: [http://wp.me/p2FmmK-96](http://wp.me/p2FmmK-96).   
   
A random file looks like:
   
![](https://circulosmeos.files.wordpress.com/2015/10/circle-urandom.png)
    
Options for non-colored consoles (in this case chars represent increments of 0.5 sigma and zero is char '*') and for using numbers instead of ASCII art are also available:   
   
    $ circle   
   
    Show statistics about bytes contained in a file,   
    as a circle graph of deviations from sigma.   
    Use:   
    $ circle <filename> [0|1=no color,2=numbers,3=uncoloured numbers] [0-255=two circles!]   
   

Licensed as [GPL v3](http://www.gnu.org/licenses/gpl-3.0.en.html) or higher.   
