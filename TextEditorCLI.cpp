
//#include "Buffer.h"
#include <windows.h>
#include <new>

#include "CommandEnum.h"
#include "CaesarCipher.h"
#include "TextEditorCLI.h"
#include <stdexcept>
#include "common.h"
#include <string>

TextEditor::TextEditor(const int rowNum, const int rowLength, const char* pathToDll)
{
    std::string dllPath = std::string(pathToDll);
   // caesarCipher = new CaesarCipher(dllPath);
    try
    {
        buffer = new Buffer(rowNum, rowLength);
        caesarCipher = new CaesarCipher(dllPath);
    }
    catch (const std::runtime_error& e)
    {
        if (buffer != nullptr)
        {
            delete buffer;
        }
        throw e;
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
    case ENCRYPT:
        HandleCaesarTxtAction(input, inputSize, true);
        break;
    case DECRYPT:
        HandleCaesarTxtAction(input, inputSize, false);
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

	Sleep(100);
	exit(0);
}

int TextEditor::HandleAppend(char* input, size_t inputSize)
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

int TextEditor::HandleInsertReplace(char* input, size_t inputSize)
{
	Buffer::Cursor curCursor = buffer->GetCurCursor();

	printf("Enter the message to insert at position %zd|%zd: ", curCursor.GetRow(), curCursor.GetColumn());
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
		"11 - undo,\t12 - redo,\t13 - cut,\t14 - copy,\t\t15 - paste;\t\t16 - Encrypt\n", curLength);
}


void TextEditor::CloseFile(FILE* filePtr)
{
    if (filePtr != nullptr)
        fclose(filePtr);

    filePtr = nullptr;
}

int TextEditor::HandleCaesarTxtAction(char* input, size_t inputSize, bool ifEncrypt)
{
    int key = 0;
    printf("Enter the filepath for FROM file: ");
    fgets(input, inputSize, stdin);
    if (!IsInputSizeValid(input, inputSize))
    {
        system("cls");
        printf(">>failure\n");
        return -1;
    }
    RemoveEndNewLine(input);

    std::string fromPath = std::string(input);

    printf("Enter the filepath for OUT file: ");
    fgets(input, inputSize, stdin);
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
        printf(">>failure\n");
        return -1;
    }
    
    switch(ifEncrypt)
    {
    case true:
        if (caesarCipher->EncryptTxt(key, fromPath, toPath) == 0)
        {
            system("cls");
            printf(">>success");
            return 0;
        }
        break;
    case false:
        if (caesarCipher->DecryptTxt(key, fromPath, toPath) == 0)
        {
            system("cls");
            printf(">>success");
            return 0;
        }
        break;
    }
    system("cls");
    printf(">>failure\n");
    return -1;
 
}


//
// BUFFFER LOGIC 
//				 

TextEditor::Buffer::Buffer(const size_t defaultRowNum, const size_t defaultRowLength)  // constructor
{
    if (defaultRowNum < 1 || defaultRowLength < 1)
    {
        throw std::runtime_error("(Exception at buffer constructor) Number of rows or row length is too small(must be >0)\n");
    }

    curCursor = new Cursor(0, 0);
    this->defaultRowNum = defaultRowNum;
    this->defaultRowLength = defaultRowLength;
    InitializeBuffer();
}

TextEditor::Buffer::~Buffer()  // destructor 
{
    delete curCursor;
    curCursor = nullptr;
    if (text != nullptr)
    {
        for (int rowIndex = 0; rowIndex <= totalRowCounter; rowIndex++)
        {
            delete[] text[rowIndex];
        }
    }
    delete[] pasteBuffer;
    pasteBuffer = nullptr;
    delete[] text;
    text = nullptr;
}

TextEditor::Buffer::Cursor::Cursor(size_t row, size_t column)
{
    this->row = row;
    this->column = column;
}

void TextEditor::Buffer::Cursor::SetRow(size_t row)
{
    this->row = row;
}

void TextEditor::Buffer::Cursor::SetColumn(size_t column)
{
    this->column = column;
}

