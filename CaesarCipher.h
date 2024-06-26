#pragma once
#ifndef _CAESAR_CIPHER_H
#define _CAESAR_CIPHER_H_

#include <windows.h>
#include <string>

using namespace std;

class CaesarCipher
{
public:
	CaesarCipher(string pathToDll);
	~CaesarCipher();

	string Encrypt(string message, const int key);
	string Decrypt(string message, const int key);
	size_t GetBufferCapacity() const;

private:
	char* GetNextChunk(string&);
	// dll function pointers
	typedef char* (*encrypt_ptr_t)(char*, const int);
	typedef char* (*decrypt_ptr_t)(char*, int);
	// Const dll function naming
	const char* encryptProcName = "EncryptStr";
	const char* decryptProcName = "DecryptStr";
	// Proc pointers 
	encrypt_ptr_t encryptProcAddress = nullptr;
	decrypt_ptr_t decryptProcAddress = nullptr;

	HINSTANCE dllHandle = nullptr;

	char* chunkBuffer = nullptr;
	const size_t BUFFERCAPACITY = 128;
};


#endif // !_CAESAR_CIPHER_H


