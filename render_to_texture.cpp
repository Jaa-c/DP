//-----------------------------------------------------------------------------
//  [PGR2] Offscreen Rendering
//  25/04/2012
//-----------------------------------------------------------------------------
//  Controls: 
//    [mouse-left-button]  ... scene rotation
//    [t], [T]             ... move geometry along camera view direction
//    [d], [D]             ... change virtual framebuffer dimension
//    [w]                  ... toggle wire mode
//    [r]                  ... recompile shaders
//-----------------------------------------------------------------------------
#define USE_ANTTWEAKBAR

#include "../common/common.h"
#include <assert.h>

// GLOBAL CONSTANTS____________________________________________________________
const char*	TEXTURE_FILE_NAME       = "../common/textures/metal01.raw";
const char* VERTEX_SHADER_FILE_NAME     = "render_to_texture.vs";
const char* FRAGMENT_SHADER_FILE_NAME   = "render_to_texture.fs";

enum eFBODimension
{	
   FBO_DIM_32   = 32,
   FBO_DIM_64   = 64,
   FBO_DIM_128  = 128,
   FBO_DIM_256  = 256,
   FBO_DIM_512  = 512,
   FBO_DIM_1024 = 1024,
   FBO_DIM_2048 = 2048,
   NUM_FBO_DIM_MODES = 7
};

// GLOBAL VARIABLES____________________________________________________________
GLint     g_WindowWidth      = 800;             // Window width
GLint     g_WindowHeight     = 600;             // Window height

GLfloat   g_CameraRot[]      = {0.42f, 0.00f, 0.03f, 0.91f}; // Camera orientation 
GLfloat   g_CameraDistance   = 24.0f;           // Camera distance from origin
bool      g_ShowUserTexture  = true;            // Show/hide user texture
bool      g_WireMode         = false;           // Wire mode enabled/disabled

GLuint    g_DiffuseTexId     = 0;               // Scene diffuse texture id

// Virtual framebuffer configuration
int       g_FBODimension     = FBO_DIM_1024;    // FBO dimension
GLuint    g_UserFBOId        = 0;               // User framebuffer object id
GLuint    g_User1TexId       = 0;               // Target texture for offscreen rendering
GLuint    g_User2TexId       = 0;               // Target texture for offscreen rendering
GLuint    g_User3TexId       = 0;               // Target texture for offscreen rendering

// GLSL variables
GLuint  g_ProgramId        = 0;                 // Program shader id

// Transformation matrixes
glm::mat4 g_CameraProjectionMatrix;             // Camera projection transformation
glm::mat4 g_CameraViewMatrix;                   // Camera view transformation
glm::vec3 g_LightPosition   = glm::vec3(-3.05f, 16.77f,-0.10f);         // Light position in world space (will be computed on the fly)
glm::mat4 g_LightViewMatrix = glm::mat4(0.63f, 0.76f, -0.15f, 0.00f,    // Light view transformation
                                        0.10f, 0.11f,  0.99f, 0.00f,
                                        0.77f,-0.64f, -0.00f, 0.00f,
                                        0.00f, 0.00f,-20.00f, 1.00f);                    
glm::mat4 g_LightProjectionMatrix = glm::frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1000.0f);      // Light projection transformation

// FORWARD DECLARATIONS________________________________________________________
void initGUI(void);
bool createFBO();
void drawScene(int pass);
void TW_CALL cbCompileShaderProgram(void *clientData);
void updateCameraViewMatrix();

