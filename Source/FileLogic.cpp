#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "BufferLogic.cpp"

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
int GetTxtSize(FILE* filePtr)
{
	if (filePtr == NULL)
		return -1;
	fseek(filePtr, 0, SEEK_END); // Move the file pointer to the end of the file
	int fileSize = ftell(filePtr); // Get the current file pointer position
	rewind(filePtr); // Reset the file pointer to the beginning of the file

	return fileSize;
}

long LoadFromFile(FILE* filePtr, 
				  char** destBuffer, 
				  int *bufferRowCounterPtr, 
				  const int BUFFERSIZE, 
				  const int ROWSIZE)
{
	if (filePtr == NULL)
		return -1;

	if (destBuffer != NULL)  // flushing the buffer if it has been initialized before
		FreeBuffer(destBuffer, BUFFERSIZE, ROWSIZE, bufferRowCounterPtr);

	InitializeBuffer(&destBuffer, BUFFERSIZE);
	AddRow(&destBuffer, BUFFERSIZE, bufferRowCounterPtr, ROWSIZE);

	long charCounter = 0;
	const int FILESIZE = GetTxtSize(filePtr);
	char* textFromTxt = (char*)calloc(FILESIZE + 2, sizeof(char));  // add 2 to make sure it is in the bounds
	fread(textFromTxt, sizeof(char), FILESIZE, filePtr);
	textFromTxt[FILESIZE] = '\0';

	// now write to the buffer
	for (int index = 0; index <= FILESIZE; index++)   
	{
		char curSymbol = textFromTxt[index];

		// some garbage symbols may appear in textFromTxt so to omit them there is a condition
		if ((int)curSymbol <= 255 && (int)curSymbol > 0) {
			if (curSymbol == '\n') {
				AddRow(&destBuffer, BUFFERSIZE, bufferRowCounterPtr, ROWSIZE);
			}
			if (GetRowRemainLength(destBuffer, *bufferRowCounterPtr, ROWSIZE) > 2)
				strncat_s(destBuffer[*bufferRowCounterPtr], ROWSIZE, &curSymbol, 1);
			charCounter++;
		}
		else if (curSymbol == '\0') 
			break;
	}
	free(textFromTxt);
	
	return charCounter;
}

