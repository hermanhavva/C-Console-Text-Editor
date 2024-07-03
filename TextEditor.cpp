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
    std::string dllPath = std::string(pathToDll);
    try
    {
        buffer = new Buffer(rowNum, rowLength);
        caesarCipher = new CaesarCipher(dllPath);
    }
    catch (const std::runtime_error& er)
    {
        if (buffer != nullptr)
        {
            delete buffer;
        } 
		printf(er.what());  // TO DO: add an if which would ensure that the program runs without dll 
    }
}

TextEditor::~TextEditor()
{
	CloseFile(filePtr);
    delete caesarCipher;
    filePtr = nullptr;
	delete buffer;
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

	if (input[0] == '0' || !IsInputSizeValid(input, inputSize))
	{
		printf(">>failure\n");
		return -1;
	}

	printf("\nEnter the filename: ");

	fgets(input, static_cast<int>(inputSize), stdin);
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

int TextEditor::HandleInsertReplace(char* input, size_t inputSize)
{
	Buffer::Cursor curCursor = buffer->GetCurCursor();

	printf("Enter the message to insert at position %zd|%zd: ", curCursor.GetRow(), curCursor.GetColumn());
	fgets(input, static_cast<int>(inputSize), stdin);
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

    printf("Enter the filepath for FROM file: ");
    fgets(input, static_cast<int>(inputSize), stdin);
    if (!IsInputSizeValid(input, inputSize))
    {
        system("cls");
        printf(">>failure\n");
        return -1;
    }
    RemoveEndNewLine(input);

    std::string fromPath = std::string(input);

    printf("Enter the filepath for OUT file: ");
    fgets(input, static_cast<int>(inputSize), stdin);
    if (!IsInputSizeValid(input, inputSize))
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
		 

