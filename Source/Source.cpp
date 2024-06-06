#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "FileLogic.cpp"
#include "Buffer Class.cpp"


FILE*  filePtr = NULL;
const  int ROWSIZE = 150;
const  int BUFFERSIZE = 256;
const  int COMMANDSIZE = 10;
char** buffer = NULL;
int	   bufferRowCounter = -1;  // default value is -1 empty buffer (no rows)
HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);

void HandleUserExit(char*);
int  HandleAppend(char*);
int  HandleNewLine();
int  HandleSaveToFile(char*);
int  HandleLoadFromFile(char*);
void HandlePrintCurrent();
int  HandleInsert();

enum Mode;
BOOL WINAPI ConsoleHandler(DWORD);
void AllocFailureProgTermination();
int  RemoveEndNewLine(char*);
void ExecuteCommand(enum Mode, Buffer buffer);
void PrintMainMenu();
enum Mode GetUserCommand();
/*
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

	while (TRUE)
	{
		PrintMainMenu();
		enum Mode command = GetUserCommand();
		ExecuteCommand(command);
	}
	
	FreeBuffer(buffer, BUFFERSIZE, ROWSIZE, &bufferRowCounter);
	CloseFile(filePtr);

	Sleep(100);

	return 0; 
}*/ 

int main() 
{
	Buffer* buffer = new Buffer();
	char* input = new char[100];
	input[0] = '\0';
	strcpy_s(input, 100, "hello");
	buffer->Append(input);
	buffer->AddRow();
	strcpy_s(input, 10, "2 row");
	char* str = new char [10];
	strcpy_s(str, 10, "ello");
	buffer->Append(input);
	buffer->SetCursorPosition(0, 1);
	buffer->AddRow();
	buffer->SetCursorPosition(1, 1); 
	buffer->PrintCurrent();  
	buffer->SearchSubstrPos(str);
	buffer->AddRow();
	fopen_s(&filePtr,"hello.txt", "a+");

	buffer->LoadFromFile(filePtr);
	buffer->Append(input);
	buffer->SaveToFile(filePtr);
	fclose(filePtr);
	buffer->PrintCurrent();  

	delete[] str;
	delete[] input;
	delete buffer;
}

void HandleUserExit(char* input, Buffer* buffer)
{
	printf(">>exiting\n");
	CloseFile(filePtr);
	delete buffer;
	delete[] input;
	Sleep(100);
	exit(0);
}

int HandleAppend(char* input, Buffer buffer)
{
	fgets(input, ROWSIZE, stdin);
	RemoveEndNewLine(input);  // removing '\n'

	if (buffer.Append(input) == 0) 
	{
		printf(">>success\n");
		return 0;
	}
	else
	{
		printf(">>failure\n");
		return -1;
	}
}

int HandleNewLine(Buffer buffer)
{
	if (buffer.AddRow() == 0)
	{
		printf(">>success\n");
		return 0;
	}
	printf(">>failure\n");
	return -1;
}

int HandleSaveToFile(char* input, Buffer buffer)
{
	errno_t err;  // to track the execution of fopen_s()
	printf("\nEnter the filename: ");
	fgets(input, ROWSIZE, stdin);
	RemoveEndNewLine(input);  // removing '\n'

	err = fopen_s(&filePtr, input, "a+");
	if (err != 0 || filePtr == nullptr)  // returns 0 if successful
	{
		printf("\nCould not open the file >>failure");
		return -1;
	}
	if (buffer.SaveToFile(filePtr) == -1)  // BETTER PUT A BOOL flag and make methods bool functions
	{
		printf(">>failure\n");
		CloseFile(filePtr);
		filePtr = nullptr;
		return -1;
	}
	
	CloseFile(filePtr);
	filePtr = nullptr;
	printf(">>success\n");
	return 0;

}

int HandleLoadFromFile(char* input, Buffer buffer)
{
	errno_t err;  // to track the execution of fopen_s() 
	printf("\nCurrent text will be deleted, 1 - continue, 0 - cancel:\n");

	fgets(input, ROWSIZE, stdin);
	if (input[0] == '0')
		return -1;

	printf("\nEnter the filename: ");
	fgets(input, ROWSIZE, stdin);   // remove '\n'
	RemoveEndNewLine(input);

	err = fopen_s(&filePtr, input, "r");
	if (err != 0 || filePtr == NULL)  // returns 0 if successful
	{
		printf("\nCould not open the file\n");
		return -1;
	}
	switch (buffer.LoadFromFile(filePtr))
	{
	case 0:
		printf(">>success\n");
		CloseFile(filePtr);
		filePtr = nullptr;
		return 0;
	case -1:
		printf(">>failure\n");
		CloseFile(filePtr);
		filePtr = nullptr;
		return -1;
	default:
		printf(">>failure\n");
		CloseFile(filePtr);
		filePtr = nullptr;
		return -1;
	}
}



