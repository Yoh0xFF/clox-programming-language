#ifndef CLOX_VALUE_H
#define CLOX_VALUE_H

#include "common.h"

typedef double Value;

typedef struct {
  int capacity;
  int count;
  Value *values;
} ValueArray;

void initValueArray(ValueArray *array);

void freeValueArray(ValueArray *array);

void writeValueArray(ValueArray *array, Value value);

void printValue(Value value);

#endif
