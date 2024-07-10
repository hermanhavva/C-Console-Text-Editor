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

int main()
{

	if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE)) {
		fprintf(stderr, "Failed to set control handler\n");
		return EXIT_FAILURE;
	}

	SetConsoleTextAttribute(hout, BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN);
	system("cls");
	
	TextEditor* textEditor = nullptr;

	textEditor = new TextEditor(50, 50, "caesarDLL");  
	
	bool ifContinue = true;
	
	while (ifContinue)
	{
		textEditor->PrintMainMenu();
		Mode command = textEditor->GetUserCommand();
		if (command != UNDEFINED)
		{
			textEditor->ExecuteCommand(command, &ifContinue);
		}
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

