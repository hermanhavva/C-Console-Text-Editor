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
int GetTxtSize(FILE* filePtr)
{
	if (filePtr == NULL)
		return -1;
	fseek(filePtr, 0, SEEK_END); // Move the file pointer to the end of the file
	int fileSize = ftell(filePtr); // Get the current file pointer position
	rewind(filePtr); // Reset the file pointer to the beginning of the file

	return fileSize;
}

void LoadFromFile(FILE* filePtr, char** destBuffer, int *bufferRowCounter)
{
	if (filePtr == NULL)
		return;
	const int FILESIZE = GetTxtSize(filePtr);
	char* textFromTxt = (char*)calloc(FILESIZE + 2, sizeof(char));  // add 2 to make sure it is in the bounds
	fread(textFromTxt, sizeof(char), FILESIZE, filePtr);
	textFromTxt[FILESIZE] = '\0';
	if ()
	// now write to the buffer
	for (int index = 0; index <= FILESIZE; index++)   
	{
		// some garbage symbols may be written in textFromTxt so to omit them there is a condition
		if ((int)textFromTxt[index] <= 255 && (int)textFromTxt[index] > 0) {
			printf("%c", textFromTxt[index]);
		}
		else if (textFromTxt[index] == '\0') {
			break;
		}
	}

	free(textFromTxt);

}