size_t TextEditor::Buffer::Cursor::GetRow() const
{
    return row;
}

size_t TextEditor::Buffer::Cursor::GetColumn() const
{
    return column;
}

int TextEditor::Buffer::InitializeBuffer()
{
    try
    {
        text = new char* [defaultRowNum];
    }
    catch (const std::bad_alloc&)
    {
        printf("Allocation failed");
        return -1;
    }
    for (int row = 0; row < defaultRowNum; row++)
    {
        text[row] = nullptr;
    }
    try
    {
        text[0] = new char[defaultRowLength];
        pasteBuffer = new char[defaultRowLength];
    }
    catch (const std::bad_alloc&)
    {
        printf("Allocation failed");
        return -1;
    }

    totalRowCounter = 0;

    text[0][0] = '\0';
    pasteBuffer[0] = '\0';
    SetCursorPosition(0, 0);

    return 0;
}

int TextEditor::Buffer::Append(char* input) // update cursor with the values of the end of the buffer 
{
    size_t curRow = totalRowCounter;

    if (GetCurRowRemainLength() > (int)strlen(input))
    {
        strcat_s(text[curRow], defaultRowLength, input);
    }
    else
    {
        printf("\nError, buffer too small, start a newline");
        return -1;
    }
    if (MoveCursorToEnd() == -1)
    {
        return -1;
        printf("\nError moving cursor");
    }

    return 0;
}

int TextEditor::Buffer::AddRow()
{
    if (text == nullptr)
    {
        return -1;
    }

    size_t curRow = curCursor->GetRow();
    size_t curColumn = curCursor->GetColumn();

    if (curRow >= defaultRowNum - 1 ||
        curRow < 0 ||
        totalRowCounter >= defaultRowNum - 1 ||
        curColumn < 0 ||
        curColumn >= defaultRowLength)
    {
        printf("\nThe buffer is too small");
        return -1;
    }

    char* addBuffer = nullptr;
    try
    {
        addBuffer = new char[defaultRowLength];
        addBuffer[0] = '\0';
    }
    catch (const std::bad_alloc&)
    {
        AllocFailureProgTermination(nullptr, this);
    }

    if (curRow < totalRowCounter)  // need to do resize
    {
        totalRowCounter++;

        try
        {
            text[totalRowCounter] = new char[defaultRowLength];
        }
        catch (const std::bad_alloc&)
        {
            totalRowCounter--;
            AllocFailureProgTermination(nullptr, this);
        }
        text[totalRowCounter][0] = '\0';

        for (size_t rowIndex = totalRowCounter - 1; rowIndex > curRow; rowIndex--)
        {
            strcpy_s(addBuffer, defaultRowLength, text[rowIndex]);
            strcpy_s(text[rowIndex + 1], defaultRowLength, addBuffer);
        }

        text[curRow + 1][0] = '\0';
    }

    else  // resize is not needed
    {
        try
        {
            text[curRow + 1] = new char[defaultRowLength];
        }
        catch (const std::bad_alloc&)
        {
            AllocFailureProgTermination(nullptr, this);
        }
        text[curRow + 1][0] = '\0';
        totalRowCounter++;
    }

    if (curColumn < strlen(text[curRow]))   // need to move text to another row
    {
        addBuffer[0] = '\0';
        for (size_t columnIndex = curColumn; columnIndex < strlen(text[curRow]); columnIndex++)
        {
            char curSymbol = text[curRow][columnIndex];
            strncat_s(addBuffer, defaultRowLength, &curSymbol, 1);
        }
        text[curRow][curColumn] = '\0';
        strcat_s(text[curRow + 1], defaultRowLength, addBuffer);
    }
    SetCursorPosition(curRow + 1, 0);

    delete[] addBuffer;
    return 0;
}

