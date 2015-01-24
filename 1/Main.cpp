//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		Multitexturing									 //
//																		 //
//		$Description:	Shows how to multitexture a polygon				 //
//																		 //
//***********************************************************************//

// This is a compiler directive that includes libraries (For Visual Studio)
// You can manually include the libraries in the "Project->settings" menu under
// the "Link" tab.  You need these libraries to compile this program.
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "main.h"										// This includes our header file
#include "glext.h"										// * IMPORTANT * Must include for extentions
#include <assert.h>

#define kSpeed	0.02f

bool  g_bFullScreen = true;								// Set full screen as default
HWND  g_hWnd;											// This is the handle for the window
RECT  g_rRect;											// This holds the window dimensions
HDC   g_hDC;											// General HDC - (handle to device context)
HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;									// This holds the global hInstance for UnregisterClass() in DeInit()
CCamera g_Camera;

UINT g_Texture[MAX_TEXTURES];							// This will reference to our texture data stored with OpenGL UINT is an unsigned int (only positive numbers)

PFNGLMULTITEXCOORD2FARBPROC		glMultiTexCoord2fARB	= NULL;
PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB		= NULL;

void Init(HWND hWnd)
{
	g_hWnd = hWnd;										// Assign the window handle to a global window handle
	GetClientRect(g_hWnd, &g_rRect);					// Assign the windows rectangle to a global RECT
	InitializeOpenGL(g_rRect.right, g_rRect.bottom);	// Init OpenGL with the global rect
	g_Camera.PositionCamera(0, 0, 6,   0, 0, 0,   0, 1, 0);

	glActiveTextureARB		= (PFNGLACTIVETEXTUREARBPROC)		wglGetProcAddress("glActiveTextureARB");
	glMultiTexCoord2fARB	= (PFNGLMULTITEXCOORD2FARBPROC)		wglGetProcAddress("glMultiTexCoord2fARB");
	if(!glActiveTextureARB || !glMultiTexCoord2fARB)
	{
		// Print a error message and quit.
		MessageBox(g_hWnd, "Your version of OpenGL does not support multitexturing", "Error", MB_OK);
		PostQuitMessage(0);
	}

	// There will be 4 textures loaded to demonstrate the effect.  We use our own CreateTexture to
	// load the bitmaps and save them in OpenGL texture memory.

	CreateTexture(g_Texture[0], "level0.bmp");				// Load the brick wall into our first texture index
	CreateTexture(g_Texture[1], "LightMap.bmp");			// Load the light map into our second texture index
	CreateTexture(g_Texture[2], "Cove.bmp");				// Load the background picture into our third texture index
	CreateTexture(g_Texture[3], "Fog.bmp");					// Load the fog into our fourth texture index
	CreateTexture(g_Texture[4], "steel.bmp");	

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

}

void CheckForMovement()
{
	CVector3 direction;
	direction.z = 0;
	
	if(GetKeyState(VK_UP) & 0x80) {				// If we hit the UP arrow key
		direction.x = 0;
		direction.y = 1;
		g_Camera.MoveCamera(direction,kSpeed);	
				// Move our camera forward by a positive speed
	}

	if(GetKeyState(VK_DOWN) & 0x80) {			// If we hit the DOWN arrow key
		direction.x = 0;
		direction.y = -1;
		g_Camera.MoveCamera(direction,kSpeed);	
	}

	if(GetKeyState(VK_LEFT) & 0x80) {			// If we hit the LEFT arrow key
		direction.x = -1;
		direction.y = 0;
		g_Camera.MoveCamera(direction,kSpeed);	
	}

	if(GetKeyState(VK_RIGHT) & 0x80) {			// If we hit the RIGHT arrow key
		direction.x = 1;
		direction.y = 0;
		g_Camera.MoveCamera(direction,kSpeed);	
	}
	
}

