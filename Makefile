OBJS = assembler.o executableBuffer.o
LDFLAGS ?= -Wl,-O1 -Wl,--as-needed
CFLAGS += -Wall -std=c++17
BINS = main


%.o: %.cpp %.h
	g++ $(CFLAGS) -c $*.cpp

build: $(OBJS)
	g++ $(LDFLAGS) $(CFLAGS) $(OBJS) main.cpp -o main

clean:
	rm -f $(OBJS)

rebuild: clean build

test: clean $(OBJS)
	g++ $(LDFLAGS) $(CFLAGS) $(OBJS) test.cpp -o main


run: build
	./main
