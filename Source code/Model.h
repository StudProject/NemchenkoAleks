#ifndef __MODEL__
#define __MODEL__

#include <Windows.h>

/*!
	\brief  TAsteroid structure describes the parameters of the food, such as coordinate, spead and radius.
*/
struct TAsteroid
{
	double X;///<coordinate food 
	double Y;///<coordinate food
	double dX;///<speed food
	double	dY;///<speed food
	double r;///<radius food


};

/*!
	\brief  TShip structure describes the parameters of the player, such as coordinate, spead and health player.
*/
struct TShip
{
	double X;///<coordinate player
	double Y;///<coordinate player
	
	double vy;///<speed player
	double vx;///<speed player

	int health; ///<health player
	bool isSelected;
	
	
};
extern double targetx,targety;


TAsteroid CreateAsteroid(RECT r);

#endif