PlateRec
=========================================================================

Ladies and gentlemen,   
here's to you PlateRec, a "simply" and crazy way to "stringify" your car plate.
This program was create by me and other two crazy engineers for an University course project.

Dependencies
-------------------------------------------------------------------------
PlateRec is based on [OpenCv 2.2](http://opencv.willowgarage.com/wiki/)
(I don't know if it work with earlier version of this library, if you are enough brave to try, mail me your news)


Compiling
-------------------------------------------------------------------------
To compile the software it's necessary opencv lib installed on your OS.
On Linux you can find that on relative repository. I use OpenCv 2.2
(version on ArchLinux repository) if you can try it on old version of
OpenCv tell me if it work.

From command line you can launch:
<code>gcc \`pkg-config --libs opencv\` \`pkg-config --cflags opencv\` -Wall main.c -o main</code>

Obviously you can <b>(and you MUST)</b> create an alias for this :)


Run
-------------------------------------------------------------------------
To run the software you can launch:
<code>./main _imagefilename_</code>

You can find the result images in output folder


FAQ
-------------------------------------------------------------------------
1. Why there isn't a fucking Makefile to compile this fucking program??

	_Because when I will find the fucking time to create that I will do :D_

2. Why your english is so poor??

	_I'm very sorry about it :( But you can ask it to my English Teacher :D_

3. Can ask you ask something more??

	_No :P_
