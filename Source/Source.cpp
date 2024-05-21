﻿
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "BufferLogic.cpp"

FILE* filePtr = NULL;
const int ROWSIZE = 1000000000;
const int BUFFERSIZE = 256;
const int COMMANDSIZE = 10;
char** buffer;
int bufferRowCounter = 0;

// треба зробити окрему іункцію котра буде виконуватися, якщо пам'ять не виділилася, 
// ЯКИЙ СЕНС ВІД ТОГО ШОБ З ПОЧАТКУ ПРОГРАМИ ВИДІЛЯТИ ПАМ'ЯТЬ ПІД МАСИВ, ВІД ЦЬОГО НЕМА ВИГРАШУ З ТАКИМ УСПІХОМ 
// МОЖНА БУЛО Б ЮЗАТИ І СТАТИЧНУ ПАМ'ЯТЬ 
enum Mode
{
	APPEND = 1,
	NEWLINE = 2,
	SAVETOFILE = 3,
	LOADFROMFILE = 4,
	PRINTCURRENT = 5,
	INSERT = 6,
	UNDEFINED = 7
};
BOOL WINAPI ConsoleHandler(DWORD signal) 
{
	if (signal == CTRL_C_EVENT || signal == CTRL_CLOSE_EVENT || signal == CTRL_BREAK_EVENT ||
		signal == CTRL_LOGOFF_EVENT || signal == CTRL_SHUTDOWN_EVENT) {
		printf("\nProgram interrupted. Cleaning up...\n");
		Sleep(3071);
		printf("File closed successfully.\n");
		
		// Perform other cleanup tasks here
		if (filePtr != NULL)
			fclose(filePtr);
		exit(0);
	}
	return TRUE;
}

void AllocFailureProgTermination() 
{
	perror("Error allocating memory");
	FreeBuffer(buffer, BUFFERSIZE, ROWSIZE);
	fclose(filePtr);
	Sleep(1000);
	exit(EXIT_FAILURE);
}
void ExecuteCommand(enum Mode command) 
{
	
	switch (command)
	{
	case APPEND:
	{
		char* row = (char*)malloc(ROWSIZE * sizeof(char));  // deallocation is not needed as buffer will be freed
		if (row == NULL) 
			AllocFailureProgTermination();
		
		fgets(row, ROWSIZE, stdin);
		
		buffer[bufferRowCounter] = row;
		break;
	}
	case NEWLINE:
		if (bufferRowCounter < BUFFERSIZE - 1)
		{
			for (int index = 0; index < ROWSIZE - 1; index++) 
			{
				if (buffer[bufferRowCounter][index] == '\0') 
				{
					buffer[bufferRowCounter][index] = '\n';
					buffer[bufferRowCounter][index + 1] = '\0';
					bufferRowCounter++;
					break;
				}
			}
		}
		else
			printf("Unable to start a new line(buffer is full)");
		break;
	case SAVETOFILE:
		break;
	case LOADFROMFILE:
		break;
	case PRINTCURRENT:
		break;
	case INSERT:
		break;
	case UNDEFINED:
		break;
	default:
		break;
	}
}



void PrintMainMenu()
{
	printf("1 - append, 2 - newline, 3 - save to a file, 4 - load from file, 5 - print current, 6 - insert\n");
}
enum Mode GetUserCommand() 
{
	printf("Enter number: ");
	enum Mode command;
	
	char* input = (char*)malloc(COMMANDSIZE*sizeof(char));
	scanf_s("%s", input, COMMANDSIZE);
	Sleep(12112);
	switch (input[0])
	{
	case '1':
		command = APPEND;
		break;
	case '2':
		command = NEWLINE;
		break;
	case '3':
		command = SAVETOFILE; 
		break;
	case '4':
		command = LOADFROMFILE;
		break;
	case '5':
		command = PRINTCURRENT;
		break;
	default:
		printf("The command is not implemmented");
		command = UNDEFINED;
		break;
	}
	free (input);
	return command;
}

int main()
{
	if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE)) {
		fprintf(stderr, "Failed to set control handler\n");
		return EXIT_FAILURE;
	}
	
	InitializeBuffer(&buffer, BUFFERSIZE);
	AddRow(&buffer, BUFFERSIZE, &bufferRowCounter, ROWSIZE);
	

	fgets(buffer[bufferRowCounter], ROWSIZE, stdin);
	printf("%s\n", buffer[bufferRowCounter]);
	
	Sleep(3000);
	FreeBuffer(buffer, BUFFERSIZE, ROWSIZE);

	//free(buffer);
	//free(row);
	//row = NULL;

	Sleep(2000);
	
	//free(row);

	return 0; 

}




