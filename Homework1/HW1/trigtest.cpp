#include <cstdlib>
#include <cstdio>
#include <cmath>
#include "mytrig.h"

using namespace std;

double degToRad(double deg)
{
	 return deg * (M_PI / 180);
}

int main()
{
	
	for (double i = 0; i <= 180; i += 10)
	{
		double rad = degToRad(i);
		double a = mysin(rad);
		double b = mycos(rad);
		double c = mytan(rad);
		double d = sin(rad);
                double e = cos(rad);
                double f = tan(rad);		

		printf("I calculated: \n");
		printf ("sin of %f˚ is %f \n cos of %f˚ is %f \n tan of %f˚ is %f \n", i, a, i, b, i, c);
		printf ("It calculated: \n");
		printf ("sin of %f˚ is %f \n cos of %f˚ is %f \n tan of %f˚ is %f \n", i, d, i, e, i, f);
	}
	return 0;
}
