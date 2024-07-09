#include "CaesarCipher.h"
#include <string>
#include <windows.h>
#include <iostream>
#include <stdexcept>
#include <fstream>

CaesarCipher::CaesarCipher(const std::string pathToDll)
{
    // Initializing an object of wstring
    std::wstring temp = std::wstring(pathToDll.begin(), pathToDll.end());
    LPCWSTR wideStringPath = temp.c_str();

    dllHandle = LoadLibrary(wideStringPath);
    if (dllHandle == nullptr || dllHandle == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error("(Exception at CaesarCipher constructor)Failed to load the DLL\n");
    }

    encryptProcAddress = (encrypt_ptr_t)GetProcAddress(dllHandle, encryptProcName);
    decryptProcAddress = (decrypt_ptr_t)GetProcAddress(dllHandle, decryptProcName);

    if (decryptProcAddress == nullptr || encryptProcAddress == nullptr)
    {
        FreeLibrary(dllHandle);
        dllHandle = nullptr;
        throw std::runtime_error("(Exception at CaesarCipher constructor)Failed to find the procedure\n");
    }

    chunkBuffer = new char[BUFFERCAPACITY + 1];  // +1 for '\0'
    chunkBuffer[0] = '\0';
}


CaesarCipher::~CaesarCipher()
{
	FreeLibrary(dllHandle);
	delete[] chunkBuffer;
}

int CaesarCipher::EncryptTxt(const int key, std::string fromPath, std::string toPath)
{
	if (!ifKeyValid(key)) 
	{
		throw std::runtime_error(std::string("The key is too large must be -26<key<26\n"));
	}
	if (fromPath == toPath)
	{
		throw std::runtime_error(std::string("The From path and Out path maust be different\n"));
	}

	std::ifstream fileFrom;
	std::fstream fileTo;

	fileFrom.open(fromPath, std::ios::binary);
	
	if(!fileFrom.is_open())
	{
		printf("Could not open the file\n");
		return -1;
	}
	
	fileTo.open(toPath, std::ios::out | std::ios::trunc);

	if (!fileTo.is_open())
	{
		printf("Could not open the file\n");
		return -1;
	}

	char curCh;
	size_t counter = 0;
	std::string encryptedMessage = "";

	while (fileFrom.get(curCh))
	{
		if (isPrintable(curCh))
		{
			chunkBuffer[counter] = curCh;
			counter++;
			
			if (counter == BUFFERCAPACITY)
			{
				chunkBuffer[BUFFERCAPACITY] = '\0';
				encryptedMessage += std::string(encryptProcAddress(chunkBuffer, key));
				counter = 0;
			}
		}
	}
	if (counter > 0)  // we need to run encrypt again
	{
		chunkBuffer[counter] = '\0';
		encryptedMessage += std::string(encryptProcAddress(chunkBuffer, key));
	}

	fileTo << encryptedMessage;
	fileTo.close();
	fileFrom.close();

	return 0;
}

int CaesarCipher::DecryptTxt(const int key, std::string fromPath, std::string toPath)
{
	if (!ifKeyValid(key))
	{
		throw std::runtime_error(std::string("The key is too large must be -26<key<26\n"));
	}
	if (fromPath == toPath)
	{
		throw std::runtime_error(std::string("The From path and Out path maust be different\n"));
	}

	std::ifstream fileFrom;
	std::fstream fileTo;

	fileFrom.open(fromPath, std::ios::binary);

	if (!fileFrom.is_open())
	{
		printf("Could not open the file\n");
		return -1;
	}

	fileTo.open(toPath, std::ios::out | std::ios::trunc);

	if (!fileTo.is_open())
	{
		printf("Could not open the file\n");
		return -1;
	}

	char curCh;
	size_t counter = 0;
	std::string decryptedMessage = "";

	while (fileFrom.get(curCh))
	{
		if (isPrintable(curCh))
		{
			chunkBuffer[counter] = curCh;
			counter++;

			if (counter == BUFFERCAPACITY)
			{
				chunkBuffer[BUFFERCAPACITY] = '\0';
				decryptedMessage += std::string(decryptProcAddress(chunkBuffer, key));
				counter = 0;
			}
		}
	}
	if (counter > 0)  // we need to put to run decrypt again
	{
		chunkBuffer[counter] = '\0';
		decryptedMessage += std::string(decryptProcAddress(chunkBuffer, key));
	}

	fileTo << decryptedMessage;
	fileTo.close();
	fileFrom.close();

	return 0;
}

bool CaesarCipher::isPrintable(char ch) const
{
	return std::isprint(static_cast<unsigned char>(ch)) || ch == '\n';
}

char* CaesarCipher::GetNextChunkFromTxt(std::string& message, std::string pathToTxt)
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

std::string CaesarCipher::EncryptStr(std::string message, const int key)
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
	std::string encryptedMessage = "";

	// chunkBuffer[BUFFERCAPACITY] = '\0'
	if (messageSize >= BUFFERCAPACITY)  // chunk logic: -1 is for counting from 0     
	{
		const int chunkAmount = static_cast<int>(ceil(static_cast<double>(messageSize + 1) / BUFFERCAPACITY + 1 - 1));

		for (int i = 0; i < chunkAmount; i++)
		{
			encryptedMessage += std::string(encryptProcAddress(GetNextChunkFromStr(message), key));
		}
	}

	else
	{
		strcpy_s(chunkBuffer, BUFFERCAPACITY, message.c_str());
		encryptProcAddress(chunkBuffer, key);
		encryptedMessage = std::string(chunkBuffer);
	}

	return encryptedMessage;
}

std::string CaesarCipher::DecryptStr(std::string message, const int key)
{
	if (!ifKeyValid(key)) {
		throw std::runtime_error(std::string("The key is too large must be -26<key<26\n"));
	}
	if (!ifLenValid(message))
	{
		return "";
	}

	size_t messageSize = message.length() - 1;  // counting from 0
	std::string decryptedMessage = "";

	if (messageSize >= BUFFERCAPACITY)  // chunk logic: -1 is for counting from 0     
	{
		const int chunkAmount = static_cast<int>(ceil(static_cast<double>(messageSize + 1) / BUFFERCAPACITY + 1 - 1));

		for (int i = 0; i < chunkAmount; i++)
		{
			decryptedMessage += std::string(decryptProcAddress(GetNextChunkFromStr(message), key));
		}
	}
	else
	{
		strcpy_s(chunkBuffer, BUFFERCAPACITY, message.c_str());
		decryptProcAddress(chunkBuffer, key);
		decryptedMessage = std::string(chunkBuffer);
	}

	return decryptedMessage;
}

char* CaesarCipher::GetNextChunkFromStr(std::string& message)
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

bool CaesarCipher::ifLenValid(std::string message) const
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