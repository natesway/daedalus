
#include "stdafx.h"

#include <stdio.h>


#include "SysGL/GL.h"

#include "Graphics/GraphicsContext.h"

#include "Graphics/ColourValue.h"

#include "SysGL/Interface/imgui.h"
#include "SysGL/Interface/backends/imgui_impl_sdl.h"
#include "SysGL/Interface/backends/imgui_impl_opengl3.h"


static u32 SCR_WIDTH = 640;
static u32 SCR_HEIGHT = 480;

SDL_Window * gWindow = nullptr;

class GraphicsContextGL : public CGraphicsContext
{
public:
	virtual ~GraphicsContextGL();


	virtual bool Initialise();
	virtual bool IsInitialised() const { return true; }

	virtual void ClearAllSurfaces();
	virtual void ClearZBuffer();
	virtual void ClearColBuffer(const c32 & colour);
	virtual void ClearToBlack();
	virtual void ClearColBufferAndDepth(const c32 & colour);
	virtual	void BeginFrame();
	virtual void EndFrame();
	virtual void UpdateFrame( bool wait_for_vbl );

	virtual void GetScreenSize(u32 * width, u32 * height) const;
	virtual void ViewportType(u32 * width, u32 * height) const;

	virtual void SetDebugScreenTarget( ETargetSurface buffer ) {}
	virtual void DumpNextScreen() {}
	virtual void DumpScreenShot() {}
};

template<> bool CSingleton< CGraphicsContext >::Create()
{
	DAEDALUS_ASSERT_Q(mpInstance == NULL);

	mpInstance = new GraphicsContextGL();
	return mpInstance->Initialise();
}


GraphicsContextGL::~GraphicsContextGL()
{
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	SDL_Quit();
}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %d - %s\n", error, description);
}


extern bool initgl();
bool GraphicsContextGL::Initialise()
{

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		return false;
	}

	//Use OpenGL 3.3 core
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	//Create window
	gWindow = SDL_CreateWindow( "Daedalus", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCR_WIDTH, SCR_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

	//Create context
	SDL_GLContext gContext = SDL_GL_CreateContext( gWindow );

	SDL_GL_MakeCurrent(gWindow, gContext);

	SDL_GL_SetSwapInterval(1);

	 // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    const char* glsl_version = "#version 130";
    ImGui_ImplSDL2_InitForOpenGL(gWindow, gContext);
    ImGui_ImplOpenGL3_Init(glsl_version);


	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);



	GLenum err = glewInit();
	if (err != GLEW_OK || !GLEW_VERSION_3_2)
	{
		SDL_DestroyWindow(gWindow);
		gWindow = NULL;
		//SDL_Quit();
		return false;
	}

	//ClearColBufferAndDepth(0,0,0,0);
	UpdateFrame(false);
	return initgl();
}


void GraphicsContextGL::GetScreenSize(u32 * width, u32 * height) const
{
	int window_width, window_height;
	SDL_GL_GetDrawableSize(gWindow, &window_width, &window_height);

	*width  = window_width;
	*height = window_height;
}

void GraphicsContextGL::ViewportType(u32 * width, u32 * height) const
{
	GetScreenSize(width, height);
}

void GraphicsContextGL::ClearAllSurfaces()
{
	// FIXME: this should clear/flip a couple of times to ensure the front and backbuffers are cleared.
	// Not sure if it's necessary...
	ClearToBlack();
}


void GraphicsContextGL::ClearToBlack()
{
	glDepthMask(GL_TRUE);
	glClearDepth( 1.0f );
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void GraphicsContextGL::ClearZBuffer()
{
	glDepthMask(GL_TRUE);
	glClearDepth( 1.0f );
	glClear( GL_DEPTH_BUFFER_BIT );
}

void GraphicsContextGL::ClearColBuffer(const c32 & colour)
{
	glClearColor( colour.GetRf(), colour.GetGf(), colour.GetBf(), colour.GetAf() );
	glClear( GL_COLOR_BUFFER_BIT );
}

void GraphicsContextGL::ClearColBufferAndDepth(const c32 & colour)
{
	glDepthMask(GL_TRUE);
	glClearDepth( 1.0f );
	glClearColor( colour.GetRf(), colour.GetGf(), colour.GetBf(), colour.GetAf() );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void GraphicsContextGL::BeginFrame()
{
	// Get window size (may be different than the requested size)
	u32 width, height;
	GetScreenSize(&width, &height);

	// Special case: avoid division by zero below
	height = height > 0 ? height : 1;

	glViewport( 0, 0, width, height );
	glScissor( 0, 0, width, height );
}

void GraphicsContextGL::EndFrame()
{
}

void GraphicsContextGL::UpdateFrame( bool wait_for_vbl )
{
	SDL_GL_SwapWindow(gWindow);

//	if( gCleanSceneEnabled ) //TODO: This should be optional
//	{
	//	ClearColBuffer( c32(0xff000000) ); // ToDo : Use gFillColor instead?
	//}
}
