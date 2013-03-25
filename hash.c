#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct hash_entry {
	char key[64];
	int value;
	struct hash_entry *chain;
} hash_entry;

typedef struct hash_table {
	hash_entry *table;
	int table_length;
} hash_table;

hash_table *make_hash() {
	hash_table *hash = malloc(sizeof(hash_table));
	hash->table = calloc(1024, sizeof(hash_entry));
	hash->table_length = 1024;
	return hash;
}

int _hashify(const char *key, int table_length) {
	int value = 0xdeadbeef;
	char c;
	while((c = *key++)) {
		value = value * 23 + c;
	}
	value %= table_length;
	return value < 0 ? value + table_length : value;
}

void hash_put(hash_table *hash, const char *key, int value) {
	int slot = _hashify(key, hash->table_length);
	hash_entry *entry = &(hash->table[slot]);
	while(entry->key[0] && strcmp(key, entry->key)) {
		if(!entry->chain) {
			entry->chain = calloc(sizeof(hash_entry), 1);
		}
		entry = entry->chain;
	}
	strcpy(entry->key, key);
	entry->value = value;
}

int hash_get(hash_table *hash, const char *key, int dfault) {
	int slot = _hashify(key, hash->table_length);
	hash_entry *entry = &(hash->table[slot]);
	while(entry && strcmp(key, entry->key)) {
		entry = entry->chain;
	}
	if(!entry) return dfault;
	return entry->value;
}