int TextEditor::Buffer::SaveToFile(FILE* filePtr)
{
    char newLineCh = '\n';

    if (filePtr == nullptr)
    {
        return -1;
    }
    size_t textSize = sizeof(char) * defaultRowLength * defaultRowNum + defaultRowNum;  // +defaultRowNum is for '\n'
    char* textToSave = nullptr;
    try
    {
        textToSave = new char[textSize];
    }
    catch (const std::bad_alloc&)
    {
        AllocFailureProgTermination(filePtr, this);
    }
    textToSave[0] = '\0';

    for (int row = 0; row <= totalRowCounter; row++) {
        strcat_s(textToSave, textSize, text[row]);
        strncat_s(textToSave, textSize, &newLineCh, 1);
    }

    fprintf_s(filePtr, "%s", textToSave);

    delete[] textToSave;
    return 0;


}

int TextEditor::Buffer::LoadFromFile(FILE* filePtr)
{
    if (filePtr == nullptr)
        return -1;

    if (text != nullptr)  // flushing the buffer if it has been initialized before
        FlushText();
    else
        return -1;

    const int FILESIZE = GetTxtSize(filePtr);
    char* textFromTxt = nullptr;

    try
    {
        textFromTxt = new char[FILESIZE + 2];  // add 2 to make sure it is in the bounds
    }
    catch (const std::bad_alloc&)
    {
        AllocFailureProgTermination(filePtr, this);
    }

    fread(textFromTxt, sizeof(char), FILESIZE, filePtr);
    textFromTxt[FILESIZE] = '\0';

    // now write to the buffer
    for (int index = 0; index <= FILESIZE; index++)
    {
        char curSymbol = textFromTxt[index];

        // some garbage symbols may appear in textFromTxt so to omit them there is a condition
        if ((int)curSymbol <= 255 && (int)curSymbol > 0) {
            if (curSymbol == '\n')  // we do not transfer '\n' from file to buffer directly
            {
                if (AddRow() == -1)
                {
                    printf("The file is too long\n");
                    delete[] textFromTxt;
                    return -1;
                }
                continue;
            }
            if (GetCurRowRemainLength() > 1)  // 1 to keep '\0'
            {
                strncat_s(text[totalRowCounter], defaultRowLength, &curSymbol, 1);
                MoveCursorToEnd();
            }
            else
            {
                printf("Lines in file are too long, loosing content\n");
                delete[] textFromTxt;
                return -1;
            }
        }
        else if (curSymbol == '\0')
        {
            MoveCursorToEnd();
            break;
        }

    }
    delete[] textFromTxt;

    return 0;
}

void TextEditor::Buffer::PrintCurrent()
{
    printf("Current text: \n________________________________\n");

    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    for (int row = 0; row <= totalRowCounter; row++)
    {
        if (row == curCursor->GetRow())
        {
            for (int column = 0; column <= strlen(text[row]); column++)
            {
                if (column == curCursor->GetColumn())
                {
                    SetConsoleTextAttribute(hout, BACKGROUND_INTENSITY | BACKGROUND_BLUE | BACKGROUND_GREEN);
                    switch (text[row][column])
                    {
                    case '\0':  // '\0' is not colored
                        printf(" \n");
                        break;
                    default:
                        printf("%c", text[row][column]);
                        break;
                    }
                    SetConsoleTextAttribute(hout, BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN);
                    continue;
                }
                switch (text[row][column])
                {
                case '\0':
                    printf("\n");
                    break;
                default:
                    printf("%c", text[row][column]);
                    break;
                }
            }
            continue;
        }
        printf("%s\n", text[row]);
    }


    printf("\n");
}

