#pragma once

#ifndef _BUFFER_CLASS_H_
#define _BUFFER_CLASS_H_

#include <stdio.h>

/*

class Buffer
{
public:
    class Cursor
    {
    public:
        Cursor(int, int);
        void SetRow(int);
        void SetColumn(int);
        int GetRow() const;
        int GetColumn() const;

    private:
        int row, column;
    };

    Buffer();  // constructor
    ~Buffer();  // destructor 
    int  Append(char*);
    int  AddRow();
    int  SaveToFile(FILE*);
    int  LoadFromFile(FILE*);
    void PrintCurrent();
    int  InsertAtCursorPos(char*);
    int  InsertReplaceAtCursorPos(char*);
    void SearchSubstrPos(char*);
    int  DeleteAtCursorPos(unsigned int, bool);
    int  CopyAtCursorPos(unsigned int);
    int  PasteAtCursorPos();
    int  SetCursorPosition(int, int);
    int  MoveCursorToEnd();
    int  GetCurRowRemainLength();
    void FlushText();  // sets the buffer to initial state
    int  GetRowSize();
    void AllocFailureProgTermination(FILE*);
    void CloseFile(FILE*);
    Buffer::Cursor GetCurCursor();

private:
    const int defaultRowNum = 256;  // will scale this baby up (no)
    const int defaultRowLength = 100;

    int     totalRowCounter = -1;
    Cursor* curCursor = nullptr;
    char** text = nullptr;
    char* pasteBuffer = nullptr;


    int InitializeBuffer();
    int GetTxtSize(FILE*);
};
*/
#endif  // !_BUFFER_CLASS_H_

