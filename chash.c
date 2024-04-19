#include <stdio.h>
#include <stdlib.h>
// #include <pthread.h>
// #include <unistd.h>
#include <string.h>

// #include "hashdb.h"
// #include "rwlock.h"

// #include "common.h"
// #include "common_threads.h"

// #ifdef linux
// #include <semaphore.h>
// #elif __APPLE__
// #include "zemaphore.h"
// #endif

int main()
{

    FILE *file;
    char ch;
    int lineNum = 0;
    int numThreads = 0;

    file = fopen("commands.txt", "r");

    if (file == NULL)
    {
        printf("Error opening file!\n");
        return 1;
    }

    char line[1000];
    char *token;

    fgets(line, sizeof(line), file);
    printf("%s\n", line);
    token = strtok(line, ",");
    while (token != NULL)
    {
        for (int i = 0; i < 3; i++)
        {
            if (i == 1)
            {
                numThreads = atoi(token);
                printf("Num Threads: %d\n", numThreads);
            }
            token = strtok(NULL, ",");
        }
    }
    // COMMANDS: 0 = insert, 1 = delete, 2 = search, 3 = print
    int command[numThreads];
    char **name = (char**)calloc(numThreads, sizeof(char*));
    int salary[numThreads];
    while (fgets(line, sizeof(line), file))
    {
        // printf("%s\n", line);
        token = strtok(line, ",");
        while (token != NULL)
        {
            for (int i = 0; i < 3; i++)
            {
                // printf("%s\n", token);
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
                    else
                    {
                        command[lineNum] = 3;
                    }
                }
                else if (i == 1)
                {
                    name[lineNum] = (char*)malloc(strlen(token) + 1);
                    strcpy(name[lineNum], token);
                }
                else
                {
                    salary[lineNum] = atoi(token);
                }
                token = strtok(NULL, ",");
            }
        }
        lineNum++;
    }

    for (int i = 0; i < numThreads; i++)
    {
        printf("%d\n", command[i]);
    }
    for (int i = 0; i < numThreads; i++)
    {
        printf("%s\n", name[i]);
    }
    for (int i = 0; i < numThreads; i++)
    {
        printf("%d\n", salary[i]);
    }
    for(int i = 0; i++; i < numThreads)
    {
        free(name[i]);
    }
    free(name);
    fclose(file);
    return 0;
}
