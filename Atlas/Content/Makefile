
# Standard defines:
CC     	=	g++
AR     	=	ar
RANLIB	=	ranlib
STDLIBS	=	-L$(OUTDIR)
STDINCS	=	-I. -I../libAtlas2
#C_FLAGS	=	-g -D_ADebug_ -Wall
C_FLAGS	=	-g -Wall
#C_FLAGS	=	-O3 -Wall
MK_DEP	=	$(CC) $(C_FLAGS) $(incDirs) -MM

OUTDIR         = linux
EXE_EXT        =
incDirs        =       -I/usr/local/include/python1.5 $(STDINCS)
#incDirs        =       -I/usr/include/python1.5 $(STDINCS)
libDirs        =       $(STDLIBS)
LIBS           =       -lstdc++ -lpython1.5 -lAtlas -ldl

include Makefile.common
