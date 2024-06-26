#include "caesar.h"
#include "pch.h"

char* Encrypt(char*, int);

extern "C"
{
    __declspec(dllexport) char* EncryptStr(char* textToEncrypt, int key)
    {
        return Encrypt(textToEncrypt, key);
    }
    __declspec(dllexport) char* DecryptStr(char* textToDecrypt, int key)
    {
        key = -key;
        return Encrypt(textToDecrypt, key);
    }
};
char* Encrypt(char* messageToEncrypt, int offSet)
{
    for (int index = 0; index <= strlen(messageToEncrypt); index++)
    {
        int curChCodeWithOffSet = (int)messageToEncrypt[index] + offSet;


        if ((int)messageToEncrypt[index] >= (int)('A') && (int)messageToEncrypt[index] <= (int)('Z'))  // uppercase letter
        {
            if (curChCodeWithOffSet < (int)('A'))
            {
                int newOffSet = (int)('A') - curChCodeWithOffSet;  // positive
                messageToEncrypt[index] = (int)('Z') + 1 - newOffSet;
            }
            else if (curChCodeWithOffSet > (int)('Z'))
            {
                int newOffSet = curChCodeWithOffSet - (int)('Z');
                messageToEncrypt[index] = (int)('A') - 1 + newOffSet;  // positive
            }
            else
            {
                messageToEncrypt[index] = curChCodeWithOffSet;
            }
        }
        else if ((int)messageToEncrypt[index] >= (int)('a') && (int)messageToEncrypt[index] <= (int)('z'))  // lowercase letter
        {
            if (curChCodeWithOffSet < (int)('a'))
            {
                int newOffSet = (int)('a') - curChCodeWithOffSet;  // positive
                messageToEncrypt[index] = (int)('z') + 1 - newOffSet;
            }
            else if (curChCodeWithOffSet > (int)('z'))
            {
                int newOffSet = curChCodeWithOffSet - (int)('z');
                messageToEncrypt[index] = (int)('a') - 1 + newOffSet;  // positive
            }
            else
            {
                messageToEncrypt[index] = curChCodeWithOffSet;
            }
        }
    }

    return messageToEncrypt;
}
