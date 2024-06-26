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

string CaesarCipher::EncryptTxt(string message, const int key, string fromPath, string outPath)
{
	if (!ifKeyValid(key)) {
		throw std::runtime_error(std::string("The key is too large must be -26<key<26\n"));
	}
	if (!ifLenValid(message))
	{
		return "";
	}

	size_t messageSize = message.length() - 1;  // counting from 0
	string encryptedMessage = "";

	// chunkBuffer[BUFFERCAPACITY] = '\0'
	if (messageSize >= BUFFERCAPACITY)  // chunk logic: -1 is for counting from 0     
	{
		const int chunkAmount = static_cast<int>(ceil(static_cast<double>(messageSize + 1) / BUFFERCAPACITY + 1 - 1));

		for (int i = 0; i < chunkAmount; i++)
		{
			encryptedMessage += string(encryptProcAddress(GetNextChunkFromTxt(message, fromPath), key));
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

string CaesarCipher::DecryptTxt(string message, const int key, string fromTxtPath, string outPath)  
{
	if (!ifKeyValid(key)) {
		throw std::runtime_error(std::string("The key is too large must be -26<key<26\n"));
	}
	if (!ifLenValid(message))
	{
		return "";
	}

	size_t messageSize = message.length() - 1;  // counting from 0
	string decryptedMessage = "";

	if (messageSize >= BUFFERCAPACITY)  // chunk logic: -1 is for counting from 0     
	{
		const int chunkAmount = static_cast<int>(ceil(static_cast<double>(messageSize + 1) / BUFFERCAPACITY + 1 - 1));

		for (int i = 0; i < chunkAmount; i++)
		{
			decryptedMessage += string(encryptProcAddress(GetNextChunkFromTxt(message, fromTxtPath), key));
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

char* CaesarCipher::GetNextChunkFromTxt(string& message, string pathToTxt)
{
	size_t messageSize = message.length() - 1;  // -1 for counting from 0 

	for (size_t index = 0; index <= BUFFERCAPACITY - 1 && index <= messageSize; index++)
	{
		chunkBuffer[index] = message[index];
	}
	if (messageSize >= BUFFERCAPACITY)
	{
		chunkBuffer[BUFFERCAPACITY] = '\0';
		message.erase(0, BUFFERCAPACITY );  
	}
	else
	{
		chunkBuffer[messageSize + 1] = '\0';  // +1 for the next element to be '\0'
		message.erase(0, messageSize + 1);  // here it becomes an empty string,  +1 for counting from 1
	}
	return chunkBuffer;
}

string CaesarCipher::EncryptStr(string message, const int key)
{
	if (!ifKeyValid(key)) 
	{
		throw std::runtime_error(std::string("The key is too large must be -26<key<26\n"));
	}
	if (!ifLenValid(message))
	{
		return "";
	}

	size_t messageSize = message.length() - 1;  // counting from 0
	string encryptedMessage = "";

	// chunkBuffer[BUFFERCAPACITY] = '\0'
	if (messageSize >= BUFFERCAPACITY)  // chunk logic: -1 is for counting from 0     
	{
		const int chunkAmount = static_cast<int>(ceil(static_cast<double>(messageSize + 1) / BUFFERCAPACITY + 1 - 1));

		for (int i = 0; i < chunkAmount; i++)
		{
			encryptedMessage += string(encryptProcAddress(GetNextChunkFromStr(message), key));
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

string CaesarCipher::DecryptStr(string message, const int key)
{
	if (!ifKeyValid(key)) {
		throw std::runtime_error(std::string("The key is too large must be -26<key<26\n"));
	}
	if (!ifLenValid(message))
	{
		return "";
	}

	size_t messageSize = message.length() - 1;  // counting from 0
	string decryptedMessage = "";

	if (messageSize >= BUFFERCAPACITY)  // chunk logic: -1 is for counting from 0     
	{
		const int chunkAmount = static_cast<int>(ceil(static_cast<double>(messageSize + 1) / BUFFERCAPACITY + 1 - 1));

		for (int i = 0; i < chunkAmount; i++)
		{
			decryptedMessage += string(encryptProcAddress(GetNextChunkFromStr(message), key));
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

char* CaesarCipher::GetNextChunkFromStr(string& message)
{
	size_t messageSize = message.length() - 1;  // -1 for counting from 0 

	for (size_t index = 0; index <= BUFFERCAPACITY - 1 && index <= messageSize; index++)
	{
		chunkBuffer[index] = message[index];
	}
	if (messageSize >= BUFFERCAPACITY)
	{
		chunkBuffer[BUFFERCAPACITY] = '\0';
		message.erase(0, BUFFERCAPACITY);
	}
	else
	{
		chunkBuffer[messageSize + 1] = '\0';  // +1 for the next element to be '\0'
		message.erase(0, messageSize + 1);  // here it becomes an empty string,  +1 for counting from 1
	}
	return chunkBuffer;
}

bool CaesarCipher::ifLenValid(string message) const
{
	return message.length() != 0;
}

bool CaesarCipher::ifKeyValid(const int key) const
{
	return (key > -26 && key < 26);
}

size_t CaesarCipher::GetBufferCapacity() const
{
	return BUFFERCAPACITY;
}