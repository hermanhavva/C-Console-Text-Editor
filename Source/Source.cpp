
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
HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);

enum Mode
{
	USEREXIT = 0,
	APPEND = 1,
	NEWLINE = 2,
	SAVETOFILE = 3,
	LOADFROMFILE = 4,
	PRINTCURRENT = 5,
	INSERT = 6,
	CLS = 7,
	UNDEFINED = 8
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
	if (input == NULL)
		AllocFailureProgTermination();

	switch (command)
	{
	case USEREXIT:
		printf(">>exiting\n");
		CloseFile(filePtr);
		FreeBuffer(buffer,BUFFERSIZE, ROWSIZE, &bufferRowCounter);
		free(input);
		Sleep(100);
		exit(0);
		break;

	case APPEND:
		fgets(input, ROWSIZE, stdin);
		strcat_s(buffer[bufferRowCounter], ROWSIZE, input);
		printf(">>success\n");
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
					printf(">>success\n");
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
		printf(">>success\n");
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
		printf(">>success");
		break;

	case PRINTCURRENT:
		printf("Current text: \n________________________________\n");
		for (int row = 0; row <= bufferRowCounter; row++) {
			printf("%s\n", buffer[row]);
		}
		printf("________________________________\n");
		break;

	case INSERT:
		break;
	case UNDEFINED:
		break;
	case CLS:
		system("cls");
		break;

	default:
		break;
	}
	free(input);
}
 


void PrintMainMenu()
{
	int curLength = GetRowRemainLength(buffer, bufferRowCounter, ROWSIZE);
	printf("Row space left is %d symbols\nEnter a digit (your command):\n0 - exit, 1 - append, 2 - newline, 3 - save to a file, 4 - load from file, 5 - print current,\n6 - insert, 7 - clean screen\n", curLength);
}
enum Mode GetUserCommand() 
{
	printf("Enter number: ");
	enum Mode command;
	
	char* input = (char*)malloc(COMMANDSIZE*sizeof(char));
	fgets(input, COMMANDSIZE, stdin);
	switch (input[0])
	{
	case '0':
		command = USEREXIT;
		break;
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
	case '7':
		command = CLS;
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
	SetConsoleTextAttribute(hout, BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN);
	system("cls");

	InitializeBuffer(&buffer, BUFFERSIZE);
	AddRow(&buffer, BUFFERSIZE, &bufferRowCounter, ROWSIZE);
	for (int i = 0; i < 4; i++) {
		PrintMainMenu();
		enum Mode command = GetUserCommand();
		ExecuteCommand(command);

	}
	

//	printf("%s", buffer[bufferRowCounter]);

	//fopen_s(&filePtr, "hello1.txt", "r");
	
	//LoadFromFile(filePtr, buffer, &bufferRowCounter, BUFFERSIZE,ROWSIZE);
//	printf("%s", buffer[0]);
	FreeBuffer(buffer, BUFFERSIZE, ROWSIZE, &bufferRowCounter);
	CloseFile(filePtr);


	Sleep(100);
	
	//free(row);

	return 0; 

}




