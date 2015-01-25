#include "hud.h"
#include "string.h"


#define EDGE_WIDTH 50
enum{GAMEOVER};
bool isTextureLoaded = false;
UINT hud_Texture[MAX_TEXTURES];							// This will reference to our texture data stored with OpenGL UINT is an unsigned int (only positive numbers)


void PerspectiveMode()										// Set Up A Perspective View
{


	// Enter into our projection matrix mode
	glMatrixMode( GL_PROJECTION );							

	// Pop off the last matrix pushed on when in projection mode (Get rid of ortho mode)
	glPopMatrix();											

	// Go back to our model view matrix like normal
	glMatrixMode( GL_MODELVIEW );							

	// We should be in the normal 3D perspective mode now
}

void OrthoMode(int left, int top, int right, int bottom)
{

	// Switch to our projection matrix so that we can change it to ortho mode, not perspective.
	glMatrixMode(GL_PROJECTION);						

	// Push on a new matrix so that we can just pop it off to go back to perspective mode
	glPushMatrix();									
	
	// Reset the current matrix to our identify matrix
	glLoadIdentity();								

	//Pass in our 2D ortho screen coordinates.like so (left, right, bottom, top).  The last
	// 2 parameters are the near and far planes.
	glOrtho( left, right, bottom, top, 0, 1 );	
	
	// Switch to model view so that we can render the scope image
	glMatrixMode(GL_MODELVIEW);								

	// Initialize the current model view matrix with the identity matrix
	glLoadIdentity();										
}

void LoadHudTexture(){
	CreateTexture(hud_Texture[GAMEOVER], "Gameover.bmp");
	isTextureLoaded = true;

}


//void drawText(float x, float y, float r, float g, float b, string s){
//	OrthoMode(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
//	glPushMatrix();
//
//	
//	glColor3f(r,g,b);
//
//		
//	glDisable(GL_BLEND);
//	glRasterPos2f(x, y);
//	
//	for(int i=0; i<s.length();i++){
//		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,s[i]);
//	}
//	glPopMatrix();
//	//PerspectiveMode();
//	glColor3f(1,1,1);
//}


void drawGameOver(){
	OrthoMode(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);


	glColor4f(1,1,1,0);
	// Disable depth testing (MUST do this to make it work)
	glDisable(GL_DEPTH_TEST);

	// Select our desired depth testing and turn on blending
	glBlendFunc(GL_DST_COLOR,GL_ZERO);
	glDisable(GL_BLEND);											

	
	// Bind the mask texture to our new 2D quad
//	if(!isTextureLoaded){
		LoadHudTexture();
	//}
	glBindTexture(GL_TEXTURE_2D,  hud_Texture[GAMEOVER]);
	// Display a 2D quad with the scope/cross hair mask
	glBegin(GL_QUADS);
	
		// Notice that when we are in ortho mode, we use glVertex2f() to pass
		// in screen coordinates, not vertices.  This makes it incredibly easy
		// to put up 2D interface art.  It's just like doing 2D graphics.
		// The texture coordinate stay the same regardless.

		// Display the top left point of the 2D image
		glTexCoord2f(0.0f, 1.0f);	glVertex2f(0, 0);

		// Display the bottom left point of the 2D image
		glTexCoord2f(0.0f, 0.0f);	glVertex2f(0, SCREEN_HEIGHT);

		// Display the bottom right point of the 2D image
		glTexCoord2f(1.0f, 0.0f);	glVertex2f(SCREEN_WIDTH, SCREEN_HEIGHT);

		// Display the top right point of the 2D image
		glTexCoord2f(1.0f, 1.0f);	glVertex2f(SCREEN_WIDTH, 0);

	// Stop drawing 
	glEnd();	
	


	glDisable(GL_BLEND);
		


	// Let's set our mode back to perspective 3D mode.  None of this archaic 2D stuff :)
	PerspectiveMode();
	


}