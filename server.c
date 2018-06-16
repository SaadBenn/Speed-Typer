// Comp 3430 Assignment 2 Speed Typer
// Saad Mushtaq
// COMP 3430
// Assignmnet 2
// Please God, make it work while the marker is testing it. 
// server.c

#include <stdio.h>
#include <stdlib.h> // rand()
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h> // signal
#include <stdbool.h> // bool

#define NUMPLAYERS 2
#include "words.h"

// forward references
int signal_client(int player);
int generate_random_word();
int open_client_pipe(int player);
int read_client_response();
void game_starter();
void handler(int signal);
//void killer(int signal);

// store the name of the pipes
char *client_pipes[NUMPLAYERS];

int counter = 0;
const char SERVER_FIFO[] = "./_pipe";
char string_buffer[10];
int clients_left = 0;
char *random_word;
char word_from_client[10];
char word_from_client2[10];
int server_file_des;
bool winner_decided = false;
char *client_pids[NUMPLAYERS];
char client_word[15];


int pid_index;
int client_scores[NUMPLAYERS] = {0, 0};
char *track_client_word[2];
char *fifo_name;

int main() {

	// signal handlers
	signal(SIGINT, handler);
	signal(SIGTERM, handler);

    printf("Server is listening for input \n");

    int status = mkfifo(SERVER_FIFO, 0666);
    if (status < 0) {
        printf("FIFO creation failed! It is likely that this FIFO already exists! \n");
        return EXIT_FAILURE;
    }

    // Open FIFO for write
    server_file_des = open(SERVER_FIFO, O_RDONLY);
    if (server_file_des < 0) {
        printf("Response FIFO is unavailable for read access");
        return EXIT_FAILURE;
    
    } else {
        printf("Response FIFO Opened \n");
    }

    // Check registration
    char length;

    int count = 0;
    while (count < NUMPLAYERS) {

    	//printf("%s\n", "Waiting for clients pids");
    	// read the length
        int stat_len = read(server_file_des, &length, 1);

        if (stat_len < 0) {
         	perror("Error reading the length\n");
     	 	return EXIT_FAILURE;
        }

        int stat_buf = read(server_file_des, &string_buffer, length);
        if (stat_buf < 0) {
        	perror("Error reading the string buffer\n");
        	return EXIT_FAILURE;
        }

        // null terminate the byte
        string_buffer[(int)length] = '\0';
        
        if (string_buffer[0] != '\0') {
            client_pids[count] = (char *)malloc(sizeof(string_buffer));
            client_pids[count] = strdup(string_buffer);
			//printf("Stored the client pid at %d %s\n", count, client_pids[count]);
            //printf("Registered Client: %s\n", string_buffer);
        }
        count++;
    }

    printf("Clients Registered Succesfully!\n\n\n");

    game_starter();
    printf("%s\n", "Game over!");
    unlink(SERVER_FIFO);
    return 0;
}


void game_starter() {
	int i;
	char length;
	int rand_num;
	for (i = 0; i < NUMPLAYERS; i++) {
		int client_fifo_status = open_client_pipe(i);
		rand_num = generate_random_word();
		// save the word for comparision with the client's response
	 	track_client_word[i] = strdup(words[rand_num]);
	 	//printf("The random word is: %s\n", track_client_word[i]);
	 	random_word = strdup(words[rand_num]);
	 	length = strlen(words[rand_num]);
	 	write(client_fifo_status, &length, 1);
	 	write(client_fifo_status, words[rand_num], length);
	}

	char client_word_length;
	while (!winner_decided) {
		//printf("%s\n", "Waiting for client to type the word!");
		int read_status = read(server_file_des, &client_word_length, 1);

		if (read_status <= 0) {
			perror("Error: Server is closed\n");
			winner_decided = true;
		} else {
			if (client_word_length == '\a') {
				clients_left++;
			}

			if (NUMPLAYERS - clients_left == 0) {
				winner_decided = true;
				printf("%s\n", "All clients left");
			
			} else {
				read(server_file_des, word_from_client, client_word_length);
				word_from_client[client_word_length] = '\0';
		//printf("Got the word: %s from the client in the server\n", word_from_client);
		
				char *user_word = strtok(word_from_client, " ");
				char *user_pid = strtok(NULL, " ");

				for (i = 0; i < NUMPLAYERS; i++) {

					if (strcmp(client_pids[i], user_pid) == 0) {
						pid_index = i;
				
						if (strcmp(track_client_word[i], user_word) == 0) {
							(client_scores[i])++;
						}

						if (client_scores[i] == 10) {
							winner_decided = true;
						} 
					}		
				}

				if (!winner_decided) {
					rand_num = generate_random_word();
			
					int _status_ = open_client_pipe(pid_index);
					if (_status_ < 0) {
						perror("Error: error in opening the client's pipe.\n");
					}

					track_client_word[pid_index] = strdup(words[rand_num]);
					char len_new = strlen(words[rand_num]);
					write(_status_, &len_new, 1);
					write(_status_, words[rand_num], len_new);
		
				} else {
					char len_result;
					for (i = 0; i < NUMPLAYERS; i++) {
						if (client_scores[i] == 10) {
							len_result = strlen("WINNER");
					
							int signalEntry = open_client_pipe(i);
							if (signalEntry < 0) {
								perror("Error opening the client's pipe.\n");
							}

							write(signalEntry, &len_result, 1);
							write(signalEntry, "WINNER", len_result);
						
						} else {
							len_result = strlen("LOSER");
							int signalEntry = open_client_pipe(i);
							if (signalEntry < 0) {
								perror("Error opening the client's pipe.\n");
							}

							write(signalEntry, &len_result, 1);
							write(signalEntry, "LOSER", len_result);
						}	
					}
				}
			}
		}
	}
}


int generate_random_word() {
	int len = sizeof(words)/ sizeof(char*);

	int rand_num = rand() % len;

	return rand_num;
}


int open_client_pipe(int player) {

	fifo_name = (char *)malloc(sizeof(SERVER_FIFO) + sizeof(client_pids[player]) + 1);
    strcat(fifo_name, SERVER_FIFO);
    strcat(fifo_name, client_pids[player]);

    //printf("Server opening client's pipe %s\n", fifo_name);

    int signalEntry = open(fifo_name, O_WRONLY);
    return signalEntry;
}


void handler(int signal) {
	printf("%s\n", "Server cleaning up the fifo because of an interruption.");
	unlink(SERVER_FIFO);
	exit(0);
}