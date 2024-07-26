# Cube WebAssembly Game

This project demonstrates how to create a simple game structure in C, compile it to WebAssembly using Emscripten, and interact with it in a web environment.

## Files Included
- `cube.c`: The main C code file.
- `index.html`: HTML file to load and interact with the WebAssembly module.

## How to Compile

### To compile the C code to WebAssembly:
```bash
emcc cube.c -o cube.js -s WASM=1 -s "EXPORTED_FUNCTIONS=['_malloc']" -s "EXPORTED_RUNTIME_METHODS=['cwrap', 'getValue']"
```

### Running a web server
```bash
python3 -m http.server
```

