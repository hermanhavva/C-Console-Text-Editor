#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "Cursor Class.cpp"



class Buffer
{
public:
    Buffer()  // constructor
    {
        curCursor = new Cursor(-1, 0);
        InitializeBuffer();
        AddRow();
    }

    ~Buffer()  // destructor 
    {
        delete curCursor;
        curCursor = nullptr;

        for (int rowIndex = 0; rowIndex <= bufferRowCounter; rowIndex++)
        {
            free(text[rowIndex]);
            printf("hello destructor");
        }

        free(text);
        text = nullptr;
    }

private:
    const int defaultRowNum = 256;
    const int defaultRowLength = 200000056;
    int bufferRowCounter = -1;
    Cursor* curCursor = nullptr;
    char** text = nullptr;

    int InitializeBuffer()
    {
        text = (char**)calloc(defaultRowNum, sizeof(char*));

        if (text == nullptr) {
            perror("Failed to allocate memory\n");
            return -1;
        }

        return 0;
    }

    int AddRow()
    {
        if (text == nullptr)
        {
            return -1;
        }

        int curRow = curCursor->GetRow();
        int curColumn = curCursor->GetColumn();

        if (curRow >= defaultRowNum - 1)
        {
            printf("The buffer is too small\n");
            return -1;
        }

        text[curRow + 1] = (char*)malloc(sizeof(char) * defaultRowLength);

        if (text[curRow + 1] == nullptr) {
            perror("Failed to allocate row\n");
            return -1;
        }

        text[curRow + 1][0] = '\0';

        curCursor->SetRow(curRow + 1);
        curCursor->SetColumn(0);
        bufferRowCounter++;

        return 0;
    }
};

/*
class Row
{


};
*/
