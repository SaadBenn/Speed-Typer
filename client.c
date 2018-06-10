#include <sys/types.h> //mkfifo
#include <sys/stat.h>  //mkfifo
#include <stdio.h>     // printf
#include <fcntl.h>     // open
#include <unistd.h>    // unlink
#include <stdlib.h>    // exit
#include <stdbool.h>   // bool
#include <string.h>    // strlen

int main(){

	const char SERVER_FIFO[] = "/tmp/server_pipe";
    char message[100];
    sprintf(message, "%d", getpid());
    int fifo;

    fifo = open(SERVER_FIFO, O_WRONLY);
    if (fifo < 0) {
        perror("Unable to open named pipe");
        exit(EXIT_FAILURE);
    }

    int messagelen = strlen(message) + 1;
    
    // write to the FIFO
    int errno = write(fifo, message, messagelen);
    if (errno < 0) {
        perror("Error writing");
    }
   
    // though it's tempting, don't close the fifo
    // leave it open for others to use!
    // Try uncommenting to see what happens.
    //close(fifo);
    return 0;
}