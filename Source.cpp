#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <new>
#include <string> 
#include <stdexcept>
#include "common.h"
#include "CommandEnum.h"
#include "CaesarCipher.h"
#include "TextEditor.h"

FILE* filePtr = nullptr;
HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);

enum Mode;
BOOL WINAPI ConsoleHandler(DWORD);
enum Mode GetUserCommand(TextEditor* buffer);

int main()
{

	if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE)) {
		fprintf(stderr, "Failed to set control handler\n");
		return EXIT_FAILURE;
	}

	SetConsoleTextAttribute(hout, BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN);
	system("cls");
	
	TextEditor* textEditor = nullptr;
	try
	{
		textEditor = new TextEditor(150, 150, "caesarDLL");  
	}
	catch (const std::runtime_error e)
	{
		printf(e.what());
	}
	
	bool ifContinue = true;
	
	while (ifContinue)
	{
		textEditor->PrintMainMenu();
		Mode command = GetUserCommand(textEditor);
		textEditor->ExecuteCommand(command, &ifContinue);
	}
	
	delete textEditor;
	Sleep(100);

	return 0;
}

BOOL WINAPI ConsoleHandler(DWORD signal) {
	if (signal == CTRL_C_EVENT || signal == CTRL_CLOSE_EVENT || signal == CTRL_BREAK_EVENT ||
		signal == CTRL_LOGOFF_EVENT || signal == CTRL_SHUTDOWN_EVENT) {
		printf("\nProgram interrupted. Cleaning up...\n");
		Sleep(1000);

		if (filePtr != nullptr)
			fclose(filePtr);
		exit(0);
	}
	return TRUE;
}

enum Mode GetUserCommand(TextEditor* textEditor)
{ 
	printf("Enter number: ");
	enum Mode command;
	char* input = nullptr;
	const int COMMANDSIZE = 10;

	try
	{
		input = new char[COMMANDSIZE];
	}
	catch (const std::bad_alloc&)
	{
		delete textEditor;
		AllocFailureProgTermination(nullptr, nullptr);
	}

	fgets(input, COMMANDSIZE, stdin);
	RemoveEndNewLine(input);

	if (strlen(input) == 1)
	{
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
			command = INSERTREPLACE;
			break;
		case '8':
			command = SEARCH;
			break;
		case '9':
			command = SETCURSOR;
			break;
		default:
			printf(">>The command is not implemmented\n");
			command = UNDEFINED;
			break;
		}
	}
	else if (strlen(input) == 2 && (int)input[0] >= 48 && (int)input[0] <= 57)
	{
		switch (input[1])
		{
		case '0':
			command = DELETESTR;
			break;
		case '1':
			command = UNDO;
			break;
		case '2':
			command = REDO;
			break;
		case '3':
			command = CUT;
			break;
		case '4':
			command = COPY;
			break;
		case '5':
			command = PASTE;
			break;
		case '6':
			command = CIPHEER;
			break;
		default:
			command = UNDEFINED;
			printf(">>The command is not implemmented\n");
			break;
		}
	}
	else
	{
		printf(">>The command is not implemmented\n");
		command = UNDEFINED;
	}

	if (command == UNDEFINED)
	{
		while ((getchar()) != '\n');  // to clear the buffer of garbage symbols
	}

	delete[] input;
	return command;
}

