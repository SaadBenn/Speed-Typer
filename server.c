#include <sys/types.h> //mkfifo
#include <sys/stat.h>  //mkfifo
#include <stdio.h>     // printf
#include <fcntl.h>     // open
#include <unistd.h>    // unlink
#include <stdlib.h>    // exit
#include <stdbool.h>   // bool

#define CLIENTS 2

int main() {

	const char SERVER_FIFO[] = "/tmp/server_pipe";
	char buf[100];
	char *client_PIDs[CLIENTS];


	// create the named pipe
	int result = mkfifo(SERVER_FIFO, 0600);
	if (result) {
		perror("Unable to create the named pipe");
		exit(EXIT_FAILURE);
	}

	// connect to the named pipe
	//printf("%s\n", "opening file");
	int fd = open(SERVER_FIFO, O_RDONLY);
	
	if (fd == -1) {
		perror("Error: pipe can't be opened.");
		exit(EXIT_FAILURE);
	}

	printf("%s\n", "Opened the file");

	int client_count = 0;
	while (client_count < CLIENTS) {
		result = read(fd, buf, sizeof(buf));
		if (result < 0) {
			perror("Error: error reading from pipe");
			exit(EXIT_FAILURE);
		} else {
			if (buf[0] != '\0') {
				client_PIDs[client_count++] = (char*)malloc(sizeof(buf));
				printf("Registeration successful %s\n", buf);
			}
		}
		
	}
	printf("%s\n", "Registeration successful");
}