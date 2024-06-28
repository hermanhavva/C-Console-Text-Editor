#ifndef _AUXILIARY_FUNCTIONS_
#define _AUXILIARY_FUNCTIONS_

//#include "Buffer.h"
#include "TextEditorCLI.h"
//#include <stdio.h>

void AllocFailureProgTermination(FILE* filePtr, TextEditor::Buffer* bufferInstance);
void AllocFailureProgTermination(FILE* filePtr, TextEditor* textEditorInstance);
int  RemoveEndNewLine(char*);
bool IsInputSizeValid(char* input, int inputSize);


#endif // !_AUXILIARY_FUNCTIONS_

