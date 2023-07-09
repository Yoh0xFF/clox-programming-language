#include <stdlib.h>
#include <string.h>

#include "object.h"
#include "memory.h"
#include "table.h"
#include "value.h"

#define TABLE_MAX_LOAD 0.75

static Entry *findEntry(Entry *entries, int capacity, ObjString *key);

static void adjustCapacity(Table *table, int capacity);

void initTable(Table *table) {
  table->count = 0;
  table->capacity = 0;
  table->entries = NULL;
}

void freeTable(Table *table) {
  FREE_ARRAY(Entry, table->entries, table->capacity);
  initTable(table);
}

bool tableSet(Table *table, ObjString *key, Value value) {
  if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
    int newCapacity = GROW_CAPACITY(table->capacity);
    adjustCapacity(table, newCapacity);
  }

  Entry *entry = findEntry(table->entries, table->capacity, key);
  bool isNewKey = entry->key == NULL;
  if (isNewKey) {
    table->count++;
  }

  entry->key = key;
  entry->value = value;
  return isNewKey;
}

void tableAddAll(Table *from, Table *to) {
  for (int i = 0; i < from->capacity; ++i) {
    Entry *entry = &from->entries[i];

    if (entry->key != NULL) {
      tableSet(to, entry->key, entry->value);
    }
  }
}

static Entry *findEntry(Entry *entries, int capacity, ObjString *key) {
  uint32_t index  = key->hash % capacity;

  for (;;) {
    Entry *entry = &entries[index];
    if (entry->key == key || entry->key == NULL) {
      return entry;
    }

    index = (index + 1) % capacity;
  }
}

static void adjustCapacity(Table *table, int capacity) {
  // Allocate new array
  Entry *entries = ALLOCATE(Entry, capacity);
  for (int i = 0; i < capacity; i++) {
    entries[i].key = NULL;
    entries[i].value = NIL_VAL;
  }

  // Move entries from the old to the new array
  for (int i = 0; i < table->capacity; i++) {
    Entry *entry = &table->entries[i];
    if (entry->key == NULL) {
      continue;
    }

    Entry *destEntry = findEntry(entries, capacity, entry->key);
    destEntry->key = entry->key;
    destEntry->value = entry->value;
  }
  FREE_ARRAY(Entry, table->entries, table->capacity);

  // Replace array in the table
  table->entries = entries;
  table->capacity = capacity;
}