int TextEditor::Buffer::InsertAtCursorPos(char* input)
{
    size_t row = curCursor->GetRow();
    size_t column = curCursor->GetColumn();

    if (column >= defaultRowLength)   // this might be unnecessary
    {
        printf(">>Not enough space (might use newline first)\n");
        return -1;
    }

    size_t rowTextLength = strlen(text[row]);  // on this index there is '\0'
    size_t insertTextLength = strlen(input);
    size_t curRowMaxSize = defaultRowLength;

    if ((insertTextLength + rowTextLength) >= defaultRowLength)
    {
        printf(">>Not enough space (might use newline first)\n");
        return -1;
    }

    if ((rowTextLength + 1) < column)  // add spaces
    {

        for (size_t colIndex = rowTextLength; colIndex < column - 1; colIndex++)
            text[row][colIndex] = ' ';

        text[row][column - 1] = '\0';
        strcat_s(text[row], curRowMaxSize, input);

    }
    else if ((rowTextLength /*/ + 1*/) > column)
    {
        char* addBuffer = new char[curRowMaxSize];
        addBuffer[0] = '\0';
        char ch = '0';
        for (size_t colIndex = column; colIndex < rowTextLength; colIndex++)
        {
            ch = text[row][colIndex];
            strncat_s(addBuffer, curRowMaxSize, &ch, 1);  // one symbol at a time

        }
        text[row][column] = '\0';

        strcat_s(text[row], curRowMaxSize, input);
        strcat_s(text[row], curRowMaxSize, addBuffer);

        delete[] addBuffer;
    }
    else  // only strcat
    {
        strcat_s(text[row], curRowMaxSize, input);
    }
    SetCursorPosition(row, column + insertTextLength);
    return 0;
}

int TextEditor::Buffer::InsertReplaceAtCursorPos(char* input)
{
    size_t row = curCursor->GetRow();
    size_t column = curCursor->GetColumn();

    if (column >= defaultRowLength)   // this might be unnecessary
    {
        printf(">>Not enough space (might use newline first)\n");
        return -1;
    }
    size_t rowTextLength = strlen(text[row]);  // on this index there is '\0'
    size_t insertTextLength = strlen(input);


    if (column + insertTextLength >= defaultRowLength)  // got to check the edge case
    {
        printf(">>Not enough space (might use newline first)\n");
        return -1;
    }
    for (size_t columnIndex = column; columnIndex < column + insertTextLength; columnIndex++)
    {
        char curChar = input[columnIndex - column];
        text[row][columnIndex] = curChar;
    }
    if (column + insertTextLength >= rowTextLength)
    {
        text[row][column + insertTextLength] = '\0';
    }
    SetCursorPosition(row, column + insertTextLength);

    return 0;
}

void TextEditor::Buffer::SearchSubstrPos(char* subString)
{
    printf("It can be found on positions(row|column): ");

    for (int row = 0; row <= totalRowCounter; row++)
    {
        for (int column = 0; column < (int)strlen(text[row]); column++)
        {
            bool ifPresent = true;
            if (text[row][column] == subString[0])  // if first elements are the same
            {
                for (unsigned int inputIndex = 1; inputIndex < strlen(subString); inputIndex++)
                {
                    if (text[row][column + inputIndex] != subString[inputIndex])
                        ifPresent = false;
                }
                if (ifPresent)
                    printf("%d|%d; ", row, column);
            }
        }
    }
    printf("\n");
}

int TextEditor::Buffer::DeleteAtCursorPos(unsigned int amountOfCharsToDelete, bool ifSaveToBuffer)
{
    size_t row = curCursor->GetRow();
    size_t column = curCursor->GetColumn();
    char*  addBuffer = nullptr;

    if (column + amountOfCharsToDelete >= defaultRowLength)
    {
        printf("\nError: deletion amount exceeds row bounds");
        return -1;
    }
    try
    {
        addBuffer = new char[defaultRowLength];
    }
    catch (const std::bad_alloc&)
    {
        AllocFailureProgTermination(nullptr, this);
    }
    addBuffer[0] = '\0';

    if (column + amountOfCharsToDelete < strlen(text[row]))  // cut out the deleted symbols
    {
        for (size_t columnIndex = column + amountOfCharsToDelete; columnIndex < strlen(text[row]); columnIndex++)
        {
            char curChar = text[row][columnIndex];
            strncat_s(addBuffer, defaultRowLength, &curChar, 1);
        }
        if (ifSaveToBuffer)  // if it is cut mode
        {
            pasteBuffer[0] = '\0';
            for (size_t columnIndex = column; columnIndex < column + amountOfCharsToDelete; columnIndex++)
            {
                char curChar = text[row][columnIndex];
                strncat_s(pasteBuffer, defaultRowLength, &curChar, 1);
            }
        }
        text[row][column] = '\0';
        strcat_s(text[row], defaultRowLength, addBuffer);
    }
    else  // just put a termination '\0' 
    {
        if (ifSaveToBuffer)
        {
            pasteBuffer[0] = '\0';
            for (size_t columnIndex = column; columnIndex < strlen(text[row]); columnIndex++)
            {
                char curChar = text[row][columnIndex];
                strncat_s(pasteBuffer, defaultRowLength, &curChar, 1);
            }
        }
        text[row][column] = '\0';
    }

    SetCursorPosition(row, column);

    return 0;
}