//-----------------------------------------------------------------------------
// Name: cbDisplay()
// Desc: 
//-----------------------------------------------------------------------------
void cbDisplay()
{
    // update OGL states
    glPolygonMode(GL_FRONT_AND_BACK, g_WireMode ? GL_LINE : GL_FILL);
    // Update camera transformation matrix
    updateCameraViewMatrix();   
    // Create FBO if necessary
    createFBO();

    glClearColor( 0.65f, 0.81f, 0.60f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    glUseProgram(g_ProgramId);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, g_DiffuseTexId);
        glUniform1i(glGetUniformLocation(g_ProgramId, "u_DiffuseTexture"), 0);
        glUniformMatrix4fv(glGetUniformLocation(g_ProgramId, "u_ProjectionMatrix"), 1, GL_FALSE, &g_CameraProjectionMatrix[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(g_ProgramId, "u_ModelViewMatrix"), 1, GL_FALSE, &g_CameraViewMatrix[0][0]);
        glUniform3fv(glGetUniformLocation(g_ProgramId, "u_LightPosition"), 1, &g_LightPosition.x);

        pgr2DrawScene();
    glUseProgram(0);

    if (g_ShowUserTexture)
    {
        pgr2ShowTexture(g_User1TexId, g_WindowWidth - 200, g_WindowHeight - 200, 200, 200);
        pgr2ShowTexture(g_User2TexId, g_WindowWidth - 200, g_WindowHeight - 400, 200, 200);
        pgr2ShowTexture(g_User3TexId, g_WindowWidth - 200, g_WindowHeight - 600, 200, 200);
    }
}


//-----------------------------------------------------------------------------
// Name: createFBO()
// Desc: 
//-----------------------------------------------------------------------------
bool createFBO()
{
    static int s_FBOSize = 0;

    if (s_FBOSize != g_FBODimension)
    {
        if (g_User1TexId) glDeleteTextures(1, &g_User1TexId);
        if (g_User2TexId) glDeleteTextures(1, &g_User2TexId);
        if (g_User3TexId) glDeleteTextures(1, &g_User3TexId);
	    if (g_UserFBOId) glDeleteFramebuffers(1, &g_UserFBOId);
	
	    glGenFramebuffers(1, &g_UserFBOId);
	    assert(g_UserFBOId > 0);
	
	    // TODO: create virtual framebuffer

	    // Check FBO status and OGL error state
	    GLenum error  = glGetError();
	    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	    // Release FBO
	    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	    if ((status != GL_FRAMEBUFFER_COMPLETE) || (error != GL_NO_ERROR))
	    {
	        printf("FBO creation failed, glCheckFramebufferStatus() = 0x%x, glGetError() = 0x%x\n", status, error);
	        return false;
	    }

        s_FBOSize = g_FBODimension;
    }

    return true;
}


//-----------------------------------------------------------------------------
// Name: cbInitGL()
// Desc: 
//-----------------------------------------------------------------------------
void cbInitGL()
{
    // Init app GUI
    initGUI();

    // Set OpenGL state variables
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // Create diffuse texture for scene
    g_DiffuseTexId = prg2LoadRGBTextureFromRawFile(TEXTURE_FILE_NAME);

    // Load shader program
    cbCompileShaderProgram(NULL);
}



//-----------------------------------------------------------------------------
// Name: updateCameraViewMatrix()
// Desc: Use OpenGL to compute camera's projection and world-to-camera space 
//       transformation matrixes
//-----------------------------------------------------------------------------
void updateCameraViewMatrix()
{
    g_CameraViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -g_CameraDistance));
    const float STHETA = float(sin(acos(g_CameraRot[3])));
    if (fabs(STHETA) > 0.00001)
	    g_CameraViewMatrix = glm::rotate(g_CameraViewMatrix, 114.592f * acos(g_CameraRot[3]), glm::vec3(g_CameraRot[0]*STHETA, g_CameraRot[1]*STHETA, g_CameraRot[2]*STHETA));
}


//-----------------------------------------------------------------------------
// Name: cbCompileShaderProgram()
// Desc: 
//-----------------------------------------------------------------------------
void TW_CALL cbCompileShaderProgram(void *clientData)
{
    // Delete shader program if exists
    if (g_ProgramId)
    {
        glDeleteProgram(g_ProgramId);
    }

    // Create shader program object
    g_ProgramId = glCreateProgram();
    if (g_ProgramId == 0)
    {
        return;
    }

    // Load shader program ---------------------
    GLuint vs_id = pgr2CreateShaderFromFile(GL_VERTEX_SHADER, VERTEX_SHADER_FILE_NAME);
    GLuint fs_id = pgr2CreateShaderFromFile(GL_FRAGMENT_SHADER, FRAGMENT_SHADER_FILE_NAME);
    glAttachShader(g_ProgramId, vs_id);
    glAttachShader(g_ProgramId, fs_id);
    glDeleteShader(vs_id);
    glDeleteShader(fs_id);
    glLinkProgram(g_ProgramId);
    if (!pgr2CheckProgramLinkStatus(g_ProgramId))
    {
        pgr2CheckProgramInfoLog(g_ProgramId);
        printf("Shader program creation failed.\n");
        getchar(); exit(1);
    }
    else
    {
        printf("Shader program compiled successfully.\n\n");
    }
}


