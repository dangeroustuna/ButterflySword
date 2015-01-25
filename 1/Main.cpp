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
#include "hud.h"
#include <assert.h>

#include <iostream>
#include <Windows.h>
#pragma comment(lib, "winmm.lib") // Add the Windows library that contains the code for the function PlaySound()

#define kSpeed	0.2f

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

enum{RED_BUTTON,PLAYER,SAWTRAP,LARGE_SCROLL,MINI_SCROLL,WALL,DOOR,TREASURE,FIRE,MONSTER};
bool touchingButton;
int collision_times_with_button;
bool isDoorOpened[10];
bool isShawdowShow[10];
float sawOffsetY1;
float sawOffsetY2;
float sawSpeed1;
float sawSpeed2;
bool isGameOver;
int roomFourNum[3];
bool touchingRoomFour[3];
CVector3 monster_pos;
int monsterTimer;
Block blocks[100];
int blocks_size;

void reset();

void Init(HWND hWnd)
{
	g_hWnd = hWnd;										// Assign the window handle to a global window handle
	GetClientRect(g_hWnd, &g_rRect);					// Assign the windows rectangle to a global RECT
	InitializeOpenGL(g_rRect.right, g_rRect.bottom);	// Init OpenGL with the global rect
	g_Camera.PositionCamera(-5, -5, 40,   -5, -5, 0,   0, 1, 0);
	PlaySound("puzzle_music.wav", NULL, SND_FILENAME | SND_ASYNC|SND_LOOP);

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


//enum{RED_BUTTON,PLAYER,A,B,C,WALL};
	CreateTexture(g_Texture[RED_BUTTON], "Red_button.bmp");				// Load the brick wall into our first texture index
	CreateTexture(g_Texture[PLAYER], "LightMap.bmp");			// Load the light map into our second texture index
	CreateTexture(g_Texture[SAWTRAP], "Saw_Trap.bmp");				// Load the background picture into our third texture index
	CreateTexture(g_Texture[LARGE_SCROLL], "scroll.bmp");					// Load the fog into our fourth texture index
	CreateTexture(g_Texture[MINI_SCROLL], "scroll_mini.bmp");	
	CreateTexture(g_Texture[WALL], "wall.bmp");	
	CreateTexture(g_Texture[DOOR], "door.bmp");	
	CreateTexture(g_Texture[TREASURE], "Treasure_Case.bmp");	
	CreateTexture(g_Texture[FIRE], "fire.bmp");
	CreateTexture(g_Texture[MONSTER], "beast.bmp");

	CreateTexture(g_Texture[41], "one.bmp");	
	CreateTexture(g_Texture[42], "two.bmp");	
	CreateTexture(g_Texture[43], "three.bmp");	
	CreateTexture(g_Texture[44], "four.bmp");	
	CreateTexture(g_Texture[45], "five.bmp");	
	CreateTexture(g_Texture[46], "six.bmp");	


	reset();

}



void reset(){

		g_Camera.PositionCamera(-5, -5, 40,   -5, -5, 0,   0, 1, 0);
		PlaySound("puzzle_music.wav", NULL, SND_FILENAME | SND_ASYNC|SND_LOOP);

		glActiveTextureARB		= (PFNGLACTIVETEXTUREARBPROC)		wglGetProcAddress("glActiveTextureARB");
		glMultiTexCoord2fARB	= (PFNGLMULTITEXCOORD2FARBPROC)		wglGetProcAddress("glMultiTexCoord2fARB");
		if(!glActiveTextureARB || !glMultiTexCoord2fARB)
			{
			// Print a error message and quit.
			MessageBox(g_hWnd, "Your version of OpenGL does not support multitexturing", "Error", MB_OK);
			PostQuitMessage(0);
			}

		touchingButton = false;
		collision_times_with_button = 0;
		sawOffsetY1=0;
		sawOffsetY2=0;
		sawSpeed1=0.15;
		sawSpeed2=0.25;
		isGameOver=false;
		for(int i=0; i<3;i++){
			roomFourNum[i] = 1;
			touchingRoomFour[i] = false;
		}
		for (int i = 3; i<10;i++){
        isDoorOpened[i] = false;
		}
	
		monster_pos = CVector3(-6,-31,0);
		blocks[49] = Block(CVector3(1,1,0),CVector3(1,1,0),1.0);
		for(int i=0;i<15;i++){
			blocks[60+i] = Block(CVector3(1,1,0),CVector3(1,1,0),1.0);
		}
		monsterTimer=120;
}	

