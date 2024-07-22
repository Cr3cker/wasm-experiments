# Dino WebAssembly Game

This project demonstrates how to create a simple game structure in C, compile it to WebAssembly using Emscripten, and interact with it in a web environment.

## Files Included
- `dino.c`: The main C code file.
- `index.html`: HTML file to load and interact with the WebAssembly module.
- `Makefile`: Makefile for compiling the project.

## How to Compile

### Native Compilation
To compile the C code natively:
```bash
make dino
```

### To remove compiled files:
```bash
make clean
```

### To compile the C code to WebAssembly:
```bash
make dino_web
```

### Running a web server
```bash
python3 -m http.server
```

