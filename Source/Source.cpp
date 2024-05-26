#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "FileLogic.cpp"

FILE* filePtr = NULL;
const int ROWSIZE = 30;
const int BUFFERSIZE = 256;
const int COMMANDSIZE = 10;
char** buffer = NULL;
int bufferRowCounter = -1;  // default value is -1 empty buffer (no rows)
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
int RemoveEndNewLine(char string[])  // if last ch is '\n', removes it
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


int HandleInsert()
{
	unsigned int row, column;  // might make unsigned
	char* input = (char*)malloc(sizeof(char) * (ROWSIZE - 1));
	printf("\nRow for insertion: ");
	scanf_s(" %u", &row);
	printf("\nColumn for insertion: ");
	scanf_s(" %u", &column);
	fgets(input, ROWSIZE, stdin);  // just to get '\n' out of stdin buffer

	printf("String to insert: ");
	fgets(input, ROWSIZE, stdin);
	RemoveEndNewLine(input);  // for omitting '\n'

	if (row > bufferRowCounter || column > ROWSIZE) {
		printf(">>Row/Column index too big (might use newline first)\n");
		return -1;
	}
	int rowTextLength = strlen(buffer[row]);  // on this index there is '\0'
	int insertTextLength = strlen(input);
	int curRowMaxSize = ROWSIZE;  // size is dynamic 

	if ((insertTextLength + rowTextLength + 2) - curRowMaxSize > 0 && (insertTextLength + rowTextLength + 2) - curRowMaxSize < 30)
	{   // the logic can handle +30 expansion, but not more
		buffer[row] = (char*)realloc(buffer[row], sizeof(char) * (curRowMaxSize + 30));

		if (buffer[row] == NULL)
			AllocFailureProgTermination();
		curRowMaxSize += 30;
	}
	else if ((insertTextLength + rowTextLength + 2) - curRowMaxSize >= 30)
	{
		printf(">>The row is full or message too big to insert\n");
		return -1;
	}

	//if ((insertLength + rowLength + 2) < ROWSIZE)  // + 2 is for '\n' and '\0'
	//{							 
	if ((rowTextLength + 1) < column)  // add spaces
	{							
		for (int colIndex = rowTextLength - 1; colIndex < column - 1; colIndex++)  // -1 for to handle '\n'
			buffer[row][colIndex] = ' ';

		buffer[row][column - 1] = '\0';
		strcat_s(buffer[row], curRowMaxSize - 1, input);

		int curLength = strlen(buffer[row]);
		buffer[row][curLength] = '\n';  // handle next row 
		buffer[row][curLength + 1] = '\0';
	}
	else if ((rowTextLength + 1) > column)
	{
		char* addBuffer = (char*)malloc(sizeof(char) * curRowMaxSize - 1);
		addBuffer[0] = '\0';
		char ch = '0';
		for (int colIndex = column; colIndex < rowTextLength; colIndex++)  // here is the problem
		{
			ch = buffer[row][colIndex];
			strncat_s(addBuffer, curRowMaxSize - 1, &ch, 1);  // one symbol at a time
				
		}
		buffer[row][column] = '\0';
		//printf("%s", addBuffer);  
		strcat_s(buffer[row], curRowMaxSize - 1, input);
		strcat_s(buffer[row], curRowMaxSize - 1, addBuffer);  // buffer too small if realloc
		free(addBuffer);
	}
	else  // only strcat
	{
		strcat_s(buffer[row], curRowMaxSize - 1, input);
	}

	//	}
	free(input);
	return 0;
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
		RemoveEndNewLine(input);  // removing '\n'

		if (GetRowRemainLength(buffer, bufferRowCounter, ROWSIZE) > strlen(input)) {
			strcat_s(buffer[bufferRowCounter], ROWSIZE - 1, input);  // ROWSIZE-1 for keeping place for '\n'
			printf(">>success\n");
		}
		else
			printf(">>Error, buffer too small, start a newline\n");
		
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
		RemoveEndNewLine(input);  // removing '\n'

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
		printf(">>success\n");
		break;

	case PRINTCURRENT:
		printf("Current text: \n________________________________\n");
		for (int row = 0; row <= bufferRowCounter; row++) {
			printf("%s", buffer[row]);
		}
		printf("\n________________________________\n");
		break;

	case INSERT:
		HandleInsert();
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
	int curLength = GetRowRemainLength(buffer, bufferRowCounter, ROWSIZE) - 1;  
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
	case '6':
		command = INSERT;
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
	for (int i = 0; i <= 5; i++) {
		PrintMainMenu();
		enum Mode command = GetUserCommand();
		ExecuteCommand(command);

	}
	
	FreeBuffer(buffer, BUFFERSIZE, ROWSIZE, &bufferRowCounter);
	CloseFile(filePtr);


	Sleep(100);
	
	//free(row);

	return 0; 

}




