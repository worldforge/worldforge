all: texturemodule.so 

texturemodule.so: texturemodule.o
	gcc -shared -lgif -L/usr/X11R6/lib -lX11 -o texturemodule.so texturemodule.o

texturemodule.o: texturemodule.c
	gcc -O2 -fPIC -I/usr/include/python2.2/ -c texturemodule.c
