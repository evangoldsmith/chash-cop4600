#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#include "hashdb.h"
#include "rwlock.h"

// #include "common.h"
// #include "common_threads.h"

#ifdef linux
#include <semaphore.h>
#elif __APPLE__
#include "zemaphore.h"
#endif

// void functionCalls();

// Struct to hold command, name, and salary
typedef struct {
    int thread_id;
    int command;
    char *name;
    int salary;
} ThreadArgs;

void *functionCall(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    if (args->command == 0) {
        ht_insert(args->name, args->salary);
    } else if (args->command == 1) {
        ht_remove(args->name);
    } else if (args->command == 2) {
        ht_search(args->name);
    } else if (*&args->command == 3) {
        ht_print_all();
    }
    // Free memory allocated for name
    free(args->name);
    // Free memory allocated for arg
    free(args);
    return NULL;
}
void *printCall(void*arg) {
    int *args = (int *)arg;
    if (*args == 0) {
        print_lock_acquisitions();
    } else if (*args == 1) {
        ht_print_all_final();
    }
    free(args);
    return NULL;
}

int main()
{

    FILE *file;
    int lineNum = 0;
    int numThreads = 0;
    init_output_file();

    file = fopen("commands.txt", "r");

    if (file == NULL)
    {
        printf("Error opening file!\n");
        return 1;
    }

    char line[1000];
    char *token;

    fgets(line, sizeof(line), file);
    token = strtok(line, ",");
    while (token != NULL)
    {
        for (int i = 0; i < 3; i++)
        {
            if (i == 1)
            {
                numThreads = atoi(token);
                print_to_output("Num Threads: %d\n", numThreads);
            }
            token = strtok(NULL, ",");
        }
    }
    // COMMANDS: 0 = insert, 1 = delete, 2 = search, 3 = print
    int command[100];
    char **name = (char **)calloc(numThreads, sizeof(char *));
    int salary[100];

    while (fgets(line, sizeof(line), file))
    {
        token = strtok(line, ",");
        while (token != NULL)
        {
            for (int i = 0; i < 3; i++)
            {
                if (i == 0)
                {
                    if (strcmp("insert", token) == 0)
                    {
                        command[lineNum] = 0;
                    }
                    else if (strcmp("delete", token) == 0)
                    {
                        command[lineNum] = 1;
                    }
                    else if (strcmp("search", token) == 0)
                    {
                        command[lineNum] = 2;
                    }
                    else if(strcmp("print", token) == 0)
                    {
                        command[lineNum] = 3;
                    }
                }
                else if (i == 1)
                {
                    name[lineNum] = (char *)malloc(strlen(token) + 1);
                    strcpy(name[lineNum], token);
                }
                else
                {
                    salary[lineNum] = atoi(token);
                }
                token = strtok(NULL, ",");
            }
        lineNum++;
        }
    }

    
    pthread_t t[numThreads];

    for (int i = 0; i < numThreads; i++) {
        ThreadArgs *args = (ThreadArgs *)malloc(sizeof(ThreadArgs));
        args->thread_id = i;
        args->command = command[i];
        args->name = strdup(name[i]);
        args->salary = salary[i];


        pthread_create(&t[i], NULL, functionCall, args);
    }
    for (int i = 0; i < numThreads; i++) {
        pthread_join(t[i], NULL);
    }
    for (int i = 0; i < 2; i++) {
        int *args = malloc(sizeof *args);
        *args = i;
        pthread_create(&t[i], NULL, printCall, (void*)args);

    }
        for (int i = 0; i < 1; i++) {
        pthread_join(t[i], NULL);
    }
    pthread_exit(NULL);

    for (int i = 0; i < numThreads; i++) {
        free(name[i]);
    }
    free(name);
    return 0;
}