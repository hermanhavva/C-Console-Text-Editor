
#include <stdio.h>

using namespace std;

// можна зробити команду 
enum Mode
{
	APPEND = 1,
	NEWLINE = 2,
	SAVETOFILE = 3,
	LOADFROMFILE = 4,
	PRINTCURRENT = 5,
	INSERT = 6
};
void PrintMainMenu()
{
	printf("1 - append, 2 - newline, 3 - save to a file, 4 - load from file, 5 - print current, 6 - insert\n");
}
enum Mode GetUserCommand() 
{
	printf("Enter number: ");
	enum Mode command;
	char input[10];
	scanf_s("%s", input, sizeof(input));
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
		break;
	}
	return command;
}

int main()
{
	PrintMainMenu();
	printf("%d", GetUserCommand());
	return 0; 

}




