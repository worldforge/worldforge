
# Standard defines:
CCC	=	g++
CC	=	gcc
AR     	=	ar
RANLIB	=	ranlib
STDLIBS	=	-L$(OUTDIR)
STDINCS	=	-I.
#C_FLAGS	=	-g -D_ADebug_ -Wall
C_FLAGS	=	-g -Wall
#C_FLAGS	=	-O3 -Wall
MK_DEP	=	$(CC) $(C_FLAGS) $(incDirs) -MM

OUTDIR         = linux
EXE_EXT        =
#incDirs        =       -I/usr/local/include/python1.5 $(STDINCS)
incDirs        =       -I/usr/include/python1.5 $(STDINCS)
libDirs        =       $(STDLIBS)
#LIBS           =       -lAtlas -lpython1.5 -lstdc++ -ldl
LIBS           =       -lstdc++ -lAtlas -lpython1.5 -ldl -lz -ltk -ltcl -L/usr/X11R6/lib -lX11 -lreadline -lpthread

include Makefile.common
