#include <stdio.h>
#include <stdlib.h>
#include <windows.h>



void InitializeBuffer(char*** buffer, int const BUFFERSIZE)  // got to check for null after all
{
	*buffer = (char**)calloc(sizeof(char*), BUFFERSIZE);

}
void AddRow(char*** buffer, const int BUFFERSIZE, int* bufferRowCounterPtr, const int ROWSIZE) {
    if (*bufferRowCounterPtr >= BUFFERSIZE) {
        printf("Buffer is full. Cannot add more rows.\n");
        return;
    }

    (*bufferRowCounterPtr)++;

    (*buffer)[*bufferRowCounterPtr] = (char*)malloc(ROWSIZE * sizeof(char));
    if ((*buffer)[*bufferRowCounterPtr] == NULL) {
        perror("Failed to allocate row\n");
        (*bufferRowCounterPtr)--;
        return;
    }

    (*buffer)[*bufferRowCounterPtr][0] = '\0';
}
void FreeBuffer(char** buffer, const int BUFFERSIZE, const int ROWSIZE) 
{
	for (int row = 0; row < BUFFERSIZE; row++) 
	{
		free(buffer[row]);
	}
	free(buffer);
}

