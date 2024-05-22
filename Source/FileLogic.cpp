#include <stdio.h>
#include <stdlib.h>
#include <windows.h>


void CloseFile(FILE* filePtr)
{
	if (filePtr != NULL)
		fclose(filePtr);
	filePtr = NULL;
}
void WriteToFile(FILE* filePtr, 
				 char** buffer, 
				 const int BUFFERSIZE, 
				 const int ROWSIZE, 
				 const int BUFFERROWCOUNTER)
{
	if (filePtr != NULL && buffer[BUFFERROWCOUNTER] != NULL && BUFFERROWCOUNTER >= 0) 
	{
		int textSize = sizeof(char)*256*256;
		char* textToWrite = (char*)malloc(textSize);
		textToWrite[0] = '\0';

		for (int row = 0; row <= BUFFERROWCOUNTER; row++) 
			strcat_s(textToWrite, textSize, buffer[row]);
		fprintf_s(filePtr,"%s", textToWrite);
		free(textToWrite);
	}
}
void LoadFromFile(FILE* filePtr, char** destBuffer)
{
	
	
	return;
}
int GetTxtSize(FILE* filePtr) 
{
	if (filePtr == NULL)
		return -1;
	fseek(filePtr, 0, SEEK_END); // Move the file pointer to the end of the file
	int fileSize = ftell(filePtr); // Get the current file pointer position, which is the size of the file
	rewind(filePtr); // Reset the file pointer to the beginning of the file

	return fileSize;
}