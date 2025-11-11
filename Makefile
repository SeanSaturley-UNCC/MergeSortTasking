CFLAGS=-O3 -std=c11 -fPIC -g
CXXFLAGS=-O3 -std=c++17 -fPIC -g
LD=g++

SRCS_SEQ = mergesort_seq_nocopy.cpp
OBJS_SEQ = $(SRCS_SEQ:.cpp=.o)

SRCS_PAR = mergesort_par_nocopy.cpp
OBJS_PAR = $(SRCS_PAR:.cpp=.o)

ALL_TARGETS = mergesort_seq mergesort_par

all: $(ALL_TARGETS)

# sequential
mergesort_seq: $(OBJS_SEQ)
	$(LD) $(LDFLAGS) $(OBJS_SEQ) $(ARCHIVES) -o $@

# parallel
mergesort_par: $(OBJS_PAR) omp_tasking.hpp
	$(LD) $(LDFLAGS) $(OBJS_PAR) $(ARCHIVES) -fopenmp -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

bench: mergesort_seq
	./queue.sh

clean:
	-rm *.o
	-rm mergesort_seq
	-rm mergesort_par

distclean:
	-rm *.sh.*

.PHONY: mergesort_seq mergesort_par all bench clean distclean
