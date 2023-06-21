#ifndef CLOX_CHUNK_H
#define CLOX_CHUNK_H

#include "common.h"
#include "value.h"

typedef enum {
  OP_CONSTANT,
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_NEGATE,
  OP_RETURN,
} OpCode;

typedef struct {
  int count;
  int capacity;
  uint8_t *code;
  uint32_t *lines;
  ValueArray constants;
} Chunk;

void initChunk(Chunk *chunk);

void freeChunk(Chunk *chunk);

void writeChunk(Chunk *chunk, uint8_t byte, uint32_t line);

int addConstant(Chunk *chunk, Value value);

#endif
