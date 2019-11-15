#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/wait.h>


/**
*	This program is the client end of a remote shell through socket.
*	The client program must run after the server program.
*	Usage: <Program> <IP address> <Port>
*/

// maximum lenth of arguments which should be big enough to make sure the socket never fails of big amount of data
#define MAX_LENGTH 1000000
// maximum lenth of each command and each argument
#define MAX_WORD_LENGTH 40

char* myRead(char *s, int fd);


int main(int argc, char* argv[]) {
	char buffer[MAX_LENGTH];
	int server, portNumber, pid, n, status;
	socklen_t len;
	struct sockaddr_in servAdd;

	if (argc != 3) {
		fprintf(stderr, "Call model:%s <IP> <Port>\n", argv[0]);
		exit(0);
	}

	if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Cannot create a socket\n");
		exit(1);
	}

	//config the server socket
	servAdd.sin_family = AF_INET;
	sscanf(argv[2], "%d", &portNumber);
	servAdd.sin_port = htons((uint16_t)portNumber);
	if (inet_pton(AF_INET, argv[1], &servAdd.sin_addr) < 0) {
		fprintf(stderr, " inet_pton() has failed\n");
		exit(2);
	}
	//connect to the server
	if (connect(server, (struct sockaddr*) &servAdd, sizeof(servAdd)) < 0) {
		fprintf(stderr, "Connection refused...\n");
		fprintf(stderr, "Check if server is running on the right port and try again...\n");
		exit(3);
	}

	while (1) {
			//prompt
			write(STDOUT_FILENO, "-INPUT COMMAND(s)->", 20);
			n = read(STDIN_FILENO, buffer, MAX_LENGTH);
			buffer[n] = '\0';
			//send commands to the server using space" " to seperate arguments
			write(server, buffer, strlen(buffer) + 1);
			//user types 'quit' to close the connection
			/* if (!strcasecmp(buffer, "quit\n")) {
				close(server);
				//kill(getppid(), SIGTERM);
				//exit(0);
			} */
			//printf("waiting for server...\n");
			do {
			if (n = read(server, buffer, MAX_LENGTH)) {
				buffer[n] = '\0';
				write(STDOUT_FILENO, buffer, n + 1);
				//printf("server returned %s\n", buffer);
			}
			} while (strncmp(buffer, "DONE", 4));
		
	}
}


