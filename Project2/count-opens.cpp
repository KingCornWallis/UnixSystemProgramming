//The program outputs 252; compared to the expected 256, 252 is not including
//the fact that it is zero indexed, nor the initial 3 process opened by the OS.
//252 + 1 + 3 = 256


#include <cstdlib>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

int main (int argc, char **argv)
{
	int process;
	
	while (open(argv[0], O_RDONLY) != -1)
	{
		process++;
	}
	cout << "The limit for opening files is " << process << endl;

	return 0;
}
 
