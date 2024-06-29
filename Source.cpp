#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <new>

//#include "Buffer.h"
#include <string> 
#include <stdexcept>
#include "common.h"
#include "CommandEnum.h"
//#include "common.h"
//#include "CaesarCipher.h"
#include "TextEditorCLI.h"
//using namespace std;

FILE* filePtr = nullptr;
HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);

enum Mode;
BOOL WINAPI ConsoleHandler(DWORD);
//void ExecuteCommand(enum Mode, Buffer*, bool*);

enum Mode GetUserCommand(TextEditor* buffer);
/*
class CaesarCipher
{
public:
	CaesarCipher(string pathToDll);
	~CaesarCipher();

	string Encrypt(string message, const int key);
	string Decrypt(string message, const int key);
	size_t GetBufferCapacity() const;

private:
	char* GetNextChunk(string&);
	// dll function pointers
	typedef char* (*encrypt_ptr_t)(char*, const int);
	typedef char* (*decrypt_ptr_t)(char*, int);
	// Const dll function naming
	const char* encryptProcName = "EncryptStr";
	const char* decryptProcName = "DecryptStr";
	// Proc pointers 
	encrypt_ptr_t encryptProcAddress = nullptr;
	decrypt_ptr_t decryptProcAddress = nullptr;

	HINSTANCE dllHandle;

	char* chunkBuffer = nullptr;
	const size_t BUFFERCAPACITY = 128;
};
*/
int main()
{

	if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE)) {
		fprintf(stderr, "Failed to set control handler\n");
		return EXIT_FAILURE;
	}
	SetConsoleTextAttribute(hout, BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN);
	system("cls");

	TextEditor* textEditor = new TextEditor(150, 150, "caesarDLL");
	CaesarCipher* caesarCipher = new CaesarCipher("hello");
	delete caesarCipher;
	bool ifContinue = true;
	
	while (ifContinue)
	{
		/*
		CaesarCipher* cipherInstance = nullptr;
		try
		{
			cipherInstance = new CaesarCipher("caesarDLL.dll");
		}
		catch (const std::exception& e)
		{
			printf(e.what());
			break;
		}
		string result = cipherInstance->EncryptStr("xyzxyzxyzxyzxyzxyzxyzxyzxyz", 1);
		printf(result.c_str());
		
		delete cipherInstance;
		break;
		/*




		/*
		PrintMainMenu(buffer);
		enum Mode command = GetUserCommand(buffer);
		ExecuteCommand(command, buffer, &ifContinue);
		*/
		textEditor->PrintMainMenu();
		
		Mode command = GetUserCommand(textEditor);
		textEditor->ExecuteCommand(command, &ifContinue);

	}
	
	delete textEditor;

	Sleep(100);

	return 0;
}
/*
CaesarCipher::CaesarCipher(string pathToDll)
{
	// Initializing an object of wstring
	wstring temp = wstring(pathToDll.begin(), pathToDll.end());
	LPCWSTR wideStringPath = temp.c_str();

	dllHandle = LoadLibrary(TEXT("C:\\C projects\\C-Console-Text-Editor1\\Text Editor\\caesarDLL\\x64\\Debug\\caesarDLL.dll"));
	if (dllHandle == nullptr || dllHandle == INVALID_HANDLE_VALUE)
	{
		throw std::runtime_error(std::string("Failed to load the DLL\n"));
	}

	encryptProcAddress = (encrypt_ptr_t)GetProcAddress(dllHandle, encryptProcName);
	decryptProcAddress = (decrypt_ptr_t)GetProcAddress(dllHandle, decryptProcName);

	if (decryptProcAddress == nullptr || encryptProcAddress == nullptr)
	{
		FreeLibrary(dllHandle);
		dllHandle = nullptr;
		throw std::runtime_error(std::string("Failed to find the procedure\n"));
	}

	chunkBuffer = new char[BUFFERCAPACITY + 1];  // +1 for '\0'
}
CaesarCipher::~CaesarCipher()
{
	FreeLibrary(dllHandle);
	delete[] chunkBuffer;
}

string CaesarCipher::Encrypt(string message, const int key)
{
	if (key > 26 || key < -26) {
		throw std::runtime_error(std::string("The key is too big\n"));
	}
	if (message.length() == 0)
	{
		return "";
	}

	size_t messageSize = message.length() - 1;  // counting from 0
	string encryptedMessage = "";

	// chunkBuffer[BUFFERCAPACITY] = '\0'
	if (messageSize >= BUFFERCAPACITY)  // chunk logic: -1 is for counting from 0     
	{
		const int chunkAmount = (int)ceil((messageSize + 1) / (BUFFERCAPACITY + 1 - 1));  // rounding and casting

		for (int i = 0; i < chunkAmount; i++)
		{
			encryptedMessage += string(encryptProcAddress(GetNextChunk(message), key));
		}
	}

	else
	{
		strcpy_s(chunkBuffer, BUFFERCAPACITY, message.c_str());
		encryptProcAddress(chunkBuffer, key);
		encryptedMessage = string(chunkBuffer);
	}

	return encryptedMessage;
}

string CaesarCipher::Decrypt(string message, const int key)
{
	if (key > 26 || key < -26) {
		throw std::runtime_error(std::string("The key is too big\n"));
	}
	if (message.length() == 0)
	{
		return "";
	}

	size_t messageSize = message.length() - 1;  // counting from 0
	string decryptedMessage = "";

	if (messageSize >= BUFFERCAPACITY)  // chunk logic: -1 is for counting from 0     
	{
		const int chunkAmount = (int)ceil((messageSize + 1) / (BUFFERCAPACITY + 1 - 1));  // rounding and casting

		for (int i = 0; i < chunkAmount; i++)
		{
			decryptedMessage += string(encryptProcAddress(GetNextChunk(message), key));
		}
	}

	else
	{
		strcpy_s(chunkBuffer, BUFFERCAPACITY, message.c_str());
		decryptProcAddress(chunkBuffer, key);
		decryptedMessage = string(chunkBuffer);
	}

	return decryptedMessage;

}

char* CaesarCipher::GetNextChunk(string& message)
{
	size_t messageSize = message.length() - 1;  // -1 for counting from 0 

	for (size_t index = 0; index <= BUFFERCAPACITY - 1 && index <= messageSize; index++)
	{
		chunkBuffer[index] = message[index];
	}
	if (messageSize >= BUFFERCAPACITY)
	{
		chunkBuffer[BUFFERCAPACITY] = '\0';
		message.erase(0, BUFFERCAPACITY + 1);  // +1 because .erase counts from 1 
	}
	else
	{
		chunkBuffer[messageSize] = '\0';
		message.erase(0, messageSize + 1);  // here it becomes an empty string
	}
	return chunkBuffer;
}

size_t CaesarCipher::GetBufferCapacity() const
{
	return BUFFERCAPACITY;
}

*/

