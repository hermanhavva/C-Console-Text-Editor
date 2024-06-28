#pragma once
#ifndef _TEXT_EDITOR_CLI_H_
#define _TEXT_EDITOR_CLI_H_
#include <stdio.h>
#include <stdlib.h>


class TextEditor
{
public:
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

        Buffer(const int, const int);  // constructor
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
        Buffer::Cursor GetCurCursor();
        void CloseFile(FILE*);

    private:
        int defaultRowNum = 0;  
        int defaultRowLength = 0;

        int     totalRowCounter = -1;
        Cursor* curCursor = nullptr;
        char**  text = nullptr;
        char*   pasteBuffer = nullptr;

        int InitializeBuffer();
        int GetTxtSize(FILE*);
    };
	
	TextEditor(const int rowNum, const int rowLength);
	~TextEditor();
    void ExecuteCommand(enum Mode command, bool* ifContinue);
    void PrintMainMenu() const;
    void CloseFile(FILE*);

private:
    void HandleUserExit(char* input);
    int HandleAppend(char* input, int inputSize);
    int HandleNewLine();
    int HandleSaveToFile(char* input, int inputSize);
    int HandleLoadFromFile(char* input, int inputSize);
    int HandleInsert(char* input, int inputSize);
    int HandleInsertReplace(char* input, int inputSize);
    int HandleSetCursor();
    int HandleSearch(char* input, int inputSize);
    int HandleDelete();
    int HandleCut();
    int HandleCopy();

    int HandlePaste();
    
    

	Buffer* buffer = nullptr;
    FILE* filePtr = nullptr;
    // common functions


};


#endif // !_TEXT_EDITOR_CLI_H_
