.PHONY : clean
vpath_src=.. ../.. ../../../fortify ../../../randlib/src ../../../SpatialAnalysis
vpath %.c    $(vpath_src)
vpath %.cpp  $(vpath_src)
vpath %.hpp  $(vpath_src)
vpath %.h    $(vpath_src)

# The X11 base dir on your system
X11BASE=/usr/X11R6
# Add directories with X11 include files here
X11INCS=-I$(X11BASE)/include
# put X11 required libraries and directories here
X11LIBS=-L$(X11BASE)/lib -lX11

SDLDEFS = -D__XWIN__

I_DIRS=-I../.. -I../../../fortify -I.. -I../../../randlib/src -I../../../SpatialAnalysis
P_DEFS=-DGRAPHICS -DPERIODIC_BOUNDARY

#CFLAGS = -O3 -Wall -Ic:/cpp/fortify -Ic:/cpp/canew -DGRAPHICS -DFORTIFY -fexternal-templates 
CXXFLAGS = -O2 -Wall $(I_DIRS) $(X11INCS)  $(SDLDEFS) $(P_DEFS)

O = bgi.o cabase.o caSimul.o caInvaIO.o caInvaStats.o com.o fortify.o\
	linpack.o mcaInva.o Period2d.o Polar2D.o randlib.o RipleyTriangle.o\
	Spec2D.o spekout.o


L = -lm -lgrx20S -lSDL $(X11LIBS)

caInva : $(O)
	g++ -o caSimul $(O) $(L)

clean:
	rm caSimul $(O)


# DEPENDENCIES

mcaInva.o: mcaInva.cpp 

caSimul.o: caInva.cpp caInva.hpp cabase.hpp cabase.cpp cellInva.hpp smattpl.hpp

cabase.o: cabase.cpp cabase.hpp

bgi.o: bgi.cpp

caInvaIO.o: caInvaIO.cpp

caInvaStats.o: caInvaStats.cpp

RipleyTriangle.o: RipleyTriangle.cpp

Period2d.o: Period2d.cpp

Polar2D.o: Polar2D.cpp

Spec2D.o: Spec2D.cpp

spekout.o: spekout.cpp

fortify.o : fortify.cpp fortify.h ufortify.h

com.o : com.c

linpack.o : linpack.c

randlib.o : randlib.c