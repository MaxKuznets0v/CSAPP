#include "Multihack.h"
#include <iostream>

int main()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	SMALL_RECT windowSize{ 0, 0, 43, 24 };
	SetConsoleWindowInfo(hConsole, TRUE, &windowSize);
	SetConsoleTitle("Multihack CS:GO");

	while (true)
	{
		try 
		{
			Multihack mh;
			break;
		}
		catch (std::runtime_error)
		{
			system("cls");
			std::cout << "Run CS:GO application!\n";
		}
		Sleep(5000);
	}
	return 0;
}