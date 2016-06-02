#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <stdio.h>
#include <math.h>
#include <Windowsx.h>

#include "Model.h"


/**
@mainpage Game Agar.
@authors Nemchenko Aleks.
@version 1.2.5

@full The application created based on the famous game Agar and makes it possible to play it offlinet.
*/

/**
@class Main
*/

// Global variables

// The main window class name.
static TCHAR szWindowClass[] = _T("win32app");

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("Автор Немченко Алексей Группа 5441");

HINSTANCE hInst;
HBITMAP hBitmap = NULL; //!

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#define ASTEROID_COUNT 10///<Number of simultaneously generating food 
#define WINDOW_WIDTH 800///< width window
#define WINDOW_HEIGHT 700///< heigt window
int GameOver = 0;
bool pause = false;
TShip ship;
TAsteroid asteroids[ASTEROID_COUNT];

void DoPhysics(ULONGLONG dT);
void StartGame();

/**
@function WinMain
@brief The main function.
@full The main function is the entry point into the program and is responsible for drawing window.
@param hInstance Handle to the current instance of the window
@param hPrevInstance Handle to the previous instance of the window
@param lpCmdLine A pointer to the command line
@param nCmdShow Shows window state 
@return Exit the application code
*/

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL,
            _T("Call to RegisterClassEx failed!"),
            _T("Win32 Guided Tour"),
            NULL);

        return 1;
    }

    hInst = hInstance; // Store instance handle in our global variable
	/**
    @function CreateWindow
	@brief The function that creates the window.
	@param szWindowClass The class name
	@param szTitle Window Name
    @param WS_OVERLAPPEDWINDOW: the type of window to create
    @param CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
    @param 800/700 The width and height of the window
	@param NULL Handle to the parent window and menu handle
	@param hInstance Handle to application instance
	@param NULL Parameters transmitted from the WndProc
	*/
    HWND hWnd = CreateWindow(
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 700,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hWnd)
    {
        MessageBox(NULL,
            _T("Call to CreateWindow failed!"),
            _T("Win32 Guided Tour"),
            NULL);

        return 1;
    }

    	/**
	@function ShowWindow
	@brief Function to display the window.
	@param hWnd Window Handle
	@param nCmdShow Window Display Mode
	*/
    ShowWindow(hWnd,
        nCmdShow);
	/**
	@function UpdateWindow
	@brief Function to update the window.
	@param hWnd Window Handle
	*/
    UpdateWindow(hWnd);

	/// The main message loop
    MSG msg;

	ULONGLONG prevTime = -1, curTime, deltaT;

	/// Call Start function
	StartGame();
/// Enter the infinite message loop
while(TRUE)
{
    /// Check to see if any messages are waiting in the queue
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        /// Translate the message and dispatch it to WindowProc()
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    /// If the message is WM_QUIT, exit the while loop
    if(msg.message == WM_QUIT)
        break;

	/// Removing the number of milliseconds that have elapsed since the system was launched
	curTime = GetTickCount64();
	/// Determination of the time change
	if (prevTime == -1)
		deltaT = 0;
	else deltaT = curTime - prevTime;
	prevTime = curTime;

	/// Calling a function is responsible for the application physics
	if(!pause){
    DoPhysics(deltaT);
	}

	/// Calling a function that initiates a redraw window
	InvalidateRect(hWnd,NULL,1);
}

    return (int) msg.wParam;
}
/*!
\brief  function void StartGame () is designed for the beginning of the start of the game. This function is implemented in the field of generation of food
in a particular area and the size of the player
The function code is as follows:
\code
void StartGame()
{
	//the data of all objects in the beginning of the game
	srand(GetTickCount());

	RECT r = {0,0,700,500};
	for (int i=0;i<ASTEROID_COUNT;i++)
	{
		asteroids[i] = CreateAsteroid(r);
	}
	ship.Y = 100;
	ship.X = 100;

	ship.health = 20;
	ship.isSelected = false;
}
\endcode
*/
void StartGame()
{
	///the data of all objects in the beginning of the game
	srand(GetTickCount());

	RECT r = {0,0,700,500};
	for (int i=0;i<ASTEROID_COUNT;i++)
	{
		asteroids[i] = CreateAsteroid(r);
	}
	ship.Y = 100;
	ship.X = 100;

	ship.health = 20;
	ship.isSelected = false;
}


