# C++ Console-Text-Editor
Console application on C++ which enables writing, editing, loading, reading from txt.  

# 🎯 Supported commands
1. **Append** - appends to the end of the file
2. **Newline** - starts a newline at current cursor position
3. **Save to a txt file**
4. **Load from a txt file**
5. **Print current text** to the screen
6. **Insert** substring at current cursor position
7. **Insert & Replace** at current cursor position 
8. **Search for a substring** and output where it occurs
9. **Set Cursor Position** - sets cursor at specified row column 
10. **Delete specified number of symbols** from current cursor position
11. **Cut/Copy/Paste** (if buffer is empty it pastes an empty str)
12. **Cipher txt** enables you to Encrypt/Decrypt an existing `.txt` 
    
# 📁 Repo structure and source files
1. `Buffer.h` features the functionality of buffer which powers the program, it is dependent on `Cursor` inner class and `Common.h`.
2. `Cursor` simple class for managing an object which has `size_t` `Row` and `Column` fields.
3. `Common.h` is a header file with functions which are not clasified to direct functionality, but still important.
4. `TextEditor.h` is a header which features the `TextEditor` class the main idea of which is to encapsulate the process of handling the user input calling corresponding functions from `Buffer.h`, `CaesarCipher.h`, etc
5. `CaesarCipher.h` is a header file which is responsible for Encrypting and Decrypting logic, it has some methods which are not used in this project 
6. `Source.cpp` is a main file, it gets the command from user, and passes on the execution to headers files

# 👨‍💻 Interface example
### Main Menu:
<img width="629" alt="main menu" src="https://github.com/hermanhavva/C-Console-Text-Editor/assets/108483440/2bad9124-aca7-4a33-a7ef-0f9643a9c1d1">

### Current Cursor marker:
<img width="637" alt="cursor demonstrating" src="https://github.com/hermanhavva/C-Console-Text-Editor/assets/108483440/affb9bad-400b-4200-976b-6305f5df1fb8">


  *This project is a part of a course on Programming Paradigms at Kyiv School of Economics*
