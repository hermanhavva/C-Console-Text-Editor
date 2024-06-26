#pragma once
extern "C"
{
	char* EncryptStr(char* textToEncrypt, int key);
	char* DecryptStr(char* textToDecrypt, int key);
};