/*!
\brief 
Function void DoPhysics responsible for all of physics in the game. In this function implements the player position on the playing field,
check with player touch the food. 

While the player touched the food, score increased  and if a player touched the food more than he that game over . The game ends when the player earns 100 points.
   \code
   for (int i=0;i<ASTEROID_COUNT;i++)
	{
		//repulsion of food from the walls
		asteroids[i].X -= ((double)dT/1000)*asteroids[i].dX;
		if ((asteroids[i].X-asteroids[i].r < 0) && (asteroids[i].dX > 0) || (asteroids[i].X+asteroids[i].r > 785) && (asteroids[i].dX < 0)) 
		{			
			
			asteroids[i].dX= -1* asteroids[i].dX;
		}
		
		asteroids[i].Y -= ((double)dT/1000)*asteroids[i].dY;
		if ((asteroids[i].Y-asteroids[i].r < 0) && (asteroids[i].dY > 0) || (asteroids[i].Y+asteroids[i].r > 663) && (asteroids[i].dY < 0) )
		{			
			
			asteroids[i].dY= -1* asteroids[i].dY;
		}
		
		//check for player collision with food and increase its size 
		double dx = asteroids[i].X - ship.X;
		double dy = asteroids[i].Y - ship.Y;

			if (sqrt(dx*dx + dy*dy) <= asteroids[i].r + ship.health)
			{
				if(asteroids[i].r>ship.health)
				{
					GameOver=1;
				}
				else
				{
				ship.health += asteroids[i].r/10;
				asteroids[i] = CreateAsteroid(r);
				}
			}
	
		//check for a collision between a food and the absorption more less
		for (int z=1;z<ASTEROID_COUNT-i;z++)
		{
		
			double dX = asteroids[i].X - asteroids[i+z].X ;
			double dY = asteroids[i].Y - asteroids[i+z].Y ;

			if (sqrt(dX*dX + dY*dY) <= asteroids[i].r+asteroids[i+1].r)
			{	
				if(asteroids[i].r < asteroids[i+z].r)
				{
					
					asteroids[i+z].r+=asteroids[i].r;
					asteroids[i] = CreateAsteroid(r);
				}
				else
				{
					
					asteroids[i].r+=asteroids[i+z].r;
					asteroids[i+z] = CreateAsteroid(r);
				}
			}
		}
		//the condition of finishing the game
		if (ship.health>120)
		{
			GameOver=1;
		}
	}	
	 \endcode
	 */
void DoPhysics(ULONGLONG dT)
{
	RECT r = {0,0,785,663};
	///the player position and speed of the food at the end of the game
	if(GameOver == 1)
	{
		ship.X=100;
		ship.Y=100;
		for (int i=0;i<ASTEROID_COUNT;i++)
		{
			asteroids[i].dX=0;
			asteroids[i].dY=0;
		}
		
	}
	for (int i=0;i<ASTEROID_COUNT;i++)
	{
		///repulsion of food from the walls
		asteroids[i].X -= ((double)dT/1000)*asteroids[i].dX;
		if ((asteroids[i].X-asteroids[i].r < 0) && (asteroids[i].dX > 0) || (asteroids[i].X+asteroids[i].r > 785) && (asteroids[i].dX < 0)) 
		{			
			
			asteroids[i].dX= -1* asteroids[i].dX;
		}
		
		asteroids[i].Y -= ((double)dT/1000)*asteroids[i].dY;
		if ((asteroids[i].Y-asteroids[i].r < 0) && (asteroids[i].dY > 0) || (asteroids[i].Y+asteroids[i].r > 663) && (asteroids[i].dY < 0) )
		{			
			
			asteroids[i].dY= -1* asteroids[i].dY;
		}
		
		
		///check for player collision with food and increase its size 
		
		double dx = asteroids[i].X - ship.X;
		double dy = asteroids[i].Y - ship.Y;

			if (sqrt(dx*dx + dy*dy) <= asteroids[i].r + ship.health)
			{
				if(asteroids[i].r>ship.health)
				{
					GameOver=1;
				}
				else
				{
				ship.health += asteroids[i].r/10;
				asteroids[i] = CreateAsteroid(r);
				}
			}
	
		///check for a collision between a food and the absorption more less
		for (int z=1;z<ASTEROID_COUNT-i;z++)
		{
		
			double dX = asteroids[i].X - asteroids[i+z].X ;
			double dY = asteroids[i].Y - asteroids[i+z].Y ;

			if (sqrt(dX*dX + dY*dY) <= asteroids[i].r+asteroids[i+1].r)
			{	
				if(asteroids[i].r < asteroids[i+z].r)
				{
					
					asteroids[i+z].r+=asteroids[i].r;
					asteroids[i] = CreateAsteroid(r);
				}
				else
				{
					
					asteroids[i].r+=asteroids[i+z].r;
					asteroids[i+z] = CreateAsteroid(r);
				}
			}
		}
		///the condition of finishing the game
		if (ship.health>120)
		{
			GameOver=1;
		}
	}	

	///The player speed by X and Y (if the speed decreases)
	if(GameOver==0){
	ship.vx= (targetx-ship.X);
	ship.vy= (targety-ship.Y);
	ship.vy=ship.vy/ship.health*30;
	ship.vx=ship.vx/ship.health*30;

	ship.X += ((double)dT/1000)*ship.vx;
	ship.Y += ((double)dT/1000)*ship.vy;
	}
	


}