bool AnimateNextFrame(int desiredFrameRate)
{
	static float lastTime = GetTickCount() * 0.001f;
	static float elapsedTime = 0.0f;

	float currentTime = GetTickCount() * 0.001f; // Get the time (milliseconds = seconds * .001)
	float deltaTime = currentTime - lastTime; // Get the slice of time
	float desiredFPS = 1.0f / desiredFrameRate; // Store 1 / desiredFrameRate

	elapsedTime += deltaTime; // Add to the elapsed time
	lastTime = currentTime; // Update lastTime

	// Check if the time since we last checked is greater than our desiredFPS
	if( elapsedTime > desiredFPS )
	{
		elapsedTime -= desiredFPS; // Adjust our elapsed time

		// Return true, to animate the next frame of animation
		return true;
	}

	// We don't animate right now.
	return false;
}


WPARAM MainLoop()
{
	MSG msg;

	while(1)											// Do our infinite loop
	{													// Check if there was a message
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{ 
			if(msg.message == WM_QUIT)					// If the message wasn't to quit
				break;
			TranslateMessage(&msg);						// Find out what the message does
			DispatchMessage(&msg);						// Execute the message
		}
		else											// if there wasn't a message
		{ 		
			if(AnimateNextFrame(60))					// Make sure we only animate 60 FPS
			{
				CheckForMovement();
				RenderScene();							// Render the scene every frame
			}
			else
			{
				Sleep(1);								// Let other processes work
			}
		} 
	}

	DeInit();											// Clean up and free all allocated memory

	return(msg.wParam);									// Return from the program
}




void RenderScene() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glClearColor(0.5,0.5,0.5,0);
	glLoadIdentity();									// Reset The matrix
	
	gluLookAt(g_Camera.m_vPosition.x, g_Camera.m_vPosition.y, g_Camera.m_vPosition.z,	
			  g_Camera.m_vView.x,	  g_Camera.m_vView.y,     g_Camera.m_vView.z,	
			  g_Camera.m_vUpVector.x, g_Camera.m_vUpVector.y, g_Camera.m_vUpVector.z);


	//init_blocks();

	// Set the second texture ID to be active, then bind our light map texture to this ID.
	//glActiveTextureARB(GL_TEXTURE1_ARB);
	//glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D,  g_Texture[1]);
	/////////////////////////////////////////////////////////////////////////////////////
	//
	//     LEVEL 0
	//
	//////////////////////////////////////////////////////////////////////////////////////

	glBindTexture(GL_TEXTURE_2D,  g_Texture[0]);
	
	glPushMatrix();

	glTranslatef(2, 0, 0);

	glBegin(GL_QUADS);
	
	glTexCoord2f(0.0f, 1.0f);
	//glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 1.0f);
	glVertex3f(-1, 1, 0);

	glTexCoord2f(0.0f, 0.0f);
	//glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 0.0f);
	glVertex3f(-1, -1, 0);

	// Display the bottom right vertice with the textures for both textures
	glTexCoord2f(1.0f, 0.0f);
	//glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 0.0f);
	glVertex3f(1, -1, 0);

	// Display the top right vertice with the textures for both textures
	glTexCoord2f(1.0f, 1.0f);
	//glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 1.0f);
	glVertex3f(1, 1, 0);

	// Stop drawing 
	glEnd();											
	glPopMatrix();

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//     LEVEL 1
	//
	//////////////////////////////////////////////////////////////////////////////////////
	
	glBindTexture(GL_TEXTURE_2D,  g_Texture[2]);

	static float wrap = 0;
	glPushMatrix();
	// Move the QUAD over to the right of the screen
	glTranslatef(-2, 0, 0);

	// Display a multitextured quad texture to the screen
	glBegin(GL_QUADS);

		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-1, 1, 0);

		// Display the bottom left vertice with each texture's coodinates
		glTexCoord2f( 0.0f, 0.0f);
		glVertex3f(-1, -1, 0);

		// Display the bottom right vertice with each texture's coodinates
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(1, -1, 0);

		// Display the top right vertice with each texture's coodinates
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1, 1, 0);

	// Stop drawing QUADS
	glEnd();
	glPopMatrix();
	
	//////////////////////////////////////////////////////////////////
	//
	//  Player
	//
	//////////////////////////////////////////////////////////////////////
	//glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D,  g_Texture[4]);
	glPushMatrix();

	glTranslatef(g_Camera.m_vView.x, g_Camera.m_vView.y, g_Camera.m_vView.z);

	glBegin(GL_QUADS);
	float size = 0.1;
	glTexCoord2f (0.0f, 1.0f);
	//glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 1.0f);
	glVertex3f(-size, size, 0);

	glTexCoord2f(0.0f, 0.0f);
	//glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 0.0f);
	glVertex3f(-size, -size, 0);

	// Display the bottom right vertice with the textures for both textures
	glTexCoord2f(1.0f, 0.0f);
	//glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 0.0f);
	glVertex3f(size, -size, 0);

	// Display the top right vertice with the textures for both textures
	glTexCoord2f(1.0f, 1.0f);
	//glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 1.0f);
	glVertex3f(size, size, 0);

	// Stop drawing 
	glEnd();											
	glPopMatrix();

	SwapBuffers(g_hDC);
	assert(glGetError() == GL_NO_ERROR);							
}


LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LONG    lRet = 0; 
    PAINTSTRUCT    ps;

    switch (uMsg)
	{ 
    case WM_SIZE:										// If the window is resized
		if(!g_bFullScreen)								// Do this only if we are NOT in full screen
		{
			SizeOpenGLScreen(LOWORD(lParam),HIWORD(lParam));// LoWord=Width, HiWord=Height
			GetClientRect(hWnd, &g_rRect);				// Get the window rectangle
		}
        break; 
 
	case WM_PAINT:										// If we need to repaint the scene
		BeginPaint(hWnd, &ps);							// Init the paint struct		
		EndPaint(hWnd, &ps);							// EndPaint, Clean up
		break;

	case WM_KEYDOWN:
		switch(wParam) {								// Check if we hit a key
			case VK_ESCAPE:								// If we hit the escape key
				PostQuitMessage(0);						// Send a QUIT message to the window
				break;
		}
		break;
 
    case WM_CLOSE:										// If the window is being closes
        PostQuitMessage(0);								// Send a QUIT Message to the window
        break; 
     
    default:											// Return by default
        lRet = DefWindowProc (hWnd, uMsg, wParam, lParam); 
        break; 
    } 
 
    return lRet;										// Return by default
}

/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// That is it!  I bet you thought there was more to this didn't you?  Well, there
// is actually.  We just skimmed the surface.  You will want to be able to
// fade certain textures, and have the background picture be more bright,
// or vice versa.  You will want to use the EXT_texture_env_combine extension.
// More on this extension in a future tutorial.  This extension will also
// allow you to do bump mapping extremely fast.
//
// Let's review:
//
// 1) Before we do anything, we need to create some function pointers to hold the
// extensions that are read in.  In order to use these data types, we need to have
// the prototypes either copied in to our program, or just include glext.h.
// you can just stick this file into your include\GL\ directory, where all your other
// opengl include files are: C:\Program Files\Microsoft Visual Studio\VC98\Include\
//
// PFNGLMULTITEXCOORD2FARBPROC		glMultiTexCoord2fARB	= NULL;
// PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB		= NULL;
//
// 2) Now we need to read in and store the function pointers, then check if we got
// valid addresses for these functions.
//
// glActiveTextureARB	= (PFNGLACTIVETEXTUREARBPROC)		wglGetProcAddress("glActiveTextureARB");
// glMultiTexCoord2fARB	= (PFNGLMULTITEXCOORD2FARBPROC)		wglGetProcAddress("glMultiTexCoord2fARB");
//
// 3) When we are about to render, we need to to assign our texture to a multitexture ID.
// That way, we can pass in that ID to specify which texture we are giving texture coordinates too.
//
// glActiveTextureARB(GL_TEXTURE0_ARB);
// glEnable(GL_TEXTURE_2D);
// glBindTexture(GL_TEXTURE_2D,  g_Texture[0]);
//
// 4) Now, when we want to give texture coordinates for our texture, we simply pass in the
// texture ID we want and it's texture coordinates to glMultiTexCoord2fARB().
//
// glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, 1.0f);
// glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f - wrap, 1.0f);
// glVertex3f(-1, 1, 0);
//
// That's it!  Let us know at GameTutorials.com if this tutorial helps you.  We would love to
// see what you come up with.
// 
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
// © 2000-2005 GameTutorials
//
