#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "Cursor Class.cpp"

int GetTxtSize1(FILE*);

class Buffer
{
public:
    Buffer();  // constructor
    ~Buffer();  // destructor 
    int  Append(char*);
    int  AddRow();
    int  SaveToFile(FILE*); 
    int  LoadFromFile(FILE*);
    void PrintCurrent();
    int  InsertAtCursorPos();
    void SearchSubstrPos(char*);
    int  SetCursorPosition(int, int);
    int  MoveCursorToEnd();
    int  GetCurRowRemainLength();
    void FlushText();  // sets the buffer to initial state

private:
    const int defaultRowNum = 256;  // will scale this baby up (no)
    const int defaultRowLength = 150;
   
    int totalRowCounter = -1;  
    Cursor* curCursor = nullptr;
    char** text = nullptr;

    int InitializeBuffer();       
};

Buffer::Buffer()  // constructor
{
    curCursor = new Cursor(-1, 0);
    InitializeBuffer();
}

Buffer::~Buffer()  // destructor 
{
    delete curCursor;
    curCursor = nullptr;

    for (int rowIndex = 0; rowIndex <= totalRowCounter; rowIndex++)
    {
        delete[] text[rowIndex];
    }

    delete[] text;
    text = nullptr;
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
    }
    catch (const std::bad_alloc&)
    {
        printf("Allocation failed");
        return -1;
    }
    totalRowCounter = 0;  

    text[0][0] = '\0';
    SetCursorPosition(0, 0);

    return 0;
}

int Buffer::Append(char* input) // update cursor with the values of the end of the buffer 
{
    int curRow = totalRowCounter;

    if (GetCurRowRemainLength() > (int)strlen(input))
    {
        strcat_s(text[curRow], defaultRowLength, input);
        printf(">>success\n");
    }
    else
    {
        printf(">>Error, buffer too small, start a newline\n");
        return -1;
    }
    MoveCursorToEnd();

    return 0;
}

int Buffer::AddRow()
{
    if (text == nullptr)
    {
        return -1;
    }

    int curRow = curCursor->GetRow();
    int curColumn = curCursor->GetColumn();
   
    if (curRow          >= defaultRowNum - 1 || 
        curRow          <  0                 ||
        totalRowCounter >= defaultRowNum - 1 || 
        curColumn       <  0                 || 
        curColumn       >= defaultRowLength)
    {
        printf(">>The buffer is too small\n");  // may be should put out of class
        return -1;
    }

    char* buffer;
    try
    {
        buffer = new char[defaultRowLength];
        buffer[0] = '\0';
    }
    catch (const std::exception&)
    {
        printf(">>Allocation failed\n");
        return -1;
    }

    if (curRow < totalRowCounter)  // need to do resize
    {
        totalRowCounter++;
        
        try
        {
            text[totalRowCounter] = new char[defaultRowLength];
            text[totalRowCounter][0] = '\0';
        }
        catch (const std::bad_alloc&)
        {
            printf(">>Allocation failed\n");
            totalRowCounter--;
            delete[] buffer;
            return -1;
        }

        for (int rowIndex = totalRowCounter - 1; rowIndex > curRow; rowIndex--) 
        {
            strcpy_s(buffer, defaultRowLength, text[rowIndex]);
            strcpy_s(text[rowIndex + 1], defaultRowLength, buffer);
        }

        text[curRow + 1][0] = '\0';
    }

    else  // resize is not needed
    {
        try
        {
            text[curRow + 1] = new char[defaultRowLength];
            text[curRow + 1][0] = '\0';
        }
        catch (const std::bad_alloc&)
        {
            printf("Allocation failed");
            delete[] buffer;
            return -1;
        }
        totalRowCounter++;
    }

    if (curColumn < strlen(text[curRow]))   // need to move text to another row
    {
        buffer[0] = '\0'; 
        for (int columnIndex = curColumn; columnIndex < strlen(text[curRow]); columnIndex++)
        {
            char curSymbol = text[curRow][columnIndex];
            strncat_s(buffer, defaultRowLength, &curSymbol, 1);   
        }
        text[curRow][curColumn] = '\0';
        strcat_s(text[curRow + 1], defaultRowLength, buffer);

    }
    else if (curColumn == strlen(text[curRow]))  // no need to move text, only move cursor
    {    
        SetCursorPosition(curRow + 1, 0);
    }

    delete[] buffer;
    return 0;
}