void CheckForMovement(){
	if(isGameOver) return;						//who need to move when the game is over
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
				if(GetKeyState(0x52) & 0x80){			//check if r is pressed
					reset();
				}
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

int findMin(float f, float b, float l, float r)
{
	float m = min(min(f,b),min(l,r));
	if (m == f) return bTOP;
	if (m == b) return bBOTTOM;
	if (m == l) return bLEFT;
	else return bRIGHT;
}

CCamera checkCollision(CCamera camera, Block block){
	// This code checks if any of the blocks have a collision with the camera
	// If there is a collision, we need to find the side of the rectangle that is closed to the camera
	// Then reposition the camera to get a sliding effect.
	CVector3 point = camera.View();
	CVector3 eye = camera.Position();
	CCamera newCamera = CCamera();

	if (block.hasCollision(camera))				
	{
		// relative to block
		float leftdist = abs(block.center.x  - block.dimension.x/2 - block.offset - point.x);
		float rightdist = abs(block.center.x + block.dimension.x/2 + block.offset - point.x);
		float bottomdist = abs(block.center.y - block.dimension.y/2 - block.offset - point.y);
		float topdist = abs(block.center.y + block.dimension.y/2 + block.offset - point.y);
		int minID = findMin(topdist,bottomdist,leftdist,rightdist); // find the side enum value

		if (minID == bBOTTOM) 
		{
			float dy = block.center.y - block.dimension.y/2 - block.offset - point.y;
			point.y += dy; eye.y += dy;
			newCamera.PositionCamera(eye.x,eye.y,eye.z,point.x,point.y,point.z,0,1,0);
			return newCamera;
		}
		if (minID == bTOP) 
		{
			float dy = block.center.y + block.dimension.y/2 + block.offset - point.y;
			point.y += dy; eye.y += dy;
			newCamera.PositionCamera(eye.x,eye.y,eye.z,point.x,point.y,point.z,0,1,0);
			return newCamera;
		}
		if (minID == bLEFT) 
		{
			float dx = block.center.x - block.dimension.x/2 - block.offset - point.x;
			point.x += dx; eye.x += dx;
			newCamera.PositionCamera(eye.x,eye.y,eye.z,point.x,point.y,point.z,0,1,0);
			return newCamera;
		}
		if (minID == bRIGHT) 
		{
			float dx = block.center.x + block.dimension.x/2 + block.offset - point.x;
			point.x += dx; eye.x += dx;
			newCamera.PositionCamera(eye.x,eye.y,eye.z,point.x,point.y,point.z,0,1,0);
			return newCamera;
		}

	}
	return camera;
}

void drawObject(float c_x,float c_y,float dim_x,float dim_y, int textureID){
	glBindTexture(GL_TEXTURE_2D,  g_Texture[textureID]);

	static float wrap = 0;
	glPushMatrix();
	glTranslatef(c_x, c_y, 0);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-dim_x/2, dim_y/2, 0);
		glTexCoord2f( 0.0f, 0.0f);
		glVertex3f(-dim_x/2, -dim_y/2, 0);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(dim_x/2, -dim_y/2, 0);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(dim_x/2, dim_y/2, 0);

	// Stop drawing QUADS
	glEnd();
	glPopMatrix();
}


void drawWall(float c_x,float c_y,float dim_x,float dim_y, int tID){
	drawObject(c_x,c_y,dim_x,dim_y,tID);
}



float player_size = 1.0f;
void addBlock(float c_x, float c_y, float dim_x, float dim_y, int index, int tID)
{
	drawWall(c_x,c_y,dim_x,dim_y, tID);
	blocks[index] = Block(CVector3(c_x,c_y,0),CVector3(dim_x,dim_y,0),player_size);
}

