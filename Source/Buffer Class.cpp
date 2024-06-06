#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "Cursor Class.cpp"



class Buffer
{
public:
    Buffer();  // constructor
    ~Buffer();  // destructor 
    int Append(char*);
    int AddRow();
    void PrintCurrent();
    int SetCursorPosition(int, int);
    int MoveCursorToEnd();
    int GetCurRowRemainLength();

private:
    const int defaultRowNum = 256;  // will scale this baby up 
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


int Buffer::AddRow()
{
    if (text == nullptr)
    {
        return -1;
    }

    int curRow = curCursor->GetRow();
    int curColumn = curCursor->GetColumn();
    // got to add some more conditions
    if (curRow >= defaultRowNum - 1 || totalRowCounter >= defaultRowNum - 1 || curColumn < 0 || curColumn >= defaultRowLength)
    {
        printf(">>The buffer is too small\n");
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

void Buffer::PrintCurrent() 
{
    printf("Current text: \n________________________________\n");

    for (int row = 0; row <= totalRowCounter; row++)
        printf("%s\n", text[row]);

    printf("\n");
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
        printf("Wrong column <<failure\n");
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
/*
class Row
{


};
*/
