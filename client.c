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
    bool done = false;
    bool killserver = false;
    char curr;
    char* result;
    int errno;
    char buf[64];
 	int fifo = open(SERVER_FIFO, O_WRONLY);
	if (fifo < 0) {
		perror("Unable to open named pipe");
		exit(-1);
	}

    // listen for input
    while(!done)
    {
        result = fgets((char*)&buf, 64, stdin);
        if (result != NULL) {
            // send this character to the FIFO
            // sends with \n, replace with \0
            buf[strlen(buf)-1] = '\0';
            if (strcmp(buf, "exit") == 0)
            {
                done = true;
                killserver = true;
            }
            else
            {
                printf("sending %s\n", buf);
                errno = write(fifo, &buf, strlen(buf)+1);
                
                if (errno < 0) {
                    perror("ERROR: Error writing to pipe");
                }
            }
        }
        else
            done = true;
    }

    if (killserver)
        write(fifo, "\a", 1);

    // though it's tempting, don't close the fifo
    // leave it open for others to use!
    // Try uncommenting to see what happens.
    //close(fifo);
    return 0;
}