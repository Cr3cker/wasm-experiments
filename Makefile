INCLUDE_DIR = ./raylib/src
LIB_DIR = ./raylib/src
CFLAGS = -I$(INCLUDE_DIR) -O3
LDFLAGS = -L$(LIB_DIR) -Wall -Wextra -g -lraylib -lm -lpthread -ldl -lrt -lX11
EM_LDFLAGS = -L$(LIB_DIR) -Wall -Wextra -g -lraylib -s USE_GLFW=3 -s ASYNCIFY -s FORCE_FILESYSTEM=1 -O3 --preload-file assets --shell-file custom_shell.html

dino: dino.c
	gcc -o dino dino.c $(CFLAGS) $(LDFLAGS)

dino.html: dino.c libraylib.a
	emcc -o dino.html dino.c $(CFLAGS) $(EM_LDFLAGS) -s EXPORTED_FUNCTIONS='["_main", "_InitPlatform", "_ClosePlatform", "_GetTime", "_GetWindowScaleDPI", "_SwapScreenBuffer", "_PollInputEvents", "_WindowShouldClose"]' -s EXPORTED_RUNTIME_METHODS=ccall

clean:
	rm -f dino dino.html