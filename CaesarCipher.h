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

	string EncryptTxt(const int key, string, string);
	string DecryptTxt(const int key, string fromPath, string outPath);
	string EncryptStr(string message, const int key);
	string DecryptStr(string message, const int key);
	size_t GetBufferCapacity() const;

private:
	char* GetNextChunkFromTxt(string&, string);
	char* GetNextChunkFromStr(string&);
	bool  isPrintable(char) const;
	bool  ifLenValid(string message) const; 
	bool  ifKeyValid(const int key) const;

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
	const size_t BUFFERCAPACITY = 10;
};


#endif // !_CAESAR_CIPHER_H


