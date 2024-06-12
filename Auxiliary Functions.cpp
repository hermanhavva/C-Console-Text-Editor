#include "Buffer class.h"
#include "windows.h"
#include "Auxiliary functions.h"

void AllocFailureProgTermination(Buffer* buffer, FILE* filePtr)
{
    perror("Error allocating memory");
    CloseFile(filePtr);
    delete buffer;

    Sleep(1000);
    exit(EXIT_FAILURE);
}

void CloseFile(FILE* filePtr)
{
    if (filePtr != nullptr)
        fclose(filePtr);

    filePtr = nullptr;
}

int RemoveEndNewLine(char* string)
{
	int lenght = strlen(string);
	if (lenght > 0)
	{
		if (string[lenght - 1] == '\n')
		{
			string[lenght - 1] = '\0';
			return 0;
		}
	}
	return -1;
}

bool IsInputSizeValid(char* input, int inputSize)
{
	if (inputSize > 0 && inputSize == strlen(input) + 1)
	{
		if (input[strlen(input) - 1] != '\n')
			while ((getchar()) != '\n');

		return false;
	}
	return true;
}
