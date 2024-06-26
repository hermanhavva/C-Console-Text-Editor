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
		dllHandle = nullptr;
		throw std::runtime_error(std::string("Failed to find the procedure\n"));
	}

	chunkBuffer = new char[BUFFERCAPACITY + 1];  // +1 for '\0'
	chunkBuffer[0] = '\0';
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
	if (message.length() == 0)
	{
		return "";
	}

	size_t messageSize = message.length() - 1;  // counting from 0
	string encryptedMessage = "";

	// chunkBuffer[BUFFERCAPACITY] = '\0'
	if (messageSize >= BUFFERCAPACITY)  // chunk logic: -1 is for counting from 0     
	{
		const int chunkAmount = (int)ceil((messageSize + 1) / (BUFFERCAPACITY + 1 - 1));  // rounding and casting

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

string CaesarCipher::Decrypt(string message, const int key)
{
	if (key > 26 || key < -26) {
		throw std::runtime_error(std::string("The key is too big\n"));
	}
	if (message.length() == 0)
	{
		return "";
	}

	size_t messageSize = message.length() - 1;  // counting from 0
	string decryptedMessage = "";

	if (messageSize >= BUFFERCAPACITY)  // chunk logic: -1 is for counting from 0     
	{
		const int chunkAmount = (int)ceil((messageSize + 1) / (BUFFERCAPACITY + 1 - 1));  // rounding and casting

		for (int i = 0; i < chunkAmount; i++)
		{
			decryptedMessage += string(encryptProcAddress(GetNextChunk(message), key));
		}
	}

	else
	{
		strcpy_s(chunkBuffer, BUFFERCAPACITY, message.c_str());
		decryptProcAddress(chunkBuffer, key);
		decryptedMessage = string(chunkBuffer);
	}

	return decryptedMessage;

}

char* CaesarCipher::GetNextChunk(string& message)
{
	size_t messageSize = message.length() - 1;  // -1 for counting from 0 

	for (size_t index = 0; index <= BUFFERCAPACITY - 1 && index <= messageSize; index++)
	{
		chunkBuffer[index] = message[index];
	}
	if (messageSize >= BUFFERCAPACITY)
	{
		chunkBuffer[BUFFERCAPACITY] = '\0';
		message.erase(0, BUFFERCAPACITY + 1);  // +1 because .erase counts from 1 
	}
	else
	{
		chunkBuffer[messageSize] = '\0';
		message.erase(0, messageSize + 1);  // here it becomes an empty string
	}
	return chunkBuffer;
}

size_t CaesarCipher::GetBufferCapacity() const
{
	return BUFFERCAPACITY;
}