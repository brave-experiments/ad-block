.PHONY: build
.PHONY: build-debug
.PHONY: test
.PHONY: static

build:
	 node-gyp configure && node-gyp rebuild

build-debug:
	 node-gyp configure -debug && node-gyp rebuild

test:
	 node-gyp configure -debug && node-gyp build && ./build/Debug/test

sample:
	 node-gyp && node-gyp build && ./build/Release/sample

xcode-proj:
	node-gyp configure -- -f xcode

clean:
	node-gyp clean