void drawVerticalFireL(float bottom,float top){
	int num = top - bottom + 1;
	for (int i = 0;i<num;i++){
		addBlock(monster_pos.x - 1.5,bottom + i,1,1,60+i,FIRE);
		if(blocks[60+i].hasCollision(g_Camera)){
			isGameOver=true;
		}
	}
}

void drawVerticalFireR(float bottom,float top){
	int num = top - bottom + 1;
	for (int i = 0;i<num;i++){	
		addBlock(monster_pos.x + 1.5,bottom + i,1,1,60+i,FIRE);
		if(blocks[60+i].hasCollision(g_Camera)){
			isGameOver=true;
		}
	}
}

void drawHorizontalFire(float left, float right){
	int num = (right - left)+1;
	for (int i = 0;i<num;i++){
		addBlock(left+i,monster_pos.y+1.5,1,1,60+i,FIRE);
		if(blocks[60+i].hasCollision(g_Camera)){
			isGameOver=true;
		}
	}

}

bool flashing = false;
int timer = 10;

void RenderScene() 
{
	// Timer thing for Room 2 flash
	if (flashing){
		timer -= 1;
		if (timer <= 0){
			timer = 10;
			flashing = false;
		}
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glClearColor(0.5,0.5,0.5,0);
	glLoadIdentity();									// Reset The matrix
	
	gluLookAt(g_Camera.m_vPosition.x, g_Camera.m_vPosition.y, g_Camera.m_vPosition.z,	
			  g_Camera.m_vView.x,	  g_Camera.m_vView.y,     g_Camera.m_vView.z,	
			  g_Camera.m_vUpVector.x, g_Camera.m_vUpVector.y, g_Camera.m_vUpVector.z);


    /////////////////////// WALLLLLLLS //////////////////////////////////////////////
    addBlock(-21,-0.5,42,1,0,WALL);
    addBlock(-8,-10.5,16,1,1,WALL);
    addBlock(-27,-10.5,16,1,2,WALL);
    addBlock(-40,-10.5,4,1,3,WALL);
    addBlock(-19.5,-22.5,17,1,4,WALL);
    addBlock(-32,-22.5,2,1,5,WALL);
    addBlock(-21,-35.5,42,1,6,WALL);
    addBlock(-0.5,-18,1,36,7,WALL);
    addBlock(-9.5,-1,1,2,8,WALL);
    addBlock(-9.5,-8,1,6,9,WALL);
    addBlock(-21.5,-2,1,2,10,WALL);
    addBlock(-21.5,-8,1,4,11,WALL);
    addBlock(-11.5,-15,1,8,12,WALL);
    addBlock(-11.5,-23.5,1,3,13,WALL);
    addBlock(-11.5,-32,1,8,14,WALL);
    addBlock(-20.5,-17,1,12,15,WALL);
    addBlock(-33.5,-8,1,16,16,WALL);
    addBlock(-33.5,-27.5,1,17,17,WALL);
    addBlock(-41.5,-18,1,36,18,WALL);
    
    
    //////////////////////// WALLLLS ///////////////////////////////////////////////////

        ////////////////////// DOORS ////////////////////////
    if(!isDoorOpened[3]){
        addBlock(-21.5,-4.5,0.5,3,53,DOOR);
        if(blocks[53].hasCollision(g_Camera)){
            g_Camera = checkCollision(g_Camera,blocks[53]);
        }
    }

    if(!isDoorOpened[4]){
        addBlock(-17.5,-10.5,3,0.5,54,DOOR);
        if(blocks[54].hasCollision(g_Camera)){
            g_Camera = checkCollision(g_Camera,blocks[54]);
        }
    }

    if(!isDoorOpened[5]){
        addBlock(-11.5,-20.5,0.5,3,55,DOOR);
        if(blocks[55].hasCollision(g_Camera)){
            g_Camera = checkCollision(g_Camera,blocks[55]);
        }
    }

    if(!isDoorOpened[6]){
        addBlock(-11.5,-26.5,0.5,3,56,DOOR);
        if(blocks[56].hasCollision(g_Camera)){
            g_Camera = checkCollision(g_Camera,blocks[56]);
        }
    }

    if(!isDoorOpened[7]){
        addBlock(-29.5,-22.5,3,0.5,57,DOOR);
        if(blocks[57].hasCollision(g_Camera)){
            g_Camera = checkCollision(g_Camera,blocks[57]);
        }
    }

    if(!isDoorOpened[8]){
        addBlock(-33.5,-17.5,0.5,3,58,DOOR);
        if(blocks[58].hasCollision(g_Camera)){
            g_Camera = checkCollision(g_Camera,blocks[58]);
        }
    }

    if(!isDoorOpened[9]){
        addBlock(-36.5,-10.5,3,0.5,59,DOOR);
        if(blocks[59].hasCollision(g_Camera)){
            g_Camera = checkCollision(g_Camera,blocks[59]);
        }
    }


    /////////////////////// DOORS ///////////////////
    blocks_size = 19;

    for(int i=0;i<blocks_size;i++){
        if(blocks[i].hasCollision(g_Camera)){
            g_Camera = checkCollision(g_Camera,blocks[i]);
        }
    }


	//drawWall((-9.0 - 22)/2.0 , -5.5,22-9,11-0);//room 2
	
	//drawWall((-31.5 - 22.5)/2.0 - 0.5, -5.5, 12,10); //room 3
	//drawWall((-11.5-24.5)/2.0,(-22.5 - 10.5)/2.0,13,12);
	//drawWall(-6,-23,11,25);
	//drawWall(-22.5,-29,22,13);
	//drawWall(-29,(-22.5 - 10.5)/2.0,9,12);
	//drawWall(-37.5,-23,8,25);
	//drawWall(-37.5,-5.5,8,10);

	//////////////////////////////////////////////////////////////////
	//
	//  Player
	//
	//////////////////////////////////////////////////////////////////////
	//glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D,  g_Texture[PLAYER]);
	glPushMatrix();

	glTranslatef(g_Camera.m_vView.x, g_Camera.m_vView.y, g_Camera.m_vView.z);

	glBegin(GL_QUADS);
	
	glTexCoord2f (0.0f, 1.0f);
	//glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 1.0f);
	glVertex3f(-player_size, player_size, 0);

	glTexCoord2f(0.0f, 0.0f);
	//glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 0.0f);
	glVertex3f(-player_size, -player_size, 0);

	// Display the bottom right vertice with the textures for both textures
	glTexCoord2f(1.0f, 0.0f);
	//glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 0.0f);
	glVertex3f(player_size, -player_size, 0);

	// Display the top right vertice with the textures for both textures
	glTexCoord2f(1.0f, 1.0f);
	//glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 1.0f);
	glVertex3f(player_size, player_size, 0);

	// Stop drawing 
	glEnd();											
	glPopMatrix();
	
    // ROOOOM 2 //


    drawObject(-13.5,-0.52,1,1,RED_BUTTON);
    Block buttonBlock = Block(CVector3(-13.5,-0.52,0),CVector3(1,1,0),player_size);

    if (collision_times_with_button < 3 && !flashing){
        drawWall(-15.5,-5.5,13.0,11, WALL);
    }

    if (buttonBlock.hasCollision(g_Camera)) {
        if(!touchingButton){
            flashing = true;
            touchingButton = true;
            collision_times_with_button++;
            if (collision_times_with_button >= 3){
                isDoorOpened[3] = true;
                isDoorOpened[4] = true;
            }

        }

    }
    else {
        if (touchingButton){
            touchingButton = false;
        }
    }
    
	
	//Room 3//
	sawOffsetY1+=sawSpeed1;
	if(sawOffsetY1>=3||sawOffsetY1<=-3){
		sawSpeed1 *= -1;
	}
	sawOffsetY2+=sawSpeed2;
	if(sawOffsetY2>=3||sawOffsetY2<=-3){
		sawSpeed2 *= -1;
	}

	drawObject(-25.5,-5.5+sawOffsetY1,1.,2.0,SAWTRAP);
	Block saw1 = Block(CVector3(-25.5,-5.5+sawOffsetY1,0),CVector3(1,2,0),player_size);
	drawObject(-29.5,-5.5+sawOffsetY2,1.,2.0,SAWTRAP);
	Block saw2 = Block(CVector3(-29.5,-5.5+sawOffsetY2,0),CVector3(1,2,0),player_size);
	
	drawObject(-32.5,-5.51,1,1.0,TREASURE);
	Block TREASURECASE = Block(CVector3(-32.5,-5.5,0),CVector3(1,1,0),player_size);

	
	//draw the shaddow
	//drawWall((-31.5 - 22.5)/2.0 - 0.5, -5.5, 12,10,WALL);

	    // ROOOOM 4

    
    drawObject(-14.5,-21,1,2,MINI_SCROLL);
    Block scroll = Block(CVector3(-14.5,-21,0),CVector3(1,2,0),player_size);
    bool drawLargeScroll = false;
    if(scroll.hasCollision(g_Camera)){
        drawLargeScroll = true;
    }


    // numbers on floor
    addBlock(-12.7,-13.5,1,1,40,40+roomFourNum[0]);
    addBlock(-12.7,-15.5,1,1,41,40+roomFourNum[1]);
    addBlock(-12.7,-17.5,1,1,42,40+roomFourNum[2]);

    for(int i=0;i<3;i++){
        if(blocks[40+i].hasCollision(g_Camera)){
            if(!touchingRoomFour[i]){
                touchingRoomFour[i] = true;
                roomFourNum[i] = roomFourNum[i]%6 + 1;
            }
        }
        else{
            touchingRoomFour[i] = false;
        }
    }

    if (roomFourNum[0] == 2 && roomFourNum[1] == 3 && roomFourNum[2] == 4){
        isDoorOpened[5] = true;
    }

    //drawWall((-11.5-24.5)/2.0,(-22.5 - 10.5)/2.0,13,12);
    
    // ROOM 5 //
	if (isDoorOpened[5]){
		addBlock(monster_pos.x,monster_pos.y,2,2,49,MONSTER);
		if (monsterTimer>0){					//use the timer to give the player a head start
			monsterTimer--;
		}
		else{
			if (monster_pos.x == 1234){
				;
			}
			else if (monster_pos.y <-16) {
				drawHorizontalFire(-10.5,-1.5);
				monster_pos.y += kSpeed*0.8;
			}

			else if (monster_pos.y >= -16 && monster_pos.x >= -17.5 && monster_pos.y < -4){
				drawVerticalFireL(-21.5,-11.5);
				monster_pos.x -= kSpeed;
			}

			else if (monster_pos.x <= -17.5 && monster_pos.y < -4){
			
				drawHorizontalFire(-23.5,-12.5);
				monster_pos.y += kSpeed;
			}
			else if (monster_pos.y >= -4 && g_Camera.m_vView.x < -17.5){
				drawVerticalFireL(-9.5,-1.5);
				monster_pos.x -= kSpeed;
				if (monster_pos.x <= -26){
					monster_pos.x = 1234;				//we call this place "Monster's boneyard"
					blocks[49] = Block(CVector3(1,1,0),CVector3(1,1,0),1.0);
					for(int i=0;i<15;i++){
						blocks[60+i] = Block(CVector3(1,1,0),CVector3(1,1,0),1.0);
					}
				}
			}
			else if (monster_pos.y >= -4 && g_Camera.m_vView.x >= -17.5){
				drawVerticalFireR(-9.5,-1.5);
				monster_pos.x += kSpeed;
			}

		}
		


	}
    if (drawLargeScroll)
        drawObject(-15.5,-21,20,20,LARGE_SCROLL);

//   G A M E ~~ O V E R ~~   //
	if(saw2.hasCollision(g_Camera)||saw1.hasCollision(g_Camera) ||blocks[49].hasCollision(g_Camera)){
		isGameOver=true;
	}


	if(isGameOver){
		drawGameOver();
	}
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

