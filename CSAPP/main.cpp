#include "Multihack.h"
#include <iostream>
using namespace std;

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
		catch (runtime_error)
		{
			system("cls");
			cout << "Run CS:GO application!" << endl;
		}
		Sleep(5000);
	}
	return 0;
}