INCLUDE_DIR = ./raylib/src
LIB_DIR = ./raylib/src
CFLAGS = -I$(INCLUDE_DIR) -O3 -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=4 -s WASM=1 -s SHARED_MEMORY=1 -s BINARYEN_ASYNC_COMPILATION=0
LDFLAGS = -L$(LIB_DIR) -Wall -Wextra -g -lraylib -lm -lpthread -ldl -lrt -lX11
EM_LDFLAGS = -L$(LIB_DIR) -Wall -Wextra -g -lraylib -lglfw -s USE_GLFW=3 -s ASYNCIFY -s FORCE_FILESYSTEM=1 -O3 --preload-file assets --shell-file custom_shell.html -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=4 -s WASM=1 -s SHARED_MEMORY=1 -s BINARYEN_ASYNC_COMPILATION=0

dino: dino.c
	gcc -o dino dino.c $(CFLAGS) $(LDFLAGS)

dino_web: dino.c
	emcc -o index.html dino.c -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result -Os \
	-I. -I $(INCLUDE_DIR) -I $(INCLUDE_DIR)/external -L. -L $(LIB_DIR) \
	-s USE_GLFW=3 -s ASYNCIFY -s TOTAL_MEMORY=67108864 -s FORCE_FILESYSTEM=1 --shell-file custom_shell.html \
	--preload-file assets \
	$(LIB_DIR)/web/libraylib.a -DPLATFORM_WEB \
	-s 'EXPORTED_FUNCTIONS=["_free","_malloc","_main"]' -s EXPORTED_RUNTIME_METHODS=ccall

clean:
	del dino.exe index.html index.data index.js index.wasm