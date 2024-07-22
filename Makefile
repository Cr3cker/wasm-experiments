dino: dino.c
	gcc -o dino -Wall -Wextra dino.c

clean:
	-rm -rf dino dino.wasm dino.js

dino_web:
	emcc -o dino.js dino.c -O2 -s EXIT_RUNTIME=1 -s EXPORTED_FUNCTIONS="['_malloc']" -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap", "getValue", "setValue"]'