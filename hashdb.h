#ifndef __hashdb_h__
#define __hashdb_h__

#include <stdint.h>

typedef struct hash_struct {
    uint32_t hash;
    char name[50];
    uint32_t salary;
    struct hash_struct *next;
} hashRecord;

void ht_init();
void ht_insert(const char *name, uint32_t salary);
void ht_remove(const char *name);
hashRecord* ht_search(const char *name);
uint32_t jenkins_one_at_a_time_hash(const uint8_t* key, size_t length);

#endif // __hashdb_h__