
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "FileLogic.cpp"
//#include "BufferLogic.cpp"

FILE* filePtr = NULL;
const int ROWSIZE = 150;
const int BUFFERSIZE = 256;
const int COMMANDSIZE = 10;
char** buffer = NULL;
int bufferRowCounter = -1;  // default value is -1 empty buffer(no rows)

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
		Sleep(1000);

		if (filePtr != NULL)
			fclose(filePtr);
		exit(0);
	}
	return TRUE;
}

void AllocFailureProgTermination() 
{
	perror("Error allocating memory");
	FreeBuffer(buffer, BUFFERSIZE, ROWSIZE, &bufferRowCounter);
	CloseFile(filePtr);
	Sleep(1000);
	exit(EXIT_FAILURE);
}

void ExecuteCommand(enum Mode command) 
{
	errno_t err;  // to track the execution of fopen_s()
	char* input = (char*)malloc(sizeof(char) * ROWSIZE);
	switch (command)
	{
	case APPEND:
		if (input == NULL)
			AllocFailureProgTermination();
		
		fgets(input, ROWSIZE, stdin);
		strcat_s(buffer[bufferRowCounter], ROWSIZE, input);
		break;
	
	case NEWLINE:
		if (bufferRowCounter < BUFFERSIZE - 1)
		{
			for (int index = 0; index < ROWSIZE - 1; index++) 
			{
				if (buffer[bufferRowCounter][index] == '\0') 
				{
					buffer[bufferRowCounter][index] = '\n';
					buffer[bufferRowCounter][index + 1] = '\0';
					AddRow(&buffer, BUFFERSIZE, &bufferRowCounter, ROWSIZE);
					break;
				}
			}
		}
		else
			printf("Unable to start a new line(buffer is full)");
		break;
	
	case SAVETOFILE:  // ADD in case if user cancels the action
		printf("\nEnter the filename: ");
		fgets(input, ROWSIZE, stdin);
		for (int index = 0; index < ROWSIZE; index++) {
			if (input[index] == '\n') {
				input[index] = '\0';
				break;
			}
		}
		err = fopen_s(&filePtr, input, "a+");
		if (err != 0 || filePtr == NULL)  // returns 0 if successful
		{
			printf("\nCould not open the file");
			break;
		}
		WriteToFile(filePtr, buffer, BUFFERSIZE, ROWSIZE, bufferRowCounter);
		CloseFile(filePtr); 
		break;

	case LOADFROMFILE:
		
		printf("\nCurrent text will be deleted, 1 - continue, 0 - cancel:\n");
		fgets(input, ROWSIZE, stdin);
		if (input[0] == '0')
			break;

		printf("\nEnter the filename: ");
		fgets(input, ROWSIZE, stdin);
		for (unsigned int index = 0; index < strlen(input); index++) {
			if (input[index] == '\n') {
				input[index] = '\0';
				break;
			}
		}
		err = fopen_s(&filePtr, input, "r");
		if (err != 0 || filePtr == NULL)  // returns 0 if successful
		{
			printf("\nCould not open the file");
			break;
		}
		LoadFromFile(filePtr, buffer, &bufferRowCounter, BUFFERSIZE, ROWSIZE);
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
	free(input);
}
 


void PrintMainMenu()
{
	int curLength = GetRowRemainLength(buffer, bufferRowCounter, ROWSIZE);
	printf("Row size is %d symbols\nEnter your command:\n1 - append, 2 - newline, 3 - save to a file, 4 - load from file, 5 - print current, 6 - insert\n", curLength);
}
enum Mode GetUserCommand() 
{
	printf("Enter number: ");
	enum Mode command;
	
	char* input = (char*)malloc(COMMANDSIZE*sizeof(char));
	fgets(input, COMMANDSIZE, stdin);
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
	default:  // here we need to check for exit
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
	for (int i = 0; i < 4; i++) {
		PrintMainMenu();
		enum Mode command = GetUserCommand();
		ExecuteCommand(command);
		printf("%s\n", buffer[bufferRowCounter - 1]);

	}
	

//	printf("%s", buffer[bufferRowCounter]);

	fopen_s(&filePtr, "hello1.txt", "r");
	
	LoadFromFile(filePtr, buffer, &bufferRowCounter, BUFFERSIZE,ROWSIZE);
	printf("%s", buffer[0]);
	FreeBuffer(buffer, BUFFERSIZE, ROWSIZE, &bufferRowCounter);
	CloseFile(filePtr);


	Sleep(100);
	
	//free(row);

	return 0; 

}




