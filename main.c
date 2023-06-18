#include "chunk.h"
#include <stdio.h>

int main() {
  Chunk chunk;
  initChunk(&chunk);
  writeChunk(&chunk, OP_RETURN);
  freeChunk(&chunk);
  printf("Hello, World!\n");
  return 0;
}
