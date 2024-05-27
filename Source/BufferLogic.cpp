#include <stdio.h>
#include <stdlib.h>
#include <windows.h>



int InitializeBuffer(char*** buffer, int const BUFFERSIZE)  // got to check for null after all
{
	*buffer = (char**)calloc(BUFFERSIZE, sizeof(char*));
    if (buffer == NULL)
        return -1;
    return 0;

}
int AddRow(char*** buffer, const int BUFFERSIZE, int* bufferRowCounterPtr, const int ROWSIZE) {
    if (*bufferRowCounterPtr >= BUFFERSIZE - 1) 
    {
        printf("Buffer is full. Cannot add more rows.\n");
        return -1;
    }

    (*bufferRowCounterPtr)++;

    (*buffer)[*bufferRowCounterPtr] = (char*)malloc(ROWSIZE * sizeof(char));
    if ((*buffer)[*bufferRowCounterPtr] == NULL) {
        perror("Failed to allocate row\n");
        (*bufferRowCounterPtr)--;
        return -1;
    }

    (*buffer)[*bufferRowCounterPtr][0] = '\0';
    return 0;
}
void FreeBuffer(char** buffer, const int BUFFERSIZE, const int ROWSIZE, int* bufferRowCounterPtr) 
{
	for (int row = 0; row < BUFFERSIZE; row++) 
	{
		free(buffer[row]);
	}
	free(buffer);
    *bufferRowCounterPtr = -1;  // sets to initial state
}
int GetRowRemainLength(char** buffer, const int CURROW, const int ROWSIZE) 
{
    int curLength = ROWSIZE;  // by default
    if (buffer[CURROW] != NULL)
        curLength = ROWSIZE - strlen(buffer[CURROW]) - 1;  // -1 to keep '\0'
    if (curLength < 0)
        return 0;
    return curLength;
}