int Buffer::SaveToFile(FILE* filePtr) 
{
    if (filePtr != NULL && text[totalRowCounter] != nullptr)
    {
        int textSize = sizeof(char) * defaultRowLength * defaultRowNum;
        char* textToSave = new char[textSize];
        textToSave[0] = '\0';

        for (int row = 0; row <= totalRowCounter; row++)
            strcat_s(textToSave, textSize, text[row]);
        fprintf_s(filePtr, "%s", textToSave);
        delete[] textToSave;
    }
    
    return 0;
}

int Buffer::LoadFromFile(FILE* filePtr)
{
    if (filePtr == NULL)
        return -1;

    if (text != nullptr)  // flushing the buffer if it has been initialized before
        FlushText();
    else
        return -1;

    const int FILESIZE = GetTxtSize1(filePtr);
    char* textFromTxt = new char [FILESIZE + 2];  // add 2 to make sure it is in the bounds
    fread(textFromTxt, sizeof(char), FILESIZE, filePtr);
    textFromTxt[FILESIZE] = '\0';

    // now write to the buffer
    for (int index = 0; index <= FILESIZE; index++)
    {
        char curSymbol = textFromTxt[index];

        // some garbage symbols may appear in textFromTxt so to omit them there is a condition
        if ((int)curSymbol <= 255 && (int)curSymbol > 0) {
            if (curSymbol == '\n')
            {
                if (AddRow() == -1)
                {
                    printf("The file is too long\n");
                    delete[] textFromTxt;
                    return -1;
                }
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
            break;
    }
    delete[] textFromTxt;

    return 0;
}

void Buffer::PrintCurrent() 
{
    printf("Current text: \n________________________________\n");

    for (int row = 0; row <= totalRowCounter; row++)
        printf("%s\n", text[row]);

    printf("\n");
}

void Buffer::SearchSubstrPos(char* subString) 
{
    printf("It can be found on positions(row|column): ");

    for (unsigned int row = 0; row <= totalRowCounter; row++)
    {
        for (unsigned int column = 0; column < strlen(text[row]); column++)
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

int Buffer::InsertAtCursorPos() {
    return 0;
}


int Buffer::SetCursorPosition(int row, int column)
{
    if (row > totalRowCounter || row < 0)
    {
        printf("Wrong row <<failure\n");
        return -1;
    }
    if (column < 0 || column > strlen(text[row]))
    {
        printf("Wrong column <<failure\n");  // got to handle it out of the class 
        return -1;
    }
    
    curCursor->SetRow(row);
    curCursor->SetColumn(column);
    
    return 0;
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
int Buffer::GetCurRowRemainLength()  // returns remaining size of the current row
{
    int remainLength = defaultRowLength;  // by default
    int curRow = curCursor->GetRow();
    int curColumn = curCursor->GetColumn();

    if (text[curRow] != nullptr)
        remainLength = remainLength - strlen(text[curRow]) - 1;  // -1 to keep '\0'

    if (remainLength < 0)
        return 0;

    return remainLength;
}

int GetTxtSize1(FILE* filePtr)
{
    if (filePtr == NULL)
        return -1;
    fseek(filePtr, 0, SEEK_END); // Move the file pointer to the end of the file
    int fileSize = ftell(filePtr); // Get the current file pointer position
    rewind(filePtr); // Reset the file pointer to the beginning of the file

    return fileSize;
}


/*
class Row
{


};
*/
