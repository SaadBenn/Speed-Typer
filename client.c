#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include<stdbool.h>

// forward reference
int spawn_threads();
//pthread_t threads[2];


char word_buffer[10];
bool ready_for_word = true;
pthread_cond_t read_word_signal;
bool done = false;
int status_read;

int fd;
char word[5];
bool signaled = false;
pthread_cond_t signalAvailable;
pthread_mutex_t signalLock;
char message_length;
char pid_client[10];
//char curr_pid[10];
const char SERVER_FIFO[] = "./_pipe";

int main() {
    
    // Create Client FIFO
    printf("Creating client's FIFO!\n");

    sprintf(pid_client, "%d", getpid());
    char *fifo_name = (char *) malloc (sizeof(SERVER_FIFO) + sizeof(pid_client) + 1);
    strcat(fifo_name, SERVER_FIFO);
    strcat(fifo_name, pid_client);

    printf("Client creating a named pipe %s\n", fifo_name);

    int send = mkfifo(fifo_name, 0666);
    if (send < 0) {
        printf("Client FIFO failed to create. It likely already exists and you can ignore this warning!\n");
    
    } else {
        printf("Created Client FIFO!\n");
    }

    printf("Client pid is: %s\n", pid_client);
    
    //Open server fifo for registering the client
    fd = open(SERVER_FIFO, O_WRONLY);
    if (fd < 0) {
        perror("Cannot open server's fifo\n");
        return EXIT_FAILURE;
    }

    message_length = (char)strlen(pid_client);
    
    printf("The client's pid is: %s\n", pid_client);
    // Send string's length to the server
    write(fd, &message_length, 1);
    // Send string characters               
    write(fd, &pid_client, message_length); 

    printf("%s\n", "Mama there goes your man.");

    spawn_threads(fifo_name);

    unlink(fifo_name);
    return EXIT_SUCCESS;
}


void* incoming_messages(void *arg) {
    char *fifo_name = (char*) arg;

    int read_fifo_status = open(fifo_name, O_RDONLY);
    if (read_fifo_status < 0) {
        perror("Error! Cannot open clients's pipe");
        pthread_exit(0);
    }

    char length;
    while (!done) {
        int status_ = read(read_fifo_status, &length, 1);

        if (status_ == 0) {
            printf("%s\n", "Eror reading the client\n");
            pthread_exit(0);
        }

        read(read_fifo_status, &word_buffer, length);
        if (strcmp(word_buffer, "WINNER") == 0) {
            done = true;
            printf("%s\n", "WINNER!");
        
        } else if (strcmp(word_buffer, "LOSER") == 0) {
            done = true;
            printf("%s\n", "LOSER");
        
        }
        pthread_mutex_lock(&signalLock);
        signaled = true;
        pthread_cond_signal(&signalAvailable);
        pthread_mutex_unlock(&signalLock);
    }

    pthread_exit(0);
}


void* incoming_stdin(void *arg) {

    char *spelled_word;
    
    while (!done) {

        pthread_mutex_lock(&signalLock);
        while (!signaled) {
            pthread_cond_wait(&signalAvailable, &signalLock);
        }

        signaled = false;
        pthread_mutex_unlock(&signalLock);

        if (done) {
            pthread_exit(0);
        }

        spelled_word = malloc(sizeof(10));
        printf("Spell the word: %s\n", word_buffer);
        fgets(spelled_word, 10, stdin);
        spelled_word[strlen(spelled_word) -1] = '\0';
        //printf("%s\n", );
        
        printf("We got the user input in thread 2 %s\n", spelled_word);

        int server_status = open(SERVER_FIFO, O_WRONLY);
        if (server_status < 0) {
            perror("Error opening server fifo.");
            pthread_exit(0);
        }
        
        char *word_pid = malloc(strlen(spelled_word) + message_length + 2);
        
        // write spelled word + " " + pid to the server's pipe 
        strcat(word_pid, spelled_word);
        strcat(word_pid, " ");
        strcat(word_pid, pid_client);
        
        printf("The word + pid in the client is: %s\n", word_pid);

        char word_pid_length = strlen(word_pid);
        write(server_status, &word_pid_length, 1);
        int success = write(server_status, word_pid, word_pid_length);
        if (success < 0) {
            perror("ERROR: writing to the server failed in the client");
        }
    } 
    pthread_exit(0);
}



int spawn_threads(char *fifo_name) {

    pthread_t threads[2];

    pthread_cond_init(&signalAvailable, NULL);
    pthread_mutex_init(&signalLock, NULL);
    
    pthread_create(&threads[0], NULL, &incoming_messages, (void *) fifo_name);
    pthread_create(&threads[1], NULL, &incoming_stdin, NULL);
    
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    return 0;
}