#ifndef _MAIN_H
#define _MAIN_H

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <gl\gl.h>										// Header File For The OpenGL32 Library
#include <gl\glu.h>										// Header File For The GLu32 Library

#define SCREEN_WIDTH 800								// We want our screen width 800 pixels
#define SCREEN_HEIGHT 600								// We want our screen height 600 pixels
#define SCREEN_DEPTH 16									// We want 16 bits per pixel

#define MAX_TEXTURES 100									// This says how many texture we will be using

extern bool  g_bFullScreen;									// Set full screen as default
extern HWND  g_hWnd;										// This is the handle for the window
extern RECT  g_rRect;										// This holds the window dimensions
extern HDC   g_hDC;											// General HDC - (handle to device context)
extern HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
extern HINSTANCE g_hInstance;								// This holds our window hInstance


extern UINT g_Texture[MAX_TEXTURES];					// This is our texture data array

// Here is our vector class
struct CVector3
{
public:
	
	// A default constructor
	CVector3() {}

	// This is our constructor that allows us to initialize our data upon creating an instance
	CVector3(float X, float Y, float Z) 
	{ 
		x = X; y = Y; z = Z;
	}

	// Here we overload the + operator so we can add vectors together 
	CVector3 operator+(CVector3 vVector)
	{
		// Return the added vectors result.
		return CVector3(vVector.x + x, vVector.y + y, vVector.z + z);
	}

	// Here we overload the - operator so we can subtract vectors 
	CVector3 operator-(CVector3 vVector)
	{
		// Return the subtracted vectors result
		return CVector3(x - vVector.x, y - vVector.y, z - vVector.z);
	}
	
	// Here we overload the * operator so we can multiply by scalars
	CVector3 operator*(float num)
	{
		// Return the scaled vector
		return CVector3(x * num, y * num, z * num);
	}

	// Here we overload the / operator so we can divide by a scalar
	CVector3 operator/(float num)
	{
		// Return the scale vector
		return CVector3(x / num, y / num, z / num);
	}

	float magnitude()
	{
		return sqrt(x*x + y*y + z*z);
	}

	void normalize()
	{
		float mag = sqrt(x*x + y*y + z*z);
		x = x/mag;
		y = y/mag;
		z = z/mag;
	}

	CVector3 hVector()
	{
		return CVector3(x,0,z);
	}

	float x, y, z;						
};
///////////////////////////////////////////////////////////////////////////////////////////////////
enum { bTOP , bBOTTOM , bLEFT, bRIGHT };




////////////////////////////////////////////////////////////////////////////////////////////////////////
// This is our camera class
class CCamera {

public:

	CVector3 m_vPosition;									// The camera's position
	CVector3 m_vView;										// The camera's View
	CVector3 m_vUpVector;									// The camera's UpVector

	// Our camera constructor
	CCamera();	
	
	// This changes the position, view, and up vector of the camera.
	// This is primarily used for initialization.
	void PositionCamera(float positionX, float positionY, float positionZ,
			 		    float viewX,     float viewY,     float viewZ,
						float upVectorX, float upVectorY, float upVectorZ);

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// This rotates the camera's view around the position depending on the values passed in.
	// The rotation is done using axis-angle rotation. That means you give this function
	// a degree (in radians) and a axis rotate around.  
	void RotateView(float angle, float x, float y, float z);

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// This will move the camera forward or backward depending on the speed
	void MoveCamera(CVector3 direction,float speed);

	CVector3 View()
	{
		return m_vView;
	}

	CVector3 Position()
	{
		return m_vPosition;
	}
};


class Block{

public:
	Block(){};
	Block(CVector3 c, CVector3 d, float off){
		center = c;
		dimension = d;
		offset = off;
	}
	CVector3 getCenter()    {	return center;	  }
	CVector3 getDimension() {	return dimension; }
	//		

	bool hasCollision(CCamera camera)
	{
	float hWidth = dimension.x/2 + offset;
	float hLength = dimension.y/2 + offset;
	CVector3 point = camera.View();

	if ((abs(point.x - center.x) < hWidth) && (abs(point.y - center.y) < hLength)) 
		return true;
	return false;
	}

	CVector3 center;				 // will just use x and z
	CVector3 dimension;			     // will just use x = width, z = length
	float offset;

};
// This is our MAIN() for windows
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow);

// The window proc which handles all of window's messages.
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

// This controls our main program loop
WPARAM MainLoop();

// This loads a texture into openGL from a file (IE, "bitmap.bmp")
bool CreateTexture(GLuint &textureID, LPTSTR szFileName);

// This changes the screen to full screen mode
void ChangeToFullScreen();

// This is our own function that makes creating a window modular and easy
HWND CreateMyWindow(LPSTR strWindowName, int width, int height, DWORD dwStyle, bool bFullScreen, HINSTANCE hInstance);

// This allows us to configure our window for OpenGL and backbuffering
bool bSetupPixelFormat(HDC hdc);

// This inits our screen translations and projections
void SizeOpenGLScreen(int width, int height);

// This sets up OpenGL
void InitializeOpenGL(int width, int height);

// This initializes the whole program
void Init(HWND hWnd);

// This draws everything to the screen
void RenderScene();

// This frees all our memory in our program
void DeInit();


#endif 


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// Nothing new was added to this file, except we increase the MAX_TEXTURES to 4
// 
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ?000-2005 GameTutorials
//
//