//-----------------------------------------------------------------------------
// Name: initGUI()
// Desc: 
//-----------------------------------------------------------------------------
void initGUI()
{
#ifdef USE_ANTTWEAKBAR
    // Initialize AntTweakBar GUI
    if (!TwInit(TW_OPENGL, NULL))
    {
        assert(0);
    }

    TwWindowSize(g_WindowWidth, g_WindowHeight);
    TwBar *controlBar = TwNewBar("Controls");
    TwDefine("Controls position='10 10' size='230 230' valueswidth=80 refresh=0.1");
   
    TwAddButton(controlBar, "recompile", cbCompileShaderProgram, NULL, " label='recompile shaders' key=r help='Compile shader program.' ");
    TwAddVarRW(controlBar, "show user texture", TW_TYPE_BOOLCPP, &g_ShowUserTexture, " label='show user texture' key=s help='Show/hide user texture.' ");
    TwAddVarRW(controlBar, "wiremode", TW_TYPE_BOOLCPP, &g_WireMode, " label='wire mode' key=w help='Toggle wire mode.' ");

    TwEnumVal fbo_dim_mode[] = 
    { 
        { FBO_DIM_32  ,     "32x32"},
        { FBO_DIM_64  ,     "64x64"},
        { FBO_DIM_128 ,   "128x128"},
        { FBO_DIM_256 ,   "256x256"},
        { FBO_DIM_512 ,   "512x512"},
        { FBO_DIM_1024, "1024x1024"},
        { FBO_DIM_2048, "2048x2048"},
    };
    TwType fbo_dim_type = TwDefineEnum("FBOdim", fbo_dim_mode, NUM_FBO_DIM_MODES);
    TwAddVarRW(controlBar, "dimension", fbo_dim_type, &g_FBODimension, " group='Virtual framebuffer' keyIncr=d keyDecr=D help='Change FBO dimension.' ");

    TwAddVarRW(controlBar, "CameraTranslate", TW_TYPE_FLOAT, &g_CameraDistance, " group='Camera' label='distance' min=1 max=1000 step=0.5 keyIncr=t keyDecr=T help='Cange camera distance.' ");
    TwAddVarRW(controlBar, "CameraRotation", TW_TYPE_QUAT4F, &g_CameraRot, " group='Camera' label='rotation' open help='Change camera orientation.' ");
#endif
}


//-----------------------------------------------------------------------------
// Name: cbWindowSizeChanged()
// Desc: 
//-----------------------------------------------------------------------------
void cbWindowSizeChanged(int width, int height)
{
    glViewport(0, 0, width, height);
    g_WindowWidth  = width;
    g_WindowHeight = height;

    g_CameraProjectionMatrix = glm::perspective(55.0f, GLfloat(g_WindowWidth)/g_WindowHeight, 1.0f, 100.0f);
}


//-----------------------------------------------------------------------------
// Name: cbKeyboardChanged()
// Desc: 
//-----------------------------------------------------------------------------
void cbKeyboardChanged(int key, int action)
{
    switch (key)
    {
        case 't' : g_CameraDistance  += 0.5f;                                           break;
        case 'T' : g_CameraDistance  -= (g_CameraDistance > 0.5) ? 0.5f : 0.0f;         break;
        case 'd' : g_FBODimension <<= (g_FBODimension < FBO_DIM_2048) ? 1 : 0;          break;
        case 'D' : g_FBODimension >>= (g_FBODimension > FBO_DIM_32) ? 1 : 0;            break;
        case 'r' : cbCompileShaderProgram(NULL);                                        break;
        case 'w' : g_WireMode = !g_WireMode;                                            break;
    }

    printf("[t/T] g_CameraDistance = %f\n", g_CameraDistance);
    printf("[d/D] g_FBODimension = %dx%d\n", g_FBODimension, g_FBODimension);
    printf("[w]   g_WireMode = %s,\n", g_WireMode ? "true" : "false");
    printf("[c]     re-compile shaders.\n\n");
}


#ifndef USE_ANTTWEAKBAR
bool g_MouseSceneRotationEnabled  = false;

//-----------------------------------------------------------------------------
// Name: cbMouseButtonChanged()
// Desc: internal
//-----------------------------------------------------------------------------
void GLFWCALL cbMouseButtonChanged(int button, int action)
{
    g_MouseSceneRotationEnabled  = (action == GLFW_PRESS) ? (button == GLFW_MOUSE_BUTTON_LEFT) : false;
}


//-----------------------------------------------------------------------------
// Name: cbMousePositionChanged()
// Desc: 
//-----------------------------------------------------------------------------
void cbMousePositionChanged(int x, int y)
{
    static int s_LastMousePoxX = x;
    static int s_LastMousePoxY = y;

    if (g_MouseSceneRotationEnabled)
    {
        g_CameraRot[1] +=  0.9f*(x - s_LastMousePoxX);
        g_CameraRot[2] +=  0.9f*(y - s_LastMousePoxY);
        s_LastMousePoxX = x;
        s_LastMousePoxY = y;
    }
}
#endif


//-----------------------------------------------------------------------------
// Name: main()
// Desc: 
//-----------------------------------------------------------------------------
int main(int argc, char* argv[]) 
{
    return common_main(g_WindowWidth, g_WindowHeight,
                       "[PGR2] Offscreen Rendering",
                       cbInitGL,              // init GL callback function
                       cbDisplay,             // display callback function
                       cbWindowSizeChanged,   // window resize callback function
                       cbKeyboardChanged,     // keyboard callback function
#ifdef USE_ANTTWEAKBAR
                       NULL,                  // mouse button callback function
                       NULL                   // mouse motion callback function
#else
                       cbMouseButtonChanged,  // mouse button callback function
                       cbMousePositionChanged // mouse motion callback function
#endif
                       );
}
