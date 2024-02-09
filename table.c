#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "object.h"
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

bool tableGet(Table *table, ObjString *key, Value *value) {
  if (table->count == 0) {
    return false;
  }

  Entry *entry = findEntry(table->entries, table->capacity, key);
  if (entry->key == NULL) {
    return false;
  }

  *value = entry->value;
  return true;
}

bool tableSet(Table *table, ObjString *key, Value value) {
  if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
    int newCapacity = GROW_CAPACITY(table->capacity);
    adjustCapacity(table, newCapacity);
  }

  Entry *entry = findEntry(table->entries, table->capacity, key);
  bool isNewKey = entry->key == NULL;
  if (isNewKey && IS_NIL(entry->value)) {
    table->count++;
  }

  entry->key = key;
  entry->value = value;
  return isNewKey;
}

bool tableDelete(Table *table, ObjString *key) {
  if (table->count == 0) {
    return false;
  }

  Entry *entry = findEntry(table->entries, table->capacity, key);
  if (entry->key == NULL) {
    return false;
  }

  // Place tombstone
  entry->key = NULL;
  entry->value = BOOL_VAL(true);
  return true;
}

void tableAddAll(Table *from, Table *to) {
  for (int i = 0; i < from->capacity; ++i) {
    Entry *entry = &from->entries[i];

    if (entry->key != NULL) {
      tableSet(to, entry->key, entry->value);
    }
  }
}

ObjString *tableFindString(Table *table, const char *chars, int length,
                           uint32_t hash) {
  if (table->count == 0) {
    return NULL;
  }

  uint32_t index = hash & (table->capacity - 1); // Same as "hash % table->capacity;"
  for (;;) {
    Entry *entry = &table->entries[index];
    if (entry->key == NULL) {
      // Stop if we find an empty non-tombstone entry.
      if (IS_NIL(entry->value)) {
        return NULL;
      }
    } else if (entry->key->length == length && entry->key->hash == hash &&
               memcmp(entry->key->chars, chars, length) == 0) {
      // We found it.
      return entry->key;
    }

    index = (index + 1) & (table->capacity - 1); // Same as "(index + 1) % table->capacity;"
  }
}

void tableRemoveWhite(Table *table) {
  for (int i = 0; i < table->capacity; i++) {
    Entry *entry = &table->entries[i];
    if (entry->key != NULL && !entry->key->obj.isMarked) {
      tableDelete(table, entry->key);
    }
  }
}

void markTable(Table *table) {
  for (int i = 0; i < table->capacity; i++) {
    Entry *entry = &table->entries[i];
    markObject((Obj *)entry->key);
    markValue(entry->value);
  }
}

static Entry *findEntry(Entry *entries, int capacity, ObjString *key) {
  uint32_t index = key->hash & (capacity - 1); // Same as "key->hash % capacity;" works only for power of two numbers.
  Entry *tombstone = NULL;

  for (;;) {
    Entry *entry = &entries[index];

    if (entry->key == NULL) {
      // Empty entry.
      if (IS_NIL(entry->value)) {
        return tombstone != NULL ? tombstone : entry;
      }

      // We found a tombstone.
      if (tombstone == NULL) {
        tombstone = entry;
      }

      continue;
    }

    if (entry->key == key) {
      // We found the key.
      return entry;
    }

    index = (index + 1) & (capacity - 1); // Same as "(index + 1) % capacity;"
  }
}

static void adjustCapacity(Table *table, int capacity) {
  // Allocate new array
  Entry *destEntries = ALLOCATE(Entry, capacity);
  for (int i = 0; i < capacity; i++) {
    destEntries[i].key = NULL;
    destEntries[i].value = NIL_VAL;
  }

  // Move destEntries from the old to the new array
  table->count = 0;
  for (int i = 0; i < table->capacity; i++) {
    Entry *entry = &table->entries[i];
    if (entry->key == NULL) {
      continue;
    }

    Entry *destEntry = findEntry(destEntries, capacity, entry->key);
    destEntry->key = entry->key;
    destEntry->value = entry->value;
    table->count++;
  }
  FREE_ARRAY(Entry, table->entries, table->capacity);

  // Replace array in the table
  table->entries = destEntries;
  table->capacity = capacity;
}