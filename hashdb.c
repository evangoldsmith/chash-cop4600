#include "hashdb.h"
#include "rwlock.h"
#include <stdlib.h>
#include <string.h>

#define SIZE 100

static hashRecord* hash_table;
static rwlock_t hash_lock;

uint32_t jenkins_one_at_a_time_hash(const uint8_t* key, size_t length) {
    size_t i = 0;
    uint32_t hash = 0;
    while (i != length) {
        hash += key[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;

}

// Search for a record
hashRecord* ht_search(const char *name) {

    // Calculate hash from input
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)name, strlen(name));

    // Acquire read lock
    rwlock_acquire_readlock(&hash_lock);

    // Search for record
    
}

// Insert a record
void ht_insert(const char *name, uint32_t salary) {
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)name, strlen(name));
    hashRecord *record = (hashRecord*)malloc(sizeof(hashRecord));
    record->hash = hash;
    strcpy(record->name, name);
    record->salary = salary;
}

int main() {
    ht_init();
    ht_insert("Alice", 1000);
    ht_insert("Bob", 2000);
    ht_insert("Charlie", 3000);
    hashRecord *record = ht_search("Bob");
    if (record != NULL) {
        printf("Bob's salary is %d\n", record->salary);
    }
    return 0;
}
