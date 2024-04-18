#include "hashdb.h"
#include "rwlock.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


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
// Initialize the hash table
void ht_init() {
    hash_table = (hashRecord*)malloc(SIZE * sizeof(hashRecord));
    memset(hash_table, 0, SIZE * sizeof(hashRecord));
    rwlock_init(&hash_lock);
}



// Search for a record
hashRecord* ht_search(const char *name) {
    // Calculate hash from input
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)name, strlen(name));
    // Acquire read lock
    rwlock_acquire_readlock(&hash_lock);

    
    // Search for record
    hashRecord *current = &hash_table[hash % SIZE];
    while (current != NULL) {
        if (current->hash == hash && strcmp(current->name, name) == 0) {
            // Found the record
            printf("READ LOCK ACQUIRED search\n");
            rwlock_release_readlock(&hash_lock);

            // printf("%s\n", current->name);
            return current;
        }
        current = current->next;
    }
    // Record not found
    printf("READ LOCK REMOVED search\n");
    rwlock_release_readlock(&hash_lock);

    return NULL;
}

// Remove a record
void ht_remove(const char *name) {
    // Calculate hash from input
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)name, strlen(name));
    
    // Acquire write lock
    printf("WRITE LOCK ACQUIRED remove\n");
    rwlock_acquire_writelock(&hash_lock);
    
    // Determine the bucket index
    int index = hash % SIZE;
    
    // Search for record to remove
    hashRecord *prev = NULL;
    hashRecord *current = &hash_table[index];

    while (current != NULL) {
        if (current->hash == hash && strcmp(current->name, name) == 0) {
            // Found the record to remove
            if (prev != NULL) {
                prev->next = current->next;
            } else {
                hash_table[hash % SIZE] = *(current->next);
            }
            // Release write lock

            printf("WRITE LOCK ACQUIRED remove while\n");
            rwlock_release_writelock(&hash_lock);
            return;
        }
        prev = current;
        current = current->next;
    }
    
    // Release write lock if record not found
    printf("WRITE LOCK REMOVED remove\n");
    rwlock_release_writelock(&hash_lock);

}



// Insert a record
void ht_insert(const char *name, uint32_t salary) {
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)name, strlen(name));
    hashRecord *record = (hashRecord*)malloc(sizeof(hashRecord));
    record->hash = hash;
    strcpy(record->name, name);
    record->salary = salary;
    // Acquire write lock
    printf("WRITE LOCK ACQUIRED insert\n");
    rwlock_acquire_writelock(&hash_lock);
    // Insert record into hash table
    record->next = &hash_table[hash % SIZE];
    hash_table[hash % SIZE] = *record;
    printf("WRITE LOCK RELEASED insert\n");
    rwlock_release_writelock(&hash_lock);

}

// int main() {
//     ht_init();
//     ht_insert("Alice", 1000);
//     ht_insert("Bob", 2000);
//     ht_insert("Charlie", 3000);
//     ht_remove("Bob");
//     ht_search("Alice");
//     ht_search("Bob is not removed");
//     hashRecord *record = ht_search("Bob");
//     if (record != NULL) {
//         printf("Bob's salary is %d\n", record->salary);
//     }
//     return 0;
// }
