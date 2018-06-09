#include <sys/types.h> //mkfifo
#include <sys/stat.h>  //mkfifo
#include <stdio.h>     // printf
#include <fcntl.h>     // open
#include <unistd.h>    // unlink
#include <stdlib.h>    // exit
#include <stdbool.h>   // bool

int main() {

	const char SERVER_FIFO[] = "/tmp/server_pipe";
	char buf[2];
	bool done = false;

	// create the named pipe
	int result = mkfifo(SERVER_FIFO, 0600);
	if (result) {
		perror("Unable to create the named pipe");
		exit(EXIT_FAILURE);
	}

	// connect to the named pipe
	printf("%s\n", "opening file");
	int fd = open(SERVER_FIFO, O_RDONLY);
	if (fd) {
		perror("Unable to create named pipe");
		exit(EXIT_FAILURE);
	}

	printf("%s\n", "done opening the file");

	while (!done) {
		result = read(fd, &buf, 1);

		if (result < 0) {
			perror("Error: error reading from pipe");
			exit(EXIT_FAILURE);

		} else if (result == 0) {
			printf("%s\n", "End of file");
			
		}

	}

}