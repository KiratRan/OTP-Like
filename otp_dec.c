#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h> 
#include <sys/socket.h>
#include <sys/types.h>

#define SIZE 10000

void sendFile(char *filename, int sock, int length)
{
	FILE *file = fopen(filename, "r");
	char buffer[SIZE];
	memset(buffer, 0, SIZE);
	int numOfBytes;

	//Reads and sends file
	while ((length = fread(buffer, sizeof(char), SIZE, file)) > 0)
	{
		if ((numOfBytes = send(sock, buffer, length, 0)) < 0)
		{
			break;
		}
		memset(buffer, 0, SIZE);
	}

	//EOF reached
	if (numOfBytes == SIZE)
	{
		send(sock, "0", 1, 0);
	}

	fclose(file);
	return;
}

int main(int argc, char* argv[])
{
	int sock,
		portNumber,
		value = 1;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[SIZE],
		authenticate[] = "dec";

	memset(buffer, 0, sizeof(buffer));

	if (argc != 4)
	{
		fprintf(stderr, "Error incorrect number of arguments.\n");
		exit(1);
	}

	//Gets port number and sets socket
	portNumber = atoi(argv[3]);
	sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock < 0)
	{
		fprintf(stderr, "Error opening socket.\n");
		exit(1);
	}

	//DNS lookup based off of lecture 4.2
	serverHostInfo = gethostbyname("localhost");
	if (serverHostInfo == NULL)
	{
		fprintf(stderr, "Error connecting to host.\n");
		exit(1);
	}

	//From program page 
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(int));

	//Sets up socket based off of lecture 4.2
	memset((char *)&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(portNumber);
	memcpy((char *)serverHostInfo->h_addr, (char *)&serverAddress.sin_addr.s_addr, serverHostInfo->h_length);

	//Makes sure socket is connected to address
	if (connect(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
	{
		fprintf(stderr, "Error connecting socket and address.\n");
		exit(1);
	}

	//Authentication to prevent erroneous connection
	write(sock, authenticate, sizeof(authenticate));
	read(sock, buffer, sizeof(buffer));
	if (strcmp(buffer, "dec_d") != 0)
	{
		fprintf(stderr, "Error otp_dec can't use otp_enc_d.\n");
		exit(2);
	}

	//Gets text, key, and the sizes of both
	int key = open(argv[2], O_RDONLY),
		keySize = lseek(key, 0, SEEK_END),
		text = open(argv[1], O_RDONLY),
		textSize = lseek(text, 0, SEEK_END);

	//Makes sure key has the proper size
	if (textSize > keySize)
	{
		fprintf(stderr, "Error key is too-short.\n");
		exit(1);
	}

	//Sends file
	memset(buffer, 0, sizeof(buffer));
	sendFile(argv[1], sock, textSize);
	sendFile(argv[2], sock, keySize);

	read(sock, buffer, sizeof(buffer) - 1);

	printf("%s\n", buffer);
	close(sock);

	return 0;
}