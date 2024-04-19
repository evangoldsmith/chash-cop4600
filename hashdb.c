#include "hashdb.h"
#include "rwlock.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>


static hashRecord* hash_table; // Pointer to the head of the linked list
static rwlock_t hash_lock;
int lockAcquisition = 0;
int releaseAcquisition = 0;

FILE *output_fp; // File pointer for output file

void init_output_file() {
    output_fp = fopen("output.txt", "w"); // Open output file in write mode
    if (output_fp == NULL) {
        fprintf(stderr, "Error opening output file.\n");
        exit(EXIT_FAILURE);
    }
}

void print_to_output(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(output_fp, format, args); // Write formatted output to the file
    va_end(args);
}

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

void ht_init() {
    hash_table = NULL; // Initialize head to NULL
    rwlock_init(&hash_lock);
}

hashRecord* ht_search(const char *name) {
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)name, strlen(name));
    hashRecord *record = NULL;
    // Acquire read lock
    lockAcquisition++;
    print_to_output("READ LOCK ACQUIRED\n"); // Release read lock
    rwlock_acquire_readlock(&hash_lock);
    print_to_output("SEARCH,%s\n", name); // Print search command
    
    // Search for record in the linked list
    hashRecord *current = hash_table;
    while (current != NULL) {
        if (current->hash == hash && strcmp(current->name, name) == 0) {
            // Found the record
            print_to_output("READ LOCK RELEASED\n"); // Release read lock
            rwlock_release_readlock(&hash_lock);
            releaseAcquisition++;
            print_to_output("%u,%s,%u\n", current->hash, current->name, current->salary); // Print record details
            return current;
        }
        current = current->next;
    }
    // Record not found
    releaseAcquisition++;
    print_to_output("READ LOCK RELEASED\n"); // Release read lock
    rwlock_release_readlock(&hash_lock);
    return NULL;
}

void ht_insert(const char *name, uint32_t salary) {
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)name, strlen(name));
    hashRecord *record = (hashRecord*)malloc(sizeof(hashRecord));
    record->hash = hash;
    strcpy(record->name, name);
    record->salary = salary;
    record->next = NULL; // Initialize next pointer to NULL
    
    print_to_output("INSERT,%u,%s,%u\n", record->hash, record->name, record->salary); // Print insert command
    lockAcquisition++;
    // Acquire write lock
    print_to_output("WRITE LOCK ACQUIRED\n");
    rwlock_acquire_writelock(&hash_lock);
    
    // Insert record into the linked list at the appropriate position (sorted by hash)
    if (hash_table == NULL || hash_table->hash > hash) {
        record->next = hash_table;
        hash_table = record; // Set record as the new head
    } else {
        hashRecord *prev = hash_table;
        hashRecord *current = hash_table->next;
        while (current != NULL && current->hash < hash) {
            prev = current;
            current = current->next;
        }
        prev->next = record;
        record->next = current;
    }
    releaseAcquisition++;
    print_to_output("WRITE LOCK RELEASED\n"); // Release write lock
    rwlock_release_writelock(&hash_lock);
}

void ht_remove(const char *name) {
    // Calculate hash from input
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)name, strlen(name));
    // Acquire write lock
    print_to_output("WRITE LOCK ACQUIRED\n");
    lockAcquisition++;
    rwlock_acquire_writelock(&hash_lock);
    print_to_output("DELETE,%s\n", name); // Print delete command
    
    // Search for record to remove
    hashRecord *prev = NULL;
    hashRecord *current = hash_table;
    while (current != NULL) {
        if (current->hash == hash && strcmp(current->name, name) == 0) {
            // Found the record to remove
            if (prev != NULL) {
                prev->next = current->next;
            } else {
                hash_table = current->next;
            }
            releaseAcquisition++;
            print_to_output("WRITE LOCK RELEASED\n"); // Release write lock
            rwlock_release_writelock(&hash_lock);
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
    // Release write lock if record not found
    releaseAcquisition++;
    print_to_output("WRITE LOCK RELEASED\n"); // Release write lock
    rwlock_release_writelock(&hash_lock);
    print_to_output("Record not found\n");
}

void ht_print_all() {
    // Acquire read lock
    print_to_output("READ LOCK ACQUIRED\n");
    lockAcquisition++;
    rwlock_acquire_readlock(&hash_lock);
    
    hashRecord *current = hash_table;
    while (current != NULL) {
        print_to_output("%u,%s,%u\n", current->hash, current->name, current->salary);
        current = current->next;
    }
    releaseAcquisition++;
    print_to_output("READ LOCK RELEASED\n");
    // Release read lock
    rwlock_release_readlock(&hash_lock);
}

void print_lock_acquisitions() {
    lockAcquisition++;
    rwlock_acquire_readlock(&hash_lock);
    releaseAcquisition++;
    print_to_output("Number of lock acquisitions: %d\n", lockAcquisition);
    print_to_output("Number of lock releases: %d\n", releaseAcquisition);
    rwlock_release_readlock(&hash_lock);
}

void ht_print_all_final() {
    // Acquire read lock
    lockAcquisition++;
    print_to_output("READ LOCK ACQUIRED\n");
    rwlock_acquire_readlock(&hash_lock);

    hashRecord *records = hash_table;

    hashRecord *i = records;
    while (i != NULL) {
        hashRecord *j = records;
        while (j->next != NULL) {
            if (j->hash > j->next->hash) {
                // Swap pointers
                hashRecord *temp = j;
                j = j->next;
                j->next = temp;
            }
            j = j->next;
        }
        i = i->next;
    }

    // Print sorted records
    hashRecord *current = records;
    while (current != NULL) {
        print_to_output("%u,%s,%u\n", current->hash, current->name, current->salary);
        current = current->next;
    }


    releaseAcquisition++;
    print_to_output("READ LOCK RELEASED\n");
    rwlock_release_readlock(&hash_lock);

    current = records;
    while (current != NULL) {
        hashRecord *temp = current->next;
        free(current);
        current = temp;
    }
}




