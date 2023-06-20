#include "vm.h"
#include "chunk.h"

int main() {
  initVM();
  Chunk chunk;
  initChunk(&chunk);

  int constant = addConstant(&chunk, 1.2);
  writeChunk(&chunk, OP_CONSTANT, 1);
  writeChunk(&chunk, constant, 1);
  writeChunk(&chunk, OP_RETURN, 2);

  interpret(&chunk);

  freeVM();
  freeChunk(&chunk);
  return 0;
}
