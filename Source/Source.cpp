
#include <stdio.h>
#include <stdlib.h>


// можна зробити команду 
enum Mode
{
	APPEND = 1,
	NEWLINE = 2,
	SAVETOFILE = 3,
	LOADFROMFILE = 4,
	PRINTCURRENT = 5,
	INSERT = 6,
	UNDEFINED = 7
};
void PrintMainMenu()
{
	printf("1 - append, 2 - newline, 3 - save to a file, 4 - load from file, 5 - print current, 6 - insert\n");
}
enum Mode GetUserCommand() 
{
	printf("Enter number: ");
	enum Mode command;
	int inputSize = 10;
	char* input = (char*)malloc(inputSize*sizeof(char));
	scanf_s("%s", input, inputSize);
	switch (input[0])
	{
	case '1':
		command = APPEND;
		break;
	case '2':
		command = NEWLINE;
		break;
	case '3':
		command = SAVETOFILE; 
		break;
	case '4':
		command = LOADFROMFILE;
		break;
	case '5':
		command = PRINTCURRENT;
		break;

	default:
		printf("The command is not implemmented");
		command = UNDEFINED;
		break;
	}
	free (input);
	return command;
}

int main()
{
	PrintMainMenu();
	enum Mode command = GetUserCommand();

	return 0; 

}