/*!
\brief This function is responsible for drawing player, foods, background playing field and shows all the text in the game.
As it prescribe management in our game.
For control of the game and drawing the background playing field meets the code.
 \code
 switch (message)
	{
	case WM_CREATE: 

		hBitmap = (HBITMAP)LoadImage(hInst, L"1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
			//Repeat game
			case 'O':
				pause= !pause;
				break;
			case 'P':
				if(GameOver == 1)
				{
					GameOver=0;
					ship.health = 20;
					RECT r = {0,0,700,500};
					for (int i=0;i<ASTEROID_COUNT;i++)
					{
						asteroids[i] = CreateAsteroid(r);
					}
				}
				break;
			}

	case WM_MOUSEMOVE:
		
		//Passing the coordinates of the mouse cursor.
			xPos = GET_X_LPARAM(lParam);
			targetx=xPos;
			yPos = GET_Y_LPARAM(lParam);
			targety=yPos;
		break;
	
		InvalidateRect(hWnd,NULL,1);
		break;
\endcode
This part of the code responsible for drawing player and foods. Drawing in WinApi performed using brushes and pens. With feathers, we draw the contours of our
player and foods. With a brush paints we figure our color.
 \code
case WM_PAINT:

        hdc = BeginPaint(hWnd, &ps);

        RECT rect;
		BITMAP 			bitmap;
		HDC 			hdcMem;
		HGDIOBJ 		oldBitmap;
        GetClientRect(hWnd, &rect);
        width=rect.right;
        height=rect.bottom;

		backbuffDC = CreateCompatibleDC(hdc);

        backbuffer = CreateCompatibleBitmap( hdc, width, height);

        savedDC = SaveDC(backbuffDC);

		SelectObject( backbuffDC, backbuffer );

		// Draw HERE
		
		//clear window
		hBrush = CreateSolidBrush(RGB(255,0,255));
        FillRect(backbuffDC,&rect,hBrush);
        DeleteObject(hBrush);
		hdcMem = CreateCompatibleDC(hdc);//!
		oldBitmap = SelectObject(hdcMem, hBitmap);//!
		//draw background
		GetObject(hBitmap, sizeof(bitmap), &bitmap);//!
		BitBlt(backbuffDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);//!
		//draw ball
		//Rectangle(backbuffDC,0, 0, 785, 663);
		//draw asteroids

		//Drawing food
		
		for (int i=0;i<ASTEROID_COUNT;i++)
		{
			hPen = CreatePen(PS_SOLID, 1, RGB(asteroids[i].dY, asteroids[i].dX, asteroids[i].dX+40));
			hOldPen = (HPEN)SelectObject(backbuffDC, hPen);
			Ellipse(backbuffDC,asteroids[i].X-asteroids[i].r, asteroids[i].Y-asteroids[i].r,asteroids[i].X+asteroids[i].r,asteroids[i].Y+asteroids[i].r);
			SelectObject(backbuffDC, hOldPen);
			DeleteObject(hPen);
			hPen = CreatePen(PS_SOLID, asteroids[i].r, RGB(asteroids[i].dY, asteroids[i].dX, asteroids[i].dX+40));
			hOldPen = (HPEN)SelectObject(backbuffDC, hPen);
			Ellipse(backbuffDC, asteroids[i].X - (asteroids[i].r/1.9), asteroids[i].Y - (asteroids[i].r/1.9),asteroids[i].X + (asteroids[i].r/1.9), asteroids[i].Y + (asteroids[i].r/1.9));
			Ellipse(backbuffDC, asteroids[i].X - (asteroids[i].r/2.1), asteroids[i].Y - (asteroids[i].r/2.1),asteroids[i].X + (asteroids[i].r/2.1), asteroids[i].Y + (asteroids[i].r/2.1));
			SelectObject(backbuffDC, hOldPen);
			DeleteObject(hPen);
		}

		//Drawing collor player
		hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		hOldPen = (HPEN)SelectObject(backbuffDC, hPen);
		Ellipse(backbuffDC,ship.X-ship.health,ship.Y-ship.health,ship.X+ship.health,ship.Y+ship.health);	

		SelectObject(backbuffDC, hOldPen);
		DeleteObject(hPen);
\endcode
This part of the code responsible for entering text on a screen. To enter text on the screen creates an array of characters. Using this array, we can create the text. If the number of characters exceeds the specified number of the program will display an error. The text displayed on the screen at a certain place and under certain conditions. The implementation is shown below
 \code

		WCHAR s[30];
		if(GameOver == 1){
		wsprintf(s, _T("Game over!")); //Conclusion inscriptions "Game over!"
		TextOut(backbuffDC, 350, 250, s, wcslen(s));
		wsprintf(s, _T("Replay enter P"));
		TextOut(backbuffDC, 350, 270, s, wcslen(s));
		wsprintf(s,_T("score %d"),ship.health-20);// The conclusion points during the end game
		TextOut(backbuffDC,350,300,s,wcslen(s));
		}
		else {wsprintf(s,_T("score %d"),ship.health-20);// The conclusion points during the game
		TextOut(backbuffDC,20,630,s,wcslen(s));}

		if(pause){
		wsprintf(s, _T("Pause")); //Conclusion inscriptions "pause"
		TextOut(backbuffDC, 350, 250, s, wcslen(s));
		}
		wsprintf(s,_T("ver 1.2.5"));//ver game
		TextOut(backbuffDC,700,630,s,wcslen(s));
		wsprintf(s,_T("Replay enter P"));
		TextOut(backbuffDC,20,30,s,wcslen(s));
		wsprintf(s,_T("Pause enter O"));
		TextOut(backbuffDC,20,50,s,wcslen(s));
		\endcode
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
	int savedDC;
	HDC backbuffDC;
	int width, height;

	HBITMAP backbuffer;
	HBRUSH hBrush;
   
	int xPos, yPos;

	HPEN hPen, hOldPen;

    switch (message)
    {
		///Paint background
	case WM_CREATE: 

		hBitmap = (HBITMAP)LoadImage(hInst, L"1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
			
			case 'O':
				pause= !pause;
				break;
			///Repeat game
			case 'P':
				if(GameOver == 1)
				{
					GameOver=0;
					ship.health = 20;
					RECT r = {0,0,700,500};
					for (int i=0;i<ASTEROID_COUNT;i++)
					{
						asteroids[i] = CreateAsteroid(r);
					}
				}
				break;
			}

		///Passing the coordinates of the mouse cursor.
		case WM_MOUSEMOVE:
			xPos = GET_X_LPARAM(lParam);
			targetx=xPos;
			yPos = GET_Y_LPARAM(lParam);
			targety=yPos;
		break;
	
		InvalidateRect(hWnd,NULL,1);
		break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);

        RECT rect;
		BITMAP 			bitmap;
		HDC 			hdcMem;
		HGDIOBJ 		oldBitmap;
        GetClientRect(hWnd, &rect);
        width=rect.right;
        height=rect.bottom;

		backbuffDC = CreateCompatibleDC(hdc);

        backbuffer = CreateCompatibleBitmap( hdc, width, height);

        savedDC = SaveDC(backbuffDC);

		SelectObject( backbuffDC, backbuffer );

		///clear window
		hBrush = CreateSolidBrush(RGB(255,0,255));
        FillRect(backbuffDC,&rect,hBrush);
        DeleteObject(hBrush);
		hdcMem = CreateCompatibleDC(hdc);//!
		oldBitmap = SelectObject(hdcMem, hBitmap);//!
		///draw background
		GetObject(hBitmap, sizeof(bitmap), &bitmap);//!
		BitBlt(backbuffDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);//!
		///draw ball
		///Rectangle(backbuffDC,0, 0, 785, 663);
		///draw asteroids

		//Drawing food
		
		for (int i=0;i<ASTEROID_COUNT;i++)
		{
			hPen = CreatePen(PS_SOLID, 1, RGB(asteroids[i].dY, asteroids[i].dX, asteroids[i].dX+40));
			hOldPen = (HPEN)SelectObject(backbuffDC, hPen);
			Ellipse(backbuffDC,asteroids[i].X-asteroids[i].r, asteroids[i].Y-asteroids[i].r,asteroids[i].X+asteroids[i].r,asteroids[i].Y+asteroids[i].r);
			SelectObject(backbuffDC, hOldPen);
			DeleteObject(hPen);
			hPen = CreatePen(PS_SOLID, asteroids[i].r, RGB(asteroids[i].dY, asteroids[i].dX, asteroids[i].dX+40));
			hOldPen = (HPEN)SelectObject(backbuffDC, hPen);
			Ellipse(backbuffDC, asteroids[i].X - (asteroids[i].r/1.9), asteroids[i].Y - (asteroids[i].r/1.9),asteroids[i].X + (asteroids[i].r/1.9), asteroids[i].Y + (asteroids[i].r/1.9));
			Ellipse(backbuffDC, asteroids[i].X - (asteroids[i].r/2.1), asteroids[i].Y - (asteroids[i].r/2.1),asteroids[i].X + (asteroids[i].r/2.1), asteroids[i].Y + (asteroids[i].r/2.1));
			SelectObject(backbuffDC, hOldPen);
			DeleteObject(hPen);
		}

		///Drawing collor player
		hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		hOldPen = (HPEN)SelectObject(backbuffDC, hPen);
		Ellipse(backbuffDC,ship.X-ship.health,ship.Y-ship.health,ship.X+ship.health,ship.Y+ship.health);	

		SelectObject(backbuffDC, hOldPen);
		DeleteObject(hPen);

		WCHAR s[30];
		if(GameOver == 1){
			///Conclusion inscriptions "Game over!"
		wsprintf(s, _T("Game over!")); 
		TextOut(backbuffDC, 350, 250, s, wcslen(s));
		wsprintf(s, _T("Replay enter P"));
		TextOut(backbuffDC, 350, 270, s, wcslen(s));
		/// The conclusion points during the end game
		wsprintf(s,_T("score %d"),ship.health-20);
		TextOut(backbuffDC,350,300,s,wcslen(s));
		}
		/// The conclusion points during the game
		else {wsprintf(s,_T("score %d"),ship.health-20);
		TextOut(backbuffDC,20,630,s,wcslen(s));}

		///Conclusion inscriptions "pause"
		if(pause){
		wsprintf(s, _T("Pause")); 
		TextOut(backbuffDC, 350, 250, s, wcslen(s));
		}
		wsprintf(s,_T("ver 1.2.5"));
		TextOut(backbuffDC,700,630,s,wcslen(s));
		wsprintf(s,_T("Replay enter P"));
		TextOut(backbuffDC,20,30,s,wcslen(s));
		wsprintf(s,_T("Pause enter O"));
		TextOut(backbuffDC,20,50,s,wcslen(s));
		

		/// End application-specific layout section.

		BitBlt(hdc,0,0,width,height,backbuffDC,0,0,SRCCOPY);
        RestoreDC(backbuffDC,savedDC);

        DeleteObject(backbuffer);
        DeleteDC(backbuffDC);
		SelectObject(hdcMem, oldBitmap);
		DeleteDC(hdcMem);

		/// End drawing
        EndPaint(hWnd, &ps);
        break;

	/// Post Uninstall window background
	case WM_ERASEBKGND:
		return 1;
		
	/// The message about the destruction of the window
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    return 0;
}