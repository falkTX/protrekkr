LIBFLAGS = -c
LIBOBJS = replay.o cubic_spline.o synth.o tb_303.o samples_unpack.o sounddriver.o

all: ptkreplay
clean: remlibobj

remlibobj:
	-rm -f $(LIBOBJS)
	
ptkreplay: $(LIBOBJS)
	ar -d libptkreplay.a
	ar -r libptkreplay.a $(LIBOBJS)
	-rm -f $(LIBOBJS)

sounddriver.o: lib/sounddriver/sounddriver.cpp
	g++ -o $(@) $(LIBFLAGS) $(FLAGS) lib/sounddriver/sounddriver.cpp

samples_unpack.o: lib/samples_unpack.cpp
	g++ -o $(@) $(LIBFLAGS) $(FLAGS) lib/samples_unpack.cpp

tb_303.o: lib/tb_303.cpp
	g++ -o $(@) $(LIBFLAGS) $(FLAGS) lib/tb_303.cpp

synth.o: lib/synth.cpp
	g++ -o $(@) $(LIBFLAGS) $(FLAGS) lib/synth.cpp

cubic_spline.o: lib/cubic_spline.cpp
	g++ -o $(@) $(LIBFLAGS) $(FLAGS) lib/cubic_spline.cpp

replay.o: lib/replay.cpp
	g++ -o $(@) $(LIBFLAGS) $(FLAGS) lib/replay.cpp
