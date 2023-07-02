#ifndef CLOX_OBJECT_H
#define CLOX_OBJECT_H

#include "common.h"
#include "value.h"

typedef enum {
  OBJ_STRING,
} ObjectType;

struct Obj {
  ObjectType type;
};

struct ObjString {
  Obj obj;
  int length;
  char *chars;
};

static inline bool isObjectType(Value value, ObjectType type) {
  return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

#define OBJECT_TYPE(value) (AS_OBJ(value)->type)

#define IS_STRING(value) isObjectType(value, OBJ_STRING)

#define AS_STRING(value) ((ObjString *) AS_OBJ(value))
#define AS_CSTRING(value) (((ObjString *) AS_OBJ(value))->chars)

#endif