int HandleInsert()
{
	int row, column;  
	const int offset = 30;
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

	if ((insertTextLength + rowTextLength + 2) - curRowMaxSize > 0 && (insertTextLength + rowTextLength + 2) - curRowMaxSize < offset)
	{   // the logic can handle +30 expansion, but not more
		buffer[row] = (char*)realloc(buffer[row], sizeof(char) * (curRowMaxSize + offset));

		if (buffer[row] == NULL)
			AllocFailureProgTermination();

		curRowMaxSize += offset;
	}
	else if ((insertTextLength + rowTextLength + 2) - curRowMaxSize >= offset)
	{
		printf(">>The row is full or message too big to insert\n");
		return -1;
	}

	if ((rowTextLength + 1) < column)  // add spaces
	{

		for (int colIndex = rowTextLength; colIndex < column - 1; colIndex++)  
			buffer[row][colIndex] = ' ';  

		buffer[row][column - 1] = '\0';
		strcat_s(buffer[row], curRowMaxSize - 1, input);
	
		if (row > bufferRowCounter)  // so there is '\n' and we need to transfer it to the end
		{
			int curLength = strlen(buffer[row]);
			buffer[row][rowTextLength - 1] = ' ';
			buffer[row][curLength] = '\n';
			buffer[row][curLength + 1] = '\0';
		}
	}
	else if ((rowTextLength + 1) > column)
	{
		char* addBuffer = (char*)malloc(sizeof(char) * curRowMaxSize - 1);
		addBuffer[0] = '\0';
		char ch = '0';
		for (int colIndex = column; colIndex < rowTextLength; colIndex++)  
		{
			ch = buffer[row][colIndex];
			strncat_s(addBuffer, curRowMaxSize - 1, &ch, 1);  // one symbol at a time

		}
		buffer[row][column] = '\0';
 
		strcat_s(buffer[row], curRowMaxSize - 1, input);
		strcat_s(buffer[row], curRowMaxSize - 1, addBuffer);  

		free(addBuffer);
	}
	else  // only strcat
	{
		strcat_s(buffer[row], curRowMaxSize - 1, input);
	}
	printf(">>success\n");

	free(input);
	return 0;
}

int HandleSearch(char* input, Buffer buffer) 
{
	printf("Enter the substring to look for: ");
	fgets(input, ROWSIZE, stdin);
	RemoveEndNewLine(input);
	buffer.SearchSubstrPos(input);
	
	return 0;
}
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
	SEARCH = 8,
	UNDEFINED = 9
};

BOOL WINAPI ConsoleHandler(DWORD signal) {
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

void ExecuteCommand(enum Mode command, Buffer buffer)
{
	char* input;
	try
	{
		char* input = new char[ROWSIZE];
	}
	catch (const std::exception&)
	{
		AllocFailureProgTermination();
	}
		

	switch (command)
	{
	case USEREXIT:
		HandleUserExit(input, &buffer);
		break;

	case APPEND:
		HandleAppend(input, buffer);
		break;

	case NEWLINE:
		HandleNewLine(buffer);
		break;

	case SAVETOFILE:  
		HandleSaveToFile(input, buffer);
		break;

	case LOADFROMFILE:
		if(HandleLoadFromFile(input, buffer) == -1)  // problem reading the file
		{
			buffer.FlushText();  
		}
		break;

	case PRINTCURRENT:
		buffer.PrintCurrent();
		break;

	case INSERT:
		HandleInsert();
		break;

	case SEARCH:
		HandleSearch(input);
		break;

	case CLS:
		system("cls");
		break;

	case UNDEFINED:
		break;

	}
	delete[] input;
	input = nullptr;
}

void PrintMainMenu()
{
	int curLength = GetRowRemainLength(buffer, bufferRowCounter, ROWSIZE) - 1;
	printf("________________________________\n");
	printf("Row space left is %d symbols\nEnter a digit (your command):\n0 - exit, 1 - append, 2 - newline, 3 - save to a file, 4 - load from file, 5 - print current,\n6 - insert, 7 - search, 8 - clean screen;\n", curLength);
}

enum Mode GetUserCommand()
{
	printf("Enter number: ");
	enum Mode command;

	char* input = (char*)malloc(COMMANDSIZE * sizeof(char));
	fgets(input, COMMANDSIZE, stdin);
	RemoveEndNewLine(input);

	if (strlen(input) > 1) 
	{
		printf(">>The command is not implemmented\n");
		command = UNDEFINED;
		return command;
	}

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
		command = SEARCH;
		break;
	case '8':
		command = CLS;
		break;
	default:  
		printf(">>The command is not implemmented\n");
		command = UNDEFINED;
		break;
	}
	free(input);
	return command;
}

