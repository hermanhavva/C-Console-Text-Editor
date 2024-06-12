
#ifndef _AUXILIARY_FUNCTIONS_
#define _AUXILIARY_FUNCTIONS_

#include "Buffer class.h"
#include <stdio.h>

void AllocFailureProgTermination(Buffer* buffer, FILE* filePtr);
void CloseFile(FILE* filePtr);
int  RemoveEndNewLine(char*);
bool IsInputSizeValid(char* input, int inputSize);

#endif // !_AUXILIARY_FUNCTIONS_

