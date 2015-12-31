.PHONY: build
.PHONY: test
.PHONY: sample
.PHONY: perf
.PHONY: xcode-proj
.PHONY: clean

build:
	 ./node_modules/.bin/node-gyp configure && node-gyp build

build-other:
	./node_modules/node-gyp/gyp/gyp_main.py --generator-output=./build --depth=. -f ninja other.gyp
	ninja -C build/out/Default -f build.ninja

test: build-other
	./build/out/Default/test || [ $$? -eq 0 ]

sample: build-other
	./build/out/Default/sample

perf: build-other
	./build/out/Default/perf

xcode-proj:
	node-gyp configure -- -f xcode

clean:
	rm -Rf build
