#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "common.h"
#include "Buffer.h"


Buffer::Buffer(const size_t defaultRowNum, const size_t defaultRowLength)  // constructor
{
    if (defaultRowNum < 1 || defaultRowLength < 1)
    {
        throw std::runtime_error("(Exception at buffer constructor) Number of rows or row length is too small(must be >0)\n");
    }
    if (defaultRowLength > INT_MAX - 100 || defaultRowNum > INT_MAX - 100)
    {
        throw std::runtime_error("(Exception at buffer constructor) Number of rows and/or columns is too big\n");
    }

    curCursor = new Cursor(0, 0);
    this->defaultRowNum = defaultRowNum;
    this->defaultRowLength = defaultRowLength;
    InitializeBuffer();
}

Buffer::~Buffer()  // destructor 
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

Buffer::Cursor::Cursor(size_t row, size_t column)
{
    this->row = row;
    this->column = column;
}

void Buffer::Cursor::SetRow(size_t row)
{
    this->row = row;
}

void Buffer::Cursor::SetColumn(size_t column)
{
    this->column = column;
}

size_t Buffer::Cursor::GetRow() const
{
    return row;
}

size_t Buffer::Cursor::GetColumn() const
{
    return column;
}

int Buffer::InitializeBuffer()
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

int Buffer::Append(char* input) // update cursor with the values of the end of the buffer 
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

int Buffer::AddRow()
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

int Buffer::SaveToFile(FILE* filePtr)
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

int Buffer::LoadFromFile(FILE* filePtr)
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

void Buffer::PrintCurrent()
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

int Buffer::InsertAtCursorPos(char* input)
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

int Buffer::InsertReplaceAtCursorPos(char* input)
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

void Buffer::SearchSubstrPos(char* subString)
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

int Buffer::DeleteAtCursorPos(unsigned int amountOfCharsToDelete, bool ifSaveToBuffer)
{
    size_t row = curCursor->GetRow();
    size_t column = curCursor->GetColumn();
    char* addBuffer = nullptr;

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

int Buffer::CopyAtCursorPos(unsigned int amountOfCharsToCopy)
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

int Buffer::PasteAtCursorPos()
{
    return this->InsertAtCursorPos(pasteBuffer);
}

int Buffer::MoveCursorToEnd()
{
    if (text[totalRowCounter] == nullptr)
    {
        return -1;
    }
    SetCursorPosition(totalRowCounter, strlen(text[totalRowCounter]));

    return 0;
}

void Buffer::FlushText()
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

int Buffer::SetCursorPosition(size_t row, size_t column)
{
    if (row > totalRowCounter ||
        column > strlen(text[row]))
    {
        return -1;
    }

    curCursor->SetRow(row);
    curCursor->SetColumn(column);
    return 0;
}

size_t Buffer::GetCurRowRemainLength()  // returns remaining size of the current row
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

int Buffer::GetTxtSize(FILE* filePtr)
{
    if (filePtr == nullptr)
        return -1;

    fseek(filePtr, 0, SEEK_END); // Move the file pointer to the end of the file
    int fileSize = ftell(filePtr); // Get the current file pointer position
    rewind(filePtr); // Reset the file pointer to the beginning of the file

    return fileSize;
}

size_t Buffer::GetRowSize() const
{
    return defaultRowLength;
}

Buffer::Cursor Buffer::GetCurCursor()
{
    return *curCursor;  // a copy
}

void Buffer::CloseFile(FILE* filePtr)
{
    if (filePtr != nullptr)
        fclose(filePtr);

    filePtr = nullptr;
}
