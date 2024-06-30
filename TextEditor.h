
#ifndef _TEXT_EDITOR_CLI_H_
#define _TEXT_EDITOR_CLI_H_
#include <stdio.h>
#include <stdlib.h>
#include "CaesarCipher.h"
#include "Buffer.h"


class TextEditor
{
public:	

	TextEditor(const size_t rowNum, const size_t rowLength, const char* pathToDll);
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
