#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main(int argc, char *argv[])
{
	srand(time(NULL));

	int random,
		keyLength,
		i;

	//Sets the length of the key to be equal to the second argument
	keyLength = atoi(argv[1]);

	//Randomly adds letters, spaces, and writes to stdout
	for (i = 0; i < keyLength; i++)
	{
		random = "ABCDEFGHIJKLMNOPQRSTUVWXYZ "[rand() % 27];
		printf("%c", random);
	}

	//Prints out newline as last character per requirement
	printf("\n");

	return 0;
}