#include "Model.h"

double targetx,targety;

TAsteroid CreateAsteroid(RECT r)
{
	TAsteroid result;

	int w = r.right - r.left;
	int h = r.bottom - r.top;

	///The function creates food in a certain area
	result.X = r.left + rand() % w;
	result.Y = r.top + rand() % h;

	///The functio sets the rate of food
	result.dX = -50 + rand() % 100;
	result.dY = -50 + rand() % 100;

	///Amount of food
	result.r = 10 ;

	return result;
}
