#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "Cursor Class.cpp"


class Buffer;
void AllocFailureProgTermination(Buffer*, FILE*);
void CloseFile(FILE* filePtr);

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
    int  InsertAtCursorPos(char*);
    void SearchSubstrPos(char*);
    int  SetCursorPosition(int, int);
    int  MoveCursorToEnd();
    int  GetCurRowRemainLength(); 
    void FlushText();  // sets the buffer to initial state
    int GetRowSize();
    Cursor GetCurCursor();

private:
    const int defaultRowNum = 256;  // will scale this baby up (no)
    const int defaultRowLength = 50;
   
    int     totalRowCounter = -1;  
    Cursor* curCursor = nullptr;
    char**  text = nullptr;

    int InitializeBuffer();    
    int GetTxtSize(FILE*);


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

    int curRow = curCursor->GetRow();
    int curColumn = curCursor->GetColumn();
   
    if (curRow          >= defaultRowNum - 1 || 
        curRow          <  0                 ||
        totalRowCounter >= defaultRowNum - 1 || 
        curColumn       <  0                 || 
        curColumn       >= defaultRowLength)
    {
        printf("\nThe buffer is too small");  
        return -1;
    }

    char* buffer = nullptr;
    try
    {
        buffer = new char[defaultRowLength];
        buffer[0] = '\0';
    }
    catch (const std::exception&)
    {
        AllocFailureProgTermination(this, nullptr);
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
            delete[] buffer;
            AllocFailureProgTermination(this, nullptr);
        }
        text[totalRowCounter][0] = '\0';

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
        }
        catch (const std::bad_alloc&)
        {
            delete[] buffer;
            AllocFailureProgTermination(this, nullptr);
        }
        text[curRow + 1][0] = '\0';
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
    char newLineCh = '\n';
    
    if (filePtr == nullptr)
    {
        return -1;
    }
    int textSize = sizeof(char) * defaultRowLength * defaultRowNum + defaultRowNum;  // +defaultRowNum is for '\n'
    char* textToSave = nullptr;
    try
    {
        textToSave = new char[textSize];
    }
    catch (const std::bad_alloc&)
    {
        AllocFailureProgTermination(this, filePtr);
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
    if (filePtr == nullptr || 
        filePtr == NULL)
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
        AllocFailureProgTermination(this, filePtr);
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

    for (int row = 0; row <= totalRowCounter; row++)
        printf("%s\n", text[row]);

    printf("\n");
}

