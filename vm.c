#include <stdio.h>
#include "common.h"
#include "vm.h"
#include "debug.h"

VM vm;

static InterpretResult run();

void initVM() {

}

void freeVM() {

}

InterpretResult interpret(Chunk *chunk) {
  vm.chunk = chunk;
  vm.ip = chunk->code;
  return run();
}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])

  for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
    disassembleInstruction(vm.chunk, vm.ip - vm.chunk->code);
#endif
    uint8_t instuction;
    switch (instuction = READ_BYTE()) {
      case OP_CONSTANT: {
        Value constant = READ_CONSTANT();
        printValue(constant);
        printf("\n");
        break;
      }
      case OP_RETURN:
        return INTERPRET_OK;
    }
  }

#undef READ_BYTE
#undef READ_CONSTANT
}