/*
void HandleUserExit(char* input, Buffer* buffer)
{
	printf(">>exiting\n");
	buffer->CloseFile(filePtr);

	delete buffer;
	buffer = nullptr;

	delete[] input;
	input = nullptr;

	Sleep(100);
	exit(0);
}

int HandleAppend(char* input, int inputSize, Buffer* buffer)
{
	fgets(input, inputSize, stdin);
	if (!IsInputSizeValid(input, inputSize))
	{
		printf(">>failure\n");
		return -1;
	}

	RemoveEndNewLine(input);  // removing '\n'
	system("cls");

	if (buffer->Append(input) == 0)
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

int HandleNewLine(Buffer* buffer)
{
	system("cls");
	if (buffer->AddRow() == 0)
	{
		printf(">>success\n");
		return 0;
	}
	printf(">>failure\n");
	return -1;
}

int HandleSaveToFile(char* input, int inputSize, Buffer* buffer)
{
	errno_t err;  // to track the execution of fopen_s()
	printf("\nEnter the filename: ");
	fgets(input, inputSize, stdin);

	if (!IsInputSizeValid(input, inputSize))
	{
		printf(">>failure\n");
		return -1;
	}

	RemoveEndNewLine(input);  // removing '\n'

	err = fopen_s(&filePtr, input, "a+");
	system("cls");

	if (err != 0 || filePtr == nullptr)  // returns 0 if successful
	{
		printf("\nCould not open the file >>failure");
		return -1;
	}
	if (buffer->SaveToFile(filePtr) == -1)  // BETTER PUT A BOOL flag and make methods bool functions
	{
		printf(">>failure\n");
		buffer->CloseFile(filePtr);
		filePtr = nullptr;
		return -1;
	}

	buffer->CloseFile(filePtr);
	filePtr = nullptr;
	printf(">>success\n");
	return 0;

}

int HandleLoadFromFile(char* input, int inputSize, Buffer* buffer)
{
	errno_t err;  // to track the execution of fopen_s() 
	printf("\nCurrent text will be deleted, 1 - continue, 0 - cancel:\n");

	fgets(input, inputSize, stdin);
	RemoveEndNewLine(input);

	if (input[0] == '0' || !IsInputSizeValid(input, inputSize))
	{
		printf(">>failure\n");
		return -1;
	}

	printf("\nEnter the filename: ");

	fgets(input, inputSize, stdin);
	if (!IsInputSizeValid(input, inputSize))
	{
		printf(">>failure\n");
		return -1;
	}

	RemoveEndNewLine(input);

	err = fopen_s(&filePtr, input, "r");
	if (err != 0 || filePtr == NULL)  // returns 0 if successful
	{
		system("cls");
		printf("\nCould not open the file\n");
		return -1;
	}
	system("cls");

	switch (buffer->LoadFromFile(filePtr))
	{
	case 0:
		printf(">>success\n");
		buffer->CloseFile(filePtr);
		filePtr = nullptr;
		return 0;
	default:
		printf(">>failure\n");
		buffer->CloseFile(filePtr);
		buffer->FlushText();
		filePtr = nullptr;
		return -1;
	}
}

int HandleInsert(char* input, int inputSize, Buffer* buffer)
{
	Buffer::Cursor curCursor = buffer->GetCurCursor();  // returns a pointer

	printf("String to insert at %d|%d: ", curCursor.GetRow(), curCursor.GetColumn());
	fgets(input, inputSize, stdin);
	if (!IsInputSizeValid(input, inputSize))
	{
		printf(">>failure\n");
		return -1;
	}

	RemoveEndNewLine(input);  // for omitting '\n'
	system("cls");

	if (buffer->InsertAtCursorPos(input) == -1)
	{
		printf("<<failure\n");
		return -1;
	}

	printf("<<success\n");
	return 0;
}

int HandleInsertReplace(char* input, int inputSize, Buffer* buffer)
{
	Buffer::Cursor curCursor = buffer->GetCurCursor();

	printf("Enter the message to insert at position %d|%d: ", curCursor.GetRow(), curCursor.GetColumn());
	fgets(input, inputSize, stdin);
	if (!IsInputSizeValid(input, inputSize))
	{
		printf(">>failure\n");
		return -1;
	}


	RemoveEndNewLine(input);
	system("cls");

	if (buffer->InsertReplaceAtCursorPos(input) == -1)
	{
		printf(">>failure\n");
		return -1;
	}

	printf(">>success\n");

	return 0;
}

int HandleSetCursor(Buffer* buffer)
{
	int row = 0;
	int column = 0;

	printf("\nEnter the Row: ");

	scanf_s(" %d", &row);
	printf("Enter the Column: ");
	scanf_s(" %d", &column);

	while ((getchar()) != '\n');  // '\n' to get out of the in buffer
	system("cls");


	if (buffer->SetCursorPosition(row, column) == -1)
	{
		printf(">>failure\n");
		return -1;
	}

	printf(">>success\n");
	return 0;

}

int HandleSearch(char* input, int inputSize, Buffer* buffer)
{
	printf("Enter the substring to look for: ");
	fgets(input, inputSize, stdin);
	if (!IsInputSizeValid(input, inputSize))
	{
		printf(">>failure\n");
		return -1;
	}

	RemoveEndNewLine(input);
	system("cls");
	buffer->SearchSubstrPos(input);

	return 0;
}

int HandleDelete(Buffer* buffer)
{
	Buffer::Cursor curCursor = buffer->GetCurCursor();
	unsigned int amountOfCharsToDelete = 0;

	printf("Enter amount of symbols to delete at %d|%d: ", curCursor.GetRow(), curCursor.GetColumn());
	scanf_s(" %u", &amountOfCharsToDelete);
	while ((getchar()) != '\n');  // to remove '\n' from stdin
	system("cls");

	if (buffer->DeleteAtCursorPos(amountOfCharsToDelete, false) == -1)
	{
		printf(">>failure\n");
		return -1;
	}

	printf(">>success\n");

	return 0;
}

int HandleCut(Buffer* buffer)
{
	unsigned int amountOfCharsToCut = 0;
	Buffer::Cursor curCursor = buffer->GetCurCursor();

	printf("Enter amount of symbols to cut from %d|%d: ", curCursor.GetRow(), curCursor.GetColumn());
	scanf_s("%u", &amountOfCharsToCut);
	while ((getchar()) != '\n');  // to get '\n' out of stdin
	system("cls");

	if (buffer->DeleteAtCursorPos(amountOfCharsToCut, true) == -1)
	{
		printf(">>failure\n");
		return -1;
	}
	printf(">>success\n");
	return 0;
}

int HandleCopy(Buffer* buffer)
{
	unsigned int amountOfCharsToCopy = 0;
	Buffer::Cursor curCursor = buffer->GetCurCursor();

	printf("Enter amount of symbols to copy from %d|%d: ", curCursor.GetRow(), curCursor.GetColumn());
	scanf_s("%u", &amountOfCharsToCopy);
	while ((getchar()) != '\n');  // to get '\n' out of stdin
	system("cls");

	if (buffer->CopyAtCursorPos(amountOfCharsToCopy) == -1)
	{
		printf(">>failure\n");
		return -1;
	}
	printf(">>success\n");
	return 0;
}

int HandlePaste(Buffer* buffer)
{
	system("cls");
	if (buffer->PasteAtCursorPos() == -1)
	{
		printf(">>failure\n");
		return -1;
	}
	printf(">>success\n");
	return 0;
}*/
/*
enum Mode
{
	USEREXIT = 0,
	APPEND = 1,
	NEWLINE = 2,
	SAVETOFILE = 3,
	LOADFROMFILE = 4,
	PRINTCURRENT = 5,
	INSERT = 6,
	INSERTREPLACE = 7,
	SEARCH = 8,
	SETCURSOR = 9,
	DELETESTR = 10,
	UNDO = 11,
	REDO = 12,
	CUT = 13,
	COPY = 14,
	PASTE = 15,
	CLS = 16,
	UNDEFINED

};*/

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

