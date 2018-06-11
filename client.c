#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include<stdbool.h>

int main()
{
    const char SERVER_FIFO[] = "/tmp/_pipe";
    char message[10];

    // Create Client FIFO
    printf("Creating new FIFO!\n");
    //char *fileNameFirst = "/tmp/client_";
    //char *fifo_name = (char *) malloc (sizeof(fileNameFirst) + sizeof(message) + 1);
    //strcat(fifo_name, fileNameFirst);
    //strcat(fifo_name, message);

    // int send = mkfifo(fifo_name, 0666);
    // if (send == -1)
    // {
    //     printf("Client FIFO failed to create. It likely already exists and you can ignore this warning!");
    // }
    // else
    // {
    //     printf("Created Client FIFO!\n");
    // }

   
    sprintf(message, "%d", getpid());
    printf("Client %s\n", message);

    //Open fifo for write
    int fd = open(SERVER_FIFO, O_WRONLY);
    if (fd == -1)
    {
        perror("Cannot open fifo");
        return EXIT_FAILURE;
    }

    char L = (char)strlen(message);
    write(fd, &L, 1);               //Send string length
    write(fd, message, 10); //Send string characters
    //close(fd);

    // Get whether accepted into game or not
    // char stringBuffer[100];
    // int readFile = open(fifo_name, O_RDONLY);
    // if (readFile == -1)
    // {
    //     printf("Client FIFO could not be opened properly!");
    //     return EXIT_FAILURE;
    // }

    // bool readyToPlay = false;
    // char length;
    // while(readyToPlay == false) {
    //     int reads = read(readFile, &length, 1);
    //     if (reads == 0) {
    //         break;
    //     }
    //     read(readFile, stringBuffer, length);
    //     //stringBuffer[(int)length] = '\0';
    //     printf("%s\n", stringBuffer);
    //     readyToPlay = true;
    // }
    // printf("Congratulations! We are now ready to play \n");
    
    // //Tidy
    // close(readFile);
    return EXIT_SUCCESS;
}