int TextEditor::Buffer::CopyAtCursorPos(unsigned int amountOfCharsToCopy)
{
    size_t row = curCursor->GetRow();
    size_t column = curCursor->GetColumn();

    if (column + amountOfCharsToCopy >= defaultRowLength)
    {
        printf("\nError: copying amount exceeds row bounds");
        return -1;
    }
    pasteBuffer[0] = '\0';
    if (column + amountOfCharsToCopy < strlen(text[row]))
    {
        for (size_t columnIndex = column; columnIndex < column + amountOfCharsToCopy; columnIndex++)
        {
            char curChar = text[row][columnIndex];
            strncat_s(pasteBuffer, defaultRowLength, &curChar, 1);  // check edge case
        }
    }
    else
    {
        for (size_t columnIndex = column; columnIndex < strlen(text[row]); columnIndex++)
        {
            char curChar = text[row][columnIndex];
            strncat_s(pasteBuffer, defaultRowLength, &curChar, 1);  // check edge case
        }
    }

    return 0;
}

int TextEditor::Buffer::PasteAtCursorPos()
{
    return this->InsertAtCursorPos(pasteBuffer);
}

int TextEditor::Buffer::MoveCursorToEnd()
{
    if (text[totalRowCounter] == nullptr)
    {
        return -1;
    }
    SetCursorPosition(totalRowCounter, strlen(text[totalRowCounter]));

    return 0;
}

void TextEditor::Buffer::FlushText()
{
    for (int rowIndex = 1; rowIndex <= totalRowCounter; rowIndex++)
    {
        delete[] text[rowIndex];
        text[rowIndex] = nullptr;
    }
    text[0][0] = '\0';
    SetCursorPosition(0, 0);
    totalRowCounter = 0;
}

int TextEditor::Buffer::SetCursorPosition(size_t row, size_t column)
{
    if (row    > totalRowCounter || 
        column > strlen(text[row]))
    {
        return -1;
    }

    curCursor->SetRow(row);
    curCursor->SetColumn(column);
    return 0;
}

size_t TextEditor::Buffer::GetCurRowRemainLength()  // returns remaining size of the current row
{
    size_t remainLength = defaultRowLength;  // by default
    size_t curRow = curCursor->GetRow();
    size_t curColumn = curCursor->GetColumn();

    if (text[curRow] != nullptr)
        remainLength = remainLength - strlen(text[curRow]) - 1;  // -1 to keep '\0'

    if (remainLength == 0 || remainLength > defaultRowLength)  // to prevent overflow of size_t type
        return 0;

    return remainLength;
}

int TextEditor::Buffer::GetTxtSize(FILE* filePtr)
{
    if (filePtr == nullptr)
        return -1;

    fseek(filePtr, 0, SEEK_END); // Move the file pointer to the end of the file
    int fileSize = ftell(filePtr); // Get the current file pointer position
    rewind(filePtr); // Reset the file pointer to the beginning of the file

    return fileSize;
}

size_t TextEditor::Buffer::GetRowSize() const
{
    return defaultRowLength;
}

TextEditor::Buffer::Cursor TextEditor::Buffer::GetCurCursor()
{
    return *curCursor;  // a copy
}

void TextEditor::Buffer::CloseFile(FILE* filePtr)
{
    if (filePtr != nullptr)
        fclose(filePtr);

    filePtr = nullptr;
}
