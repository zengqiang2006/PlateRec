PlateRec
=========================================================================

PlateRec is a software for Italian License Plate Recognition. It was create using __Ansi C__
and __OpenCV__ libraries. Using this program is possible to get Plate Number from a picture
of the rear part of a car or motorbike.

Dependencies
-------------------------------------------------------------------------
PlateRec is based on [OpenCv 2.2](http://opencv.willowgarage.com/wiki/)
(Likely it will work also with successive versions.)


Compiling
-------------------------------------------------------------------------
To compile the software it's necessary opencv lib installed on your OS.
On Linux you can find that on relative repository. 

From command line you can launch:
<code>gcc \`pkg-config --libs opencv\` \`pkg-config --cflags opencv\` -Wall main.c -o main</code>

(create an alias for it strong recommended)


Run
-------------------------------------------------------------------------
To run the software you can launch:
<code>./main _imagefilename_</code>

You can find the result images in output folder

