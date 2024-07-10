#include <windows.h>
#include <new>
#include "CommandEnum.h"
#include "CaesarCipher.h"
#include "TextEditor.h"
#include <stdexcept>
#include "common.h"
#include "Buffer.h"
#include <string>

TextEditor::TextEditor(const size_t rowNum, const size_t rowLength, const char* pathToDll)
{
    this->pathToDll = std::string(pathToDll);
    try
    {
		buffer = new Buffer(rowNum, rowLength);
    }
    catch (const std::runtime_error& er)
    {
        if (buffer != nullptr)
        {
            delete buffer;
        } 
		printf(er.what());  // TO DO: add an if which would ensure that the program runs without dll
		Sleep(1000);
		exit(1);
    }
}

TextEditor::~TextEditor()
{
	CloseFile(filePtr);
	filePtr = nullptr;
	delete caesarCipher;
	caesarCipher = nullptr;
	delete buffer;
	buffer = nullptr;
}


enum Mode TextEditor::GetUserCommand()
{
	printf("Enter number: ");
	enum Mode command;
	char* input = nullptr;
	const size_t COMMANDSIZE = 10;

	try
	{
		input = new char[COMMANDSIZE];
	}
	catch (const std::bad_alloc&)
	{
		delete this;
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
	else if (strlen(input) == 2 && input[0] == '1')
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

	IfInputSizeNotValidClearSTDin(input, COMMANDSIZE);

	delete[] input;
	input = nullptr;

	return command;
}

void TextEditor::ExecuteCommand(enum Mode command, bool* ifContinue)
{
	char* input = nullptr;
	size_t inputSize = buffer->GetRowSize() + 1;
	try
	{
		input = new char[inputSize];
	}
	catch (const std::bad_alloc&)
	{
		AllocFailureProgTermination(nullptr, buffer);
	}
	input[0] = '\0';

	switch (command)
	{
	case USEREXIT:
		*ifContinue = false;
		HandleUserExit(input);
		break;
	case APPEND:
		HandleAppend(input, inputSize);
		break;
	case NEWLINE:
		HandleNewLine();
		break;
	case SAVETOFILE:
		HandleSaveToFile(input, inputSize);
		break;
	case LOADFROMFILE:
		HandleLoadFromFile(input, inputSize);  // problem reading the file
		break;
	case PRINTCURRENT:
		buffer->PrintCurrent();
		break;
	case INSERT:
		HandleInsert(input, inputSize);
		break;
	case INSERTREPLACE:
		HandleInsertReplace(input, inputSize);
		break;
	case SETCURSOR:
		HandleSetCursor();
		break;
	case SEARCH:
		HandleSearch(input, inputSize);
		break;
	case DELETESTR:
		HandleDelete();
		break;
	case CUT:
		HandleCut();
		break;
	case COPY:
		HandleCopy();
		break;
	case PASTE:
		HandlePaste();
		break;
    case CIPHEER:
        HandleCipherTxtAction(input, inputSize);
        break;
	case UNDEFINED:
		break;

	}
	delete[] input;
	input = nullptr;
}

void TextEditor::HandleUserExit(char* input)
{
	printf(">>exiting\n");
	CloseFile(filePtr);

	delete buffer;
	buffer = nullptr;

	delete[] input;
	input = nullptr;

	delete this;


	Sleep(100);
	exit(0);
}

int TextEditor::HandleAppend(char* input, size_t inputSize)
{
	fgets(input, static_cast<int>(inputSize), stdin);
	if (!IfInputSizeNotValidClearSTDin(input, inputSize))
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

int TextEditor::HandleNewLine()
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

int TextEditor::HandleSaveToFile(char* input, size_t inputSize)
{
	errno_t err;  // to track the execution of fopen_s()
	printf("\nEnter the filename: ");
	fgets(input, static_cast<int>(inputSize), stdin);

	if (!IfInputSizeNotValidClearSTDin(input, inputSize))
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
		CloseFile(filePtr);
		filePtr = nullptr;
		return -1;
	}

	CloseFile(filePtr);
	filePtr = nullptr;
	printf(">>success\n");
	return 0;

}

int TextEditor::HandleLoadFromFile(char* input, size_t inputSize)
{
	errno_t err;  // to track the execution of fopen_s() 
	printf("\nCurrent text will be deleted, 1 - continue, 0 - cancel:\n");

	fgets(input, static_cast<int>(inputSize), stdin);
	RemoveEndNewLine(input);

	if (input[0] == '0' || !IfInputSizeNotValidClearSTDin(input, inputSize))
	{
		printf(">>failure\n");
		return -1;
	}

	printf("\nEnter the filename: ");

	fgets(input, static_cast<int>(inputSize), stdin);
	if (!IfInputSizeNotValidClearSTDin(input, inputSize))
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
		CloseFile(filePtr);
		filePtr = nullptr;
		return 0;
	default:
		printf(">>failure\n");
		CloseFile(filePtr);
		buffer->FlushText();
		filePtr = nullptr;
		return -1;
	}
}

