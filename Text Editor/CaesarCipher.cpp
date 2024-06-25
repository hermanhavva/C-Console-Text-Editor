#include "CaesarCipher.h"
#include <string>
#include <windows.h>
#include <stdexcept>
#include <cmath>

CaesarCipher::CaesarCipher(string pathToDll)
{
	// Initializing an object of wstring
	wstring temp = wstring(pathToDll.begin(), pathToDll.end());
	LPCWSTR wideStringPath = temp.c_str();
	
	dllHandle = LoadLibrary(wideStringPath);
	if (dllHandle == nullptr || dllHandle == INVALID_HANDLE_VALUE)
	{
		throw std::runtime_error(std::string("Failed to load the DLL\n"));
	}

	encryptProcAddress = (encrypt_ptr_t)GetProcAddress(dllHandle, encryptProcName);
	decryptProcAddress = (decrypt_ptr_t)GetProcAddress(dllHandle, decryptProcName);

	if (decryptProcAddress == nullptr || encryptProcAddress == nullptr)
	{
		FreeLibrary(dllHandle);
		throw std::runtime_error(std::string("Failed to find the procedure\n"));
	}
	
	chunkBuffer = new char[BUFFERCAPACITY];
}
CaesarCipher::~CaesarCipher()
{
	FreeLibrary(dllHandle);
	delete[] chunkBuffer;
}

string CaesarCipher::Encrypt(string message, const int key)
{
	if (key > 26 || key < -26) {
		throw std::runtime_error(std::string("The key is too big\n"));
	}
	
	size_t messageLength = message.length();
	string encryptedMessage = "";

	if (messageLength >= BUFFERCAPACITY - 1)  // chunk logic     -1 for '\0'
	{
		const int chunkAmount = (int)ceil(messageLength / (BUFFERCAPACITY - 2));  // rounding and casting
		
		for (int i = 0; i < chunkAmount; i++)
		{
			encryptedMessage += string(encryptProcAddress(GetNextChunk(message), key));
		}
	}

	else 
	{
		strcpy_s(chunkBuffer, BUFFERCAPACITY, message.c_str());
		encryptProcAddress(chunkBuffer, key);
		encryptedMessage = string(chunkBuffer);
	}

	return encryptedMessage;
}

char* CaesarCipher::GetNextChunk(string message)
{
	size_t messageLength = message.length();

	for (size_t index = 0; index < BUFFERCAPACITY - 1 && index <= messageLenght; index++)
	{
		chunkBuffer[index] = message[index];
	}
	if (messageLength )
}

string CaesarCipher::Decrypt(string message, const int key)
{
	if (key > 26 || key < -26) 
	{
		throw std::runtime_error(std::string("The key is too big\n"));
	}
}

size_t CaesarCipher::GetBufferCapacity() const
{
	return BUFFERCAPACITY;
}