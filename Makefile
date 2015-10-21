CC=clang++
CFLAGS=-std=c++11 -c
EXEC=run
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

.PHONY: test

$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC)

%.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

test-release:
	 node-gyp configure && node-gyp build && ./build/Release/test

test:
	 node-gyp configure -debug && node-gyp build && ./build/Debug/test

xcode-proj:
	node-gyp configure -- -f xcode

clean:
	rm -Rf run build *.o

