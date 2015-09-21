OBJ = iid_mc_model.o match.o mc_model.o monte_carlo.o point.o score.o service_game.o set.o tiebreak.o adjusted_mc_model.o
executable = monte_carlo
GCC = g++
CFLAGS = -Wall -O3 -g -std=c++11 -MMD
LIBPATHS = 
LIBS = 
INCLUDES = 
RPATH = 
OPENCV = 

$(executable): $(OBJ)
		$(GCC) $(RPATH) $(CFLAGS) $(INCLUDES) $(OBJ) $(LIBPATHS) -o $(executable) $(OPENCV) $(LIBS)

%.o: %.cc
		$(GCC) $(CFLAGS) $(INCLUDES) -c $<

-include $(OBJ:.o=.d)

.PHONY: clean

clean:
		rm -f $(OBJ) $(executable)