int TextEditor::HandleInsert(char* input, size_t inputSize)
{
	Buffer::Cursor curCursor = buffer->GetCurCursor();  // returns a pointer

	printf("String to insert at %zd|%zd: ", curCursor.GetRow(), curCursor.GetColumn());
	fgets(input, static_cast<int>(inputSize), stdin);
	if (!IfInputSizeNotValidClearSTDin(input, inputSize))
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

int TextEditor::HandleInsertReplace(char* input, size_t inputSize)
{
	Buffer::Cursor curCursor = buffer->GetCurCursor();

	printf("Enter the message to insert at position %zd|%zd: ", curCursor.GetRow(), curCursor.GetColumn());
	fgets(input, static_cast<int>(inputSize), stdin);
	if (!IfInputSizeNotValidClearSTDin(input, inputSize))
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

int TextEditor::HandleSetCursor()
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

int TextEditor::HandleSearch(char* input, size_t inputSize)
{
	printf("Enter the substring to look for: ");
	fgets(input, static_cast<int>(inputSize), stdin);
	if (!IfInputSizeNotValidClearSTDin(input, inputSize))
	{
		printf(">>failure\n");
		return -1;
	}

	RemoveEndNewLine(input);
	system("cls");
	buffer->SearchSubstrPos(input);

	return 0;
}

int TextEditor::HandleDelete()
{
	Buffer::Cursor curCursor = buffer->GetCurCursor();
	unsigned int amountOfCharsToDelete = 0;

	printf("Enter amount of symbols to delete at %zd|%zd: ", curCursor.GetRow(), curCursor.GetColumn());
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

int TextEditor::HandleCut()
{
	unsigned int amountOfCharsToCut = 0;
	Buffer::Cursor curCursor = buffer->GetCurCursor();

	printf("Enter amount of symbols to cut from %zd|%zd: ", curCursor.GetRow(), curCursor.GetColumn());
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

int TextEditor::HandleCopy()
{
	unsigned int amountOfCharsToCopy = 0;
	Buffer::Cursor curCursor = buffer->GetCurCursor();

	printf("Enter amount of symbols to copy from %zd|%zd: ", curCursor.GetRow(), curCursor.GetColumn());
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

int TextEditor::HandlePaste()
{
	system("cls");
	if (buffer->PasteAtCursorPos() == -1)
	{
		printf(">>failure\n");
		return -1;
	}
	printf(">>success\n");
	return 0;
}

void TextEditor::PrintMainMenu() const
{
	size_t curLength = buffer->GetCurRowRemainLength();
	printf("________________________________\n");
	printf("Row space left is %zd symbols\nEnter a digit (your command):\n0 - exit,\t1 - append,\t2 - newline,\t"
		"3 - save to a file,\t4 - load from file,\t5 - print current,\n6 - insert,\t7 - insert & replace,\t\t"
		"8 - search,\t\t9 - set cursor pos,\t10 - delete,\n"
		"11 - undo,\t12 - redo,\t13 - cut,\t14 - copy,\t\t15 - paste;\t\t16 - Cipher txt\n", curLength);
}

void TextEditor::CloseFile(FILE* filePtr)
{
    if (filePtr != nullptr)
        fclose(filePtr);

    filePtr = nullptr;
}

int TextEditor::HandleCipherTxtAction(char* input, size_t inputSize)
{
    int	 key = 0;
	char ifEncrypt;
	if (!ifLibLoaded)  // use handle instead
	{
		try
		{
			caesarCipher = new CaesarCipher(pathToDll);
		}
		catch (const std::runtime_error& er)
		{
			printf(er.what());
			return -1;
		}
	}
	
	ifLibLoaded = true;

    printf("Enter the filepath for FROM file: ");
    fgets(input, static_cast<int>(inputSize), stdin);
    if (!IfInputSizeNotValidClearSTDin(input, inputSize))
    {
        system("cls");
        printf(">>failure\n");
        return -1;
    }
    RemoveEndNewLine(input);

    std::string fromPath = std::string(input);

    printf("Enter the filepath for OUT file: ");
    fgets(input, static_cast<int>(inputSize), stdin);
    if (!IfInputSizeNotValidClearSTDin(input, inputSize))
    {
        system("cls");
        printf(">>failure\n");
        return -1;
    }
    RemoveEndNewLine(input);

    std::string toPath = std::string(input);
    printf("Enter the key: ");
    scanf_s(" %d", &key);
    while ((getchar()) != '\n');  // to remove '\n' from stdin
    if (!caesarCipher->ifKeyValid(key))
    {
        system("cls");
        printf(">>failure, wrong key\n");
        return -1;
    }

	printf("1 - Encrypt, 0 - Decrypt: ");
	scanf_s(" %c", &ifEncrypt, 1);
	while ((getchar()) != '\n');  // to remove '\n' from stdin
	if (ifEncrypt != '0' && ifEncrypt != '1')
	{
		system("cls");
		printf(">>failure, wrong number\n");
		return -1;
	}
	if (fromPath == toPath)
	{
		printf(">>failure, the paths must be differnt\n");
		return -1;
	}
	try
	{
		switch (ifEncrypt)
		{
		case '1':
			if (caesarCipher->EncryptTxt(key, fromPath, toPath) == 0)
			{
				system("cls");
				printf(">>success\n");
				return 0;
			}
			break;
		case '0':
			if (caesarCipher->DecryptTxt(key, fromPath, toPath) == 0)
			{
				system("cls");
				printf(">>success\n");
				return 0;
			}
			break;
		}
	}
	catch (const std::runtime_error& er)
	{
		printf(er.what());
	}
    
    printf(">>failure\n");
    return -1;
 
}
		 
std::string TextEditor::GetDllPath()
{
	return pathToDll;
}
void TextEditor::SetDllPath(std::string pathToDll)
{
	this->pathToDll = pathToDll;
}
