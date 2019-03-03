#include <cmath>
#include "mytrig.h"

using namespace std;

double mycos(double x)
{
	return mysin(x + (M_PI / 2));
}
