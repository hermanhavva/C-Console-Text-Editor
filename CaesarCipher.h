#ifndef _CAESAR_CIPHER_H_
#define _CAESAR_CIPHER_H_

#include <windows.h>
#include <string>

class CaesarCipher
{
public:
	CaesarCipher(std::string pathToDll);
	~CaesarCipher();

	int EncryptTxt(const int key, std::string, std::string);
	int DecryptTxt(const int key, std::string fromPath, std::string outPath);
	std::string EncryptStr(std::string message, const int key);
	std::string DecryptStr(std::string message, const int key);
	size_t GetBufferCapacity() const;
	bool  ifKeyValid(const int key) const;

private:
	char* GetNextChunkFromTxt(std::string&, std::string);
	char* GetNextChunkFromStr(std::string&);
	bool  isPrintable(char) const;
	bool  ifLenValid(std::string message) const;

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
	static const size_t BUFFERCAPACITY = 10;
};


#endif // !_CAESAR_CIPHER_H


