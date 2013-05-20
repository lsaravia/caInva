vpath %.cpp c:/cpp/canew c:/cpp/fortify
vpath %.hpp c:/cpp/canew c:/cpp/fortify
vpath %.h c:/cpp/canew c:/cpp/fortify
#CFLAGS = -O3 -Wall -Ic:/cpp/fortify -Ic:/cpp/canew -DGRAPHICS -DFORTIFY -fexternal-templates 
CFLAGS = -O3 -Wall -Ic:/cpp/fortify -Ic:/cpp/canew -DGRAPHICS
%o : %cpp
	gcc $(CFLAGS) -c $<

O = mvel.o caInva.o cabase.o r250.o bgi.o fortify.o 

L = -lgpp -lm -lgrx20 -lpc

gcaimp : $(O)
	gcc -o gcaVel $(O) $(L)
	stubify gcaVel
	copy *.cpp *.cc
	del gcaVel

# DEPENDENCIES

mvel.o: mvel.cpp 

caInva.o: caInva.cpp caInva.hpp cabase.hpp cabase.cpp  cellInva.hpp smattpl.hpp

cabase.o: cabase.cpp cabase.hpp

r250.o: r250.cpp r250.h

bgi.o: bgi.cpp

fortify.o : fortify.cpp fortify.h ufortify.h

clean:
	del *.o
	del *.bak
