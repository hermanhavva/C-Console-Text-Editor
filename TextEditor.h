#ifndef _TEXT_EDITOR_CLI_H_
#define _TEXT_EDITOR_CLI_H_

#include <stdio.h>
#include <stdlib.h>
#include "CaesarCipher.h"
#include "Buffer.h"
#include "string"


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
    int HandleCipherTxtAction(char* input, size_t inputSize);
    int HandlePaste();
    void SetDllPath(std::string);
    std::string GetDllPath();

    std::string pathToDll = "";
    bool ifLibLoaded = false;
    CaesarCipher* caesarCipher = nullptr;
    Buffer* buffer = nullptr;
    FILE* filePtr = nullptr;
};


#endif // !_TEXT_EDITOR_CLI_H_
