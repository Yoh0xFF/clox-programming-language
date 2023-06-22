#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vm.h"
#include "chunk.h"

static void repl();

static void runFile(const char *path);

static char *readFile(const char *path);

int main(int argc, const char *argv[]) {
  initVM();
  Chunk chunk;
  initChunk(&chunk);

  if (argc == 1) {
    repl();
  } else if (argc == 2) {
    runFile(argv[1]);
  } else {
    fprintf(stderr, "Usage: clox [path]\n");
    exit(64);
  }

  /*
  int constant = addConstant(&chunk, 1.2);
  writeChunk(&chunk, OP_CONSTANT, 1);
  writeChunk(&chunk, constant, 1);

  constant = addConstant(&chunk, 3.4);
  writeChunk(&chunk, OP_CONSTANT, 2);
  writeChunk(&chunk, constant, 2);

  writeChunk(&chunk, OP_ADD, 3);

  constant = addConstant(&chunk, 5.6);
  writeChunk(&chunk, OP_CONSTANT, 4);
  writeChunk(&chunk, constant, 4);

  writeChunk(&chunk, OP_DIVIDE, 5);
  writeChunk(&chunk, OP_NEGATE, 6);
  writeChunk(&chunk, OP_RETURN, 7);

  interpret(&chunk);
   */

  freeVM();
  freeChunk(&chunk);
  return 0;
}

static void repl() {
  char line[1024];

  for (;;) {
    printf("=> ");

    if (!fgets(line, sizeof(line), stdin)) {
      printf("\n");
      break;
    }

    interpret(line);
  }
}

static void runFile(const char *path) {
  char *source = readFile(path);
  InterpretResult result = interpret(source);
  free(source);

  if (result == INTERPRET_COMPILE_ERROR) {
    exit(65);
  }
  if (result == INTERPRET_RUNTIME_ERROR) {
    exit(70);
  }
}

static char *readFile(const char *path) {
  FILE *file = fopen(path, "rb");
  if (file == NULL) {
    fprintf(stderr, "Count not open file \"%s\".\n", path);
    exit(74);
  }

  fseek(file, 0L, SEEK_END);
  size_t fileSize = ftell(file);
  rewind(file);

  char *buffer = (char *) malloc(fileSize + 1);
  if (buffer == NULL) {
    fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
    exit(74);
  }

  size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
  if (bytesRead < fileSize) {
    fprintf(stderr, "Could not read file \"%s\".\n", path);
    exit(74);
  }
  buffer[bytesRead] = '\0';

  fclose(file);
  return buffer;
}