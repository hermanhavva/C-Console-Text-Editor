#ifndef _AUXILIARY_FUNCTIONS_ 
#define _AUXILIARY_FUNCTIONS_

#include "Buffer.h"
#include "TextEditor.h"


void AllocFailureProgTermination(FILE* filePtr, Buffer* bufferInstance);
//void AllocFailureProgTermination(FILE* filePtr, TextEditor* textEditorInstance);
int  RemoveEndNewLine(char*);
bool IsInputSizeValid(char* input, size_t inputSize);


#endif // !_AUXILIARY_FUNCTIONS_

