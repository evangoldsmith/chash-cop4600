# chash-cop4600
Concurrent Hash Table

To run the program on Windows Powershell: type "make" in the terminal, followed by "./chash"
Another way to run the program is to type "gcc -o chash rwlock.c hashdb.c chash.c -lpthread" followed by "./chash"

To run the program on Mac zsh: type "make" in the terminal, followed by "./clash"

To our best efforts, it does not seem to function on wsl on Windows but functions on everything else. Most likely an issue with compatibility.

Use of AI: AI was used when making the hashdb.c function. Unfortunately, it's result had to be scrapped for a similar but different result. Originally, it was used to make the linked list but it kept making the linked list using an array of linked list which was causing a plethora of problems with running the program. We decided to just change it manually using the same logic as the one that we got from Chatgpt but using an actual linked list implementation this time. AI was also used for making the Makefile, others in my group knew how to make one but I never made one before and had no knowledge of it so I used it for assistance. Although it still needed human input since it kept putting them into a src and obj folder so in the end, I still had to look up how to make a makefile to realize this and reference the .c files outside of the src folder and how to access all of them instead dynamically instead of having to type in every .c file.

Prompts: "using this, create a hashdb.c file that handles the function calls of the chash.c file" 
"this is the struct for it "typedef struct {
    int thread_id;
    int command;
    char *name;
    int salary;
} ThreadArgs;"" We gave it the chash.c file we created. It gave a close enough result but was missing some information so we provided it with the struct as well.
"how can I create a makefile for my code"
