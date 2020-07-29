#include "Multihack.h"
#include <iostream>
using namespace std;

int main()
{
	while (true)
	{
		try 
		{
			Multihack mh;
			break;
		}
		catch (runtime_error& e)
		{
			system("cls");
			cout << "Run CS:GO application!" << endl;
		}
		Sleep(5000);
	}
	return 0;
}