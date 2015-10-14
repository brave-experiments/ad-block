CC=clang++
CFLAGS=-std=c++11 -c
EXEC=run
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

.PHONY: test

$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC)

%.o: %.cpp
	$(CC) -c $(CC_FLAGS) $< -o $@

test:
	 node-gyp configure && node-gyp build && ./build/Release/test

clean:
	rm -Rf run build *.o