/*
void ExecuteCommand(enum Mode command, Buffer* buffer, bool* ifContinue)
{
	char* input = nullptr;
	int inputSize = buffer->GetRowSize() + 1;

	try
	{
		input = new char[inputSize];
	}
	catch (const std::bad_alloc&)
	{
		buffer->AllocFailureProgTermination(nullptr);
	}


	switch (command)
	{
	case USEREXIT:
		*ifContinue = false;
		HandleUserExit(input, buffer);
		break;

	case APPEND:
		HandleAppend(input, inputSize, buffer);
		break;

	case NEWLINE:
		HandleNewLine(buffer);
		break;

	case SAVETOFILE:
		HandleSaveToFile(input, inputSize, buffer);
		break;

	case LOADFROMFILE:
		HandleLoadFromFile(input, inputSize, buffer);  // problem reading the file
		break;

	case PRINTCURRENT:
		buffer->PrintCurrent();
		break;

	case INSERT:
		HandleInsert(input, inputSize, buffer);
		break;

	case INSERTREPLACE:
		HandleInsertReplace(input, inputSize, buffer);
		break;

	case SETCURSOR:
		HandleSetCursor(buffer);
		break;

	case SEARCH:
		HandleSearch(input, inputSize, buffer);
		break;
	case DELETESTR:
		HandleDelete(buffer);
		break;
	case CUT:
		HandleCut(buffer);
		break;
	case COPY:
		HandleCopy(buffer);
		break;
	case PASTE:
		HandlePaste(buffer);
		break;

	case CLS:
		system("cls");
		break;

	case UNDEFINED:
		break;

	}
	delete[] input;
	input = nullptr;
}*/
/*
void PrintMainMenu(Buffer* buffer)
{
	int curLength = buffer->GetCurRowRemainLength();
	printf("________________________________\n");
	printf("Row space left is %d symbols\nEnter a digit (your command):\n0 - exit,\t1 - append,\t2 - newline,\t"
		"3 - save to a file,\t4 - load from file,\t5 - print current,\n6 - insert,\t7 - insert & replace,\t\t"
		"8 - search,\t\t9 - set cursor pos,\t10 - delete,\n"
		"11 - undo,\t12 - redo,\t13 - cut,\t14 - copy,\t\t15 - paste;\t\t16 - clean screen\n", curLength);
}
*/


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
		AllocFailureProgTermination(nullptr, textEditor);
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
			command = ENCRYPT;
			break;
		case '7':
			command = DECRYPT;
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

