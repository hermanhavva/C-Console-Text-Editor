#pragma once

#ifndef _BUFFER_CLASS_H_
#define _BUFFER_CLASS_H_

#include <stdio.h>

class Buffer
{
public:
    class Cursor
    {
    public:
        Cursor(size_t, size_t);
        void SetRow(size_t);
        void SetColumn(size_t);
        size_t GetRow() const;
        size_t GetColumn() const;

    private:
        size_t row, column;
    };

    Buffer(const size_t, const size_t);  // constructor
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
    int  SetCursorPosition(size_t, size_t);
    int  MoveCursorToEnd();
    size_t GetCurRowRemainLength();
    void FlushText();  // sets the buffer to initial state
    size_t  GetRowSize() const;
    Buffer::Cursor GetCurCursor();
    void CloseFile(FILE*);

private:
    size_t defaultRowNum = 0;
    size_t defaultRowLength = 0;

    size_t  totalRowCounter = 0;
    Cursor* curCursor = nullptr;
    char** text = nullptr;
    char* pasteBuffer = nullptr;

    int InitializeBuffer();
    int GetTxtSize(FILE*);
};

#endif  // !_BUFFER_CLASS_H_

