
#ifndef _TEXT_EDITOR_CLI_H_
#define _TEXT_EDITOR_CLI_H_
#include <stdio.h>
#include <stdlib.h>
#include "CaesarCipher.h"


class TextEditor
{
public:
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
        char**  text = nullptr;
        char*   pasteBuffer = nullptr;

        int InitializeBuffer();
        int GetTxtSize(FILE*);
    };
	
    


	TextEditor(const int rowNum, const int rowLength, const char* pathToDll);
	~TextEditor();
    void ExecuteCommand(enum Mode command, bool* ifContinue);
    void PrintMainMenu() const;
    void CloseFile(FILE*);

private:
    void HandleUserExit(char* input);
    int HandleAppend(char* input, size_t inputSize);
    int HandleNewLine();
    int HandleSaveToFile(char* input, size_t inputSize);
    int HandleLoadFromFile(char* input, size_t inputSize);
    int HandleInsert(char* input, size_t inputSize);
    int HandleInsertReplace(char* input, size_t inputSize);
    int HandleSetCursor();
    int HandleSearch(char* input, size_t inputSize);
    int HandleDelete();
    int HandleCut();
    int HandleCopy();
    int HandleCaesarTxtAction(char* input, size_t inputSize, bool ifEncrypt);
    

    int HandlePaste();
    
    CaesarCipher* caesarCipher = nullptr;
    Buffer* buffer = nullptr;
    
    FILE* filePtr = nullptr;
    // common functions


};


#endif // !_TEXT_EDITOR_CLI_H_
