#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define SIZE 10000               

int charToInt(char character)
{
	int i;
	char *characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

	//Switches chars for ints
	for (i = 0; i < 27; i++)
	{
		if (characters[i] == character)
		{
			return i;
		}
	}
	return -1;
}

int main(int argc, char* argv[])
{
	int value = 1,
		listenSocketFD,
		portNumber,
		establishedConnectionFD;
	char buffer[SIZE];
	struct sockaddr_in clientAddress;
	struct sockaddr_in serverAddress;
	socklen_t sizeOfClientInfo;
	pid_t pid;

	if (argc != 2)
	{
		fprintf(stderr, "Error incorrect number of arguments.\n");
		exit(1);
	}

	//Sets up and validates socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocketFD < 0)
	{
		fprintf(stderr, "Error opening socket.\n");
		exit(1);
	}

	//From program page
	setsockopt(listenSocketFD, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(int));

	//Sets up address structure based off of lecture 4.3
	memset((char *)&serverAddress, 0, sizeof(serverAddress));
	portNumber = atoi(argv[1]);
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(portNumber);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	//Enables socket based off of lecture 4.3
	if (bind(listenSocketFD, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
	{
		fprintf(stderr, "Error on binding.\n");
		exit(1);
	}

	listen(listenSocketFD, 5);

	//Based off of lecture 4.3
	while (1)
	{
		//Accepts a connection and validates
		sizeOfClientInfo = sizeof(clientAddress);
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *) &clientAddress, &sizeOfClientInfo);
		if (establishedConnectionFD < 0)
		{
			fprintf(stderr, "Error on accept.\n");
			exit(1);
		}

		//Fork the process and make sure there wasn't an error
		pid = fork();
		if (pid < 0)
		{
			fprintf(stderr, "Error on fork.\n");
			exit(1);
		}

		//Handles processes
		if (pid == 0)
		{
			memset(buffer, 0, sizeof(buffer));
			int next = 0,
				bytesRead = 0,
				numOfBytes = sizeof(buffer),
				textNumber,
				keyNumber,
				decryptNumber,
				length,
				i;
			char *key,
				message[SIZE],
				*characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

			//Makes sure process is properly connected to otp_dec
			read(establishedConnectionFD, buffer, sizeof(buffer) - 1);
			if (strcmp(buffer, "dec") != 0)
			{
				char output[] = "Non-valid connection";
				write(establishedConnectionFD, output, sizeof(output));
				exit(2);
			}
			//Sends message for otp_dec to authenticate connection
			else
			{
				char output[] = "dec_d";
				write(establishedConnectionFD, output, sizeof(output));
			}

			//Clear buffer and create a pointer to track 
			memset(buffer, 0, sizeof(buffer));
			char* buffHelper = buffer;

			while (1)
			{
				//Reads number of bytes
				bytesRead = read(establishedConnectionFD, buffHelper, numOfBytes);
				if (numOfBytes == 0)
				{
					break;
				}

				//Grabs key character until second newline
				for (i = 0; i < bytesRead; i++)
				{
					if (buffer[i] == '\n')
					{
						next++;
						if (next == 1)
						{
							key = buffer + i + 1;
						}
					}
				}
				if (next == 2)
				{
					break;
				}

				//Tracks position
				numOfBytes = numOfBytes - bytesRead;
				buffHelper = buffHelper + bytesRead;
			}

			//Sets message and gets ready to write
			memset(message, 0, sizeof(message));
			strncpy(message, buffer, key - buffer);

			length = (strlen(message) - 1);

			//Decrypts using key
			for (i = 0; i < length; i++)
			{
				textNumber = charToInt(message[i]);
				keyNumber = charToInt(key[i]);
				decryptNumber = (textNumber - keyNumber) % 27;

				if (decryptNumber < 0)
				{
					decryptNumber += 27;
				}

				message[i] = characters[decryptNumber];

			}
			message[i] = '\0';
			write(establishedConnectionFD, message, sizeof(message));
		}
		close(establishedConnectionFD);
	}

	close(listenSocketFD);

	return 0;
}