#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include<stdbool.h>

const char SERVER_FIFO[] = "./_pipe";

int main() {
    char message[10];

    // Create Client FIFO
    printf("Creating client's FIFO!\n");

    sprintf(message, "%d", getpid());
    char *fifo_name = (char *) malloc (sizeof(SERVER_FIFO) + sizeof(message) + 1);
    strcat(fifo_name, SERVER_FIFO);
    strcat(fifo_name, message);

    printf("Client creating a named pipe %s\n", fifo_name);

    int send = mkfifo(fifo_name, 0666);
    if (send < 0) {
        printf("Client FIFO failed to create. It likely already exists and you can ignore this warning!");
    
    } else {
        printf("Created Client FIFO!\n");
    }

    printf("Client %s\n", message);
    
    //Open server fifo for registering the client
    int fd = open(SERVER_FIFO, O_WRONLY);
    if (fd < 0) {
        perror("Cannot open fifo");
        return EXIT_FAILURE;
    }

    char message_length = (char)strlen(message);
    // Send string's length to the server
    write(fd, &message_length, 1);
    // Send string characters               
    write(fd, message, 10); 
    //close(fd);


    // Get whether accepted into game or not
    char string_buffer[100];
    int read_server_message = open(fifo_name, O_RDONLY);
    if (read_server_message < 0) {
        printf("Client FIFO could not be opened properly!");
        return EXIT_FAILURE;
    }

    bool player_ready = false;
    char server_res_length;
    
    while(!player_ready) {
        // get the message_length o fthe message
        int reads_ = read(read_server_message, &server_res_length, 1);
        if (reads_ == 0) {
            break;
        }
        
        // read the actual message
        read(read_server_message, string_buffer, server_res_length);
        //string_buffer[(int)message_length] = '\0';
        printf("%s\n", string_buffer);
        player_ready = true;
    }
    printf("Congratulations! We are now ready to play!!\n");
    
    // //Tidy
    close(read_server_message);
    return EXIT_SUCCESS;
}