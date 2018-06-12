// Comp 3430 Assignment 2 Speed Typer

// server.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define NUMPLAYERS 2
#include "words.h"

// forward reference
int signal_client(int player);

const char SERVER_FIFO[] = "./_pipe";
char string_buffer[10];

char *client_pids[NUMPLAYERS];
int client_scores[NUMPLAYERS] = {0, 0};

int main()
{
    printf("Server is listening for input \n");

    int status = mkfifo(SERVER_FIFO, 0666);
    if (status < 0) {
        printf("FIFO creation failed! It is likely that this FIFO already exists! \n");
    }

    // Open FIFO for write
    int response = open(SERVER_FIFO, O_RDONLY);
    if (response < 0) {
        printf("Response FIFO is unavailable for read access");
        return EXIT_FAILURE;
    
    } else {
        printf("Response FIFO Opened \n");
    }

    // Check registration
    char length;

    int count = 0;
    while (count < NUMPLAYERS)
    {
    	// read the length
        int reads = read(response, &length, 1);
        read(response, string_buffer, length);

        string_buffer[(int)length] = '\0';
        
        if (string_buffer[0] != '\0') {
            client_pids[count] = (char *)malloc(sizeof(string_buffer));
            client_pids[count] = string_buffer;
            printf("Registered Client: %s\n", string_buffer);
            
            int errnum = signal_client(count);
            if (errnum < 0) {
                 printf("Failed to notify client properly!");
                 return EXIT_FAILURE;
            }
            count++;
        }
    }
    printf("Clients Registered Succesfully!\n");

    // Write to client FIFOs
    // int wordsSpelledCorrectly = 0;

    //int clientFifos[numPlayers];
    //for (int i = 0; i < numPlayers; i++) {
    //   clientFifos[i] = open("fifo_%d")
    // }

    // Check if word was spelled correctly using the client_responses FIFO

    // Send all clients win or lose through the FIFOS created by clients
    
    return 0;
}

//************************************************************
// int signal_client(int player)
// {
//     // Signal registration
//     //char *SERVER_FIFO = "/tmp/_pipe";
//     char *fifo_name = (char *)malloc(sizeof(SERVER_FIFO) + sizeof(client_pclient_client_pids[player]) + 1);
//     strcat(fifo_name, SERVER_FIFO);
//     strcat(fifo_name, client_pclient_client_pids[player]);
    
//     int signal_result = open(fifo_name, O_WRONLY);
    
//     if (signal_result < 0) {
//         printf("Could not find client FIFO!");
//         return EXIT_FAILURE;
    
//     } else {
//         char *message = "You have been accepted into the game! \n";
//         char L = (char)strlen(message);
//         write(signal_result, &L, 1);                          //Send string length
//         write(signal_result, message, (char)strlen(message)); //Send string characters
//         close(signal_result);
//     }
//     return EXIT_SUCCESS;
// }



int signal_client(int player)
{
    // Signal registration
    //char *fileNameFirst = "./_pipe";
    char *fifo_name = (char *)malloc(sizeof(SERVER_FIFO) + sizeof(client_pids[player]) + 1);
    strcat(fifo_name, SERVER_FIFO);
    strcat(fifo_name, client_pids[player]);

    //printf("Clients pids %s\n", client_pids[0]);
    
    printf("Server opening client's pipe %s\n", fifo_name);

    int signalEntry = open(fifo_name, O_WRONLY);
    if (signalEntry == -1) {
        printf("Could not find client FIFO!");
        return EXIT_FAILURE;
    
    } else {
        char *message = "You have been accepted into the game! \n";
        char L = (char)strlen(message);
        write(signalEntry, &L, 1);                          //Send string length
        write(signalEntry, message, (char)strlen(message)); //Send string characters
        close(signalEntry);
    }
    return EXIT_SUCCESS;
}