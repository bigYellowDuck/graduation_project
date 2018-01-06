CXX = g++

CXXFLAGS = -O2 -std=c++11

LDFFLAGS = -lev -pthread

INCLUDE = -I. -I../include

objects =  main.o event_loop.o threads.o util.o socket.o inet_address.o \
					 acceptor.o logging.o udpkeeper.o tinyxml2.o

main : $(objects)
	$(CXX) -o $@ $^ $(INCLUDE) $(CXXFLAGS) $(LDFFLAGS)

$(objects) : %.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) $< -o $@

.PHONY : clean
clean:
	rm main $(objects)