int Buffer::InsertAtCursorPos(char* input)
{
    int row    = curCursor->GetRow();
    int column = curCursor->GetColumn();


    if (row > totalRowCounter || column >= defaultRowLength)   // this might be unnecessary
    {
        printf(">>Not enough space (might use newline first)\n");
        return -1;
    }

    int rowTextLength = strlen(text[row]);  // on this index there is '\0'
    int insertTextLength = strlen(input);
    int curRowMaxSize = defaultRowLength;  

    if ((insertTextLength + rowTextLength ) - curRowMaxSize >= 0)
    {   // the logic can handle +30 expansion, but not more
        printf(">>Not enough space (might use newline first)\n");
        return -1;
    }
    /*
    else if ((insertTextLength + rowTextLength + 2) - curRowMaxSize >= offset)
    {
        printf(">>The row is full or message too big to insert\n");
        return -1;
    }*/

    if ((rowTextLength + 1) < column)  // add spaces
    {

        for (int colIndex = rowTextLength; colIndex < column - 1; colIndex++)
            text[row][colIndex] = ' ';

        text[row][column - 1] = '\0';
        strcat_s(text[row], curRowMaxSize, input);

        /*if (row > bufferRowCounter)  // so there is '\n' and we need to transfer it to the end
        {
            int curLength = strlen(buffer[row]);
            buffer[row][rowTextLength - 1] = ' ';
            buffer[row][curLength] = '\n';
            buffer[row][curLength + 1] = '\0';
        }*/
    }
    else if ((rowTextLength /*/ + 1*/) > column)
    {
        char* addBuffer = new char[curRowMaxSize ];
        addBuffer[0] = '\0';
        char ch = '0';
        for (int colIndex = column; colIndex < rowTextLength; colIndex++)
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
        strcat_s(text[row], curRowMaxSize - 1, input);
    }

    return 0;
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
/*
int Buffer::InsertAtCursorPos(char* strToInsert) 
{
    int row, column; 
    

    if (row > totalRowCounter || column > ROWSIZE) {
        printf(">>Row/Column index too big (might use newline first)\n");
        return -1;
    }
    int rowTextLength = strlen(buffer[row]);  // on this index there is '\0'
    int insertTextLength = strlen(input);
    int curRowMaxSize = ROWSIZE;  // size is dynamic 

    if ((insertTextLength + rowTextLength + 2) - curRowMaxSize > 0 && (insertTextLength + rowTextLength + 2) - curRowMaxSize < offset)
    {   // the logic can handle +30 expansion, but not more
        buffer[row] = (char*)realloc(buffer[row], sizeof(char) * (curRowMaxSize + offset));

        if (buffer[row] == NULL)
            AllocFailureProgTermination();

        curRowMaxSize += offset;
    }
    else if ((insertTextLength + rowTextLength + 2) - curRowMaxSize >= offset)
    {
        printf(">>The row is full or message too big to insert\n");
        return -1;
    }

    if ((rowTextLength + 1) < column)  // add spaces
    {

        for (int colIndex = rowTextLength; colIndex < column - 1; colIndex++)
            buffer[row][colIndex] = ' ';

        buffer[row][column - 1] = '\0';
        strcat_s(buffer[row], curRowMaxSize - 1, input);

        if (row > totalRowCounter)  // so there is '\n' and we need to transfer it to the end
        {
            int curLength = strlen(buffer[row]);
            buffer[row][rowTextLength - 1] = ' ';
            buffer[row][curLength] = '\n';
            buffer[row][curLength + 1] = '\0';
        }
    }
    else if ((rowTextLength + 1) > column)
    {
        char* addBuffer = (char*)malloc(sizeof(char) * curRowMaxSize - 1);
        addBuffer[0] = '\0';
        char ch = '0';
        for (int colIndex = column; colIndex < rowTextLength; colIndex++)
        {
            ch = buffer[row][colIndex];
            strncat_s(addBuffer, curRowMaxSize - 1, &ch, 1);  // one symbol at a time

        }
        buffer[row][column] = '\0';

        strcat_s(buffer[row], curRowMaxSize - 1, input);
        strcat_s(buffer[row], curRowMaxSize - 1, addBuffer);

        free(addBuffer);
    }
    else  // only strcat
    {
        strcat_s(buffer[row], curRowMaxSize - 1, input);
    }
    printf(">>success\n");

    free(input);
    return 0;
}*/ 

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

int Buffer::SetCursorPosition(int row, int column)
{
    if (row < 0 || row > totalRowCounter || column < 0 || column > strlen(text[row]))
    {
        return -1;
    }
    
    curCursor->SetRow(row);
    curCursor->SetColumn(column);
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

int Buffer::GetTxtSize(FILE* filePtr)
{
    if (filePtr == NULL ||
        filePtr == nullptr)
        return -1;

    fseek(filePtr, 0, SEEK_END); // Move the file pointer to the end of the file
    int fileSize = ftell(filePtr); // Get the current file pointer position
    rewind(filePtr); // Reset the file pointer to the beginning of the file

    return fileSize;
}

int Buffer::GetRowSize()
{
    return defaultRowLength;
}
Cursor Buffer::GetCurCursor()
{
    return *curCursor;
}

void CloseFile(FILE* filePtr)
{
    if (filePtr != NULL &&
        filePtr != nullptr)
        fclose(filePtr);

    filePtr = nullptr;
}

void AllocFailureProgTermination(Buffer* buffer, FILE* filePtr)  //     BETTER KEEP THIS AND CloseFile() OUT OF THE CLASS
{
    perror("Error allocating memory");
    CloseFile(filePtr);
    delete buffer;

    Sleep(1000);
    exit(EXIT_FAILURE);
}

