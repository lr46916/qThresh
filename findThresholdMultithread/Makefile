CC = g++
CFLAGS = -std=c++0x -O2 -Wall 

SRCS = memoryUtils.cpp findThreshold.cpp findThresholdMain.cpp precomputationUtils.cpp thresholdUtils.cpp
HEADERS = memoryUtils.hpp findThreshold.hpp precomputationUtils.hpp thresholdUtils.hpp
OBJS = $(SRCS:.cpp=.o) 
LINKING = -lboost_system -lboost_filesystem -lboost_thread
EXE = findAllThresholds 

all: $(SRCS) $(HEADERS)  $(EXE)

$(EXE): $(OBJS)
	$(CC) -g $(CFLAGS) $(OBJS) $(LINKING) -o $@

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXE) $(OBJS)
