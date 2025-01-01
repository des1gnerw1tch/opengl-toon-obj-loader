/* Compilation on Linux: 
 g++ -std=c++17 ./src/*.cpp -o prog -I ./include/ -I./../common/thirdparty/ -lSDL2 -ldl
*/

// Third Party Libraries
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> 

// C++ Standard Template Library (STL)
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>

// Our libraries
#include "GraphicsProgram.hpp"
#include "Camera.hpp"
#include "Geometry.hpp"
#include "DeltaTime.hpp"
#include "ObjModelLoader.hpp"
#include "PointLight.hpp"
#include "ShadowDirectionalLight.hpp"

void GraphicsProgram::GLClearAllErrors(){
    while(glGetError() != GL_NO_ERROR){ }
}


bool GraphicsProgram::GLCheckErrorStatus(const char* function, int line){
    while(GLenum error = glGetError()){
        std::cout << "OpenGL Error:" << error 
                  << "\tLine: " << line 
                  << "\tfunction: " << function << std::endl;
        return true;
    }
    return false;
}


std::string GraphicsProgram::LoadShaderAsString(const std::string& filename){
    // Resulting shader program loaded as a single string
    std::string result = "";

    std::string line = "";
    std::ifstream myFile(filename.c_str());

    if(myFile.is_open()){
        while(std::getline(myFile, line)){
            result += line + '\n';
        }
        myFile.close();

    }

    return result;
}


GLuint GraphicsProgram::CompileShader(GLuint type, const std::string& source){
	// Compile our shaders
	GLuint shaderObject;

	// Based on the type passed in, we create a shader object specifically for that
	// type.
	if(type == GL_VERTEX_SHADER){
		shaderObject = glCreateShader(GL_VERTEX_SHADER);
	}else if(type == GL_FRAGMENT_SHADER){
		shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	}

	const char* src = source.c_str();
	// The source of our shader
	glShaderSource(shaderObject, 1, &src, nullptr);
	// Now compile our shader
	glCompileShader(shaderObject);

	// Retrieve the result of our compilation
	int result;
	// Our goal with glGetShaderiv is to retrieve the compilation status
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &result);

	if(result == GL_FALSE){
		int length;
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
		char* errorMessages = new char[length]; // Could also use alloca here.
		glGetShaderInfoLog(shaderObject, length, &length, errorMessages);

		if(type == GL_VERTEX_SHADER){
			std::cout << "ERROR: GL_VERTEX_SHADER compilation failed!\n" << errorMessages << "\n";
		}else if(type == GL_FRAGMENT_SHADER){
			std::cout << "ERROR: GL_FRAGMENT_SHADER compilation failed!\n" << errorMessages << "\n";
		}
		// Reclaim our memory
		delete[] errorMessages;

		// Delete our broken shader
		glDeleteShader(shaderObject);

		return 0;
	}

  return shaderObject;
}


GLuint GraphicsProgram::CreateShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource){

    // Create a new program object
    GLuint programObject = glCreateProgram();

    // Compile our shaders
    GLuint myVertexShader   = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint myFragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    // Link our two shader programs together.
	// Consider this the equivalent of taking two .cpp files, and linking them into
	// one executable file.
    glAttachShader(programObject,myVertexShader);
    glAttachShader(programObject,myFragmentShader);
    glLinkProgram(programObject);

    // Validate our program
    glValidateProgram(programObject);

    // Once our final program Object has been created, we can
	// detach and then delete our individual shaders.
    glDetachShader(programObject,myVertexShader);
    glDetachShader(programObject,myFragmentShader);
	// Delete the individual shaders once we are done
    glDeleteShader(myVertexShader);
    glDeleteShader(myFragmentShader);

    return programObject;
}


void GraphicsProgram::CreateGraphicsPipeline(){
    std::string vertexShaderSourceLit;
    std::string fragmentShaderSourceLit;
    vertexShaderSourceLit      = LoadShaderAsString(m_vertexShaderSourceTextureLit);
    fragmentShaderSourceLit    = LoadShaderAsString(m_fragmentShaderSourceTextureLit);

    std::string vertexShaderSourceUnlit      = LoadShaderAsString(m_vertexShaderSourceUnlit);
    std::string fragmentShaderSourceUnlit   = LoadShaderAsString(m_fragmentShaderSourceUnlit);

	m_graphicsPipelineLit = CreateShaderProgram(vertexShaderSourceLit,fragmentShaderSourceLit);
    m_graphicsPipelineLights = CreateShaderProgram(vertexShaderSourceUnlit,fragmentShaderSourceUnlit);

    // Shadows
    std::string vertexShaderSourceShadow = LoadShaderAsString(m_shadowVertShader);
    std::string fragShaderSourceShadow = LoadShaderAsString(m_shadowFragShader);
    m_graphicsPipelineShadows = CreateShaderProgram(vertexShaderSourceShadow, fragShaderSourceShadow);

    // Outlines
    std::string vertexShaderSourceOutline = LoadShaderAsString(m_vertexShaderSourceOutlineOfObject);
    std::string fragShaderSourceOutline = LoadShaderAsString(m_fragmentShaderSourceOutlineOfObject);
    m_graphicsPipelineOutline = CreateShaderProgram(vertexShaderSourceOutline, fragShaderSourceOutline);
}


void GraphicsProgram::InitializeProgram(){
	// Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO)< 0){
		std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
		exit(1);
	}
	
	// Setup the OpenGL Context
	// Use OpenGL 4.1 core or greater
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
	// We want to request a double buffer for smooth updating.
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8); // Need this for outlines, stencil buffer

	// Create an application window using OpenGL that supports SDL
	gGraphicsApplicationWindow = SDL_CreateWindow( "zachwalkergames OpenGL OBJ Renderer",
													SDL_WINDOWPOS_UNDEFINED,
													SDL_WINDOWPOS_UNDEFINED,
													gScreenWidth,
													gScreenHeight,
													SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );

	// Check if Window did not create.
	if( gGraphicsApplicationWindow == nullptr ){
		std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << "\n";
		exit(1);
	}

	// Create an OpenGL Graphics Context
	gOpenGLContext = SDL_GL_CreateContext( gGraphicsApplicationWindow );
	if( gOpenGLContext == nullptr){
		std::cout << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << "\n";
		exit(1);
	}

	// Initialize GLAD Library
	if(!gladLoadGLLoader(SDL_GL_GetProcAddress)){
		std::cout << "glad did not initialize" << std::endl;
		exit(1);
	}
	
}

void GraphicsProgram::VertexSpecification(std::string modelPath){
    ObjModelLoader modelLoader(modelPath);
	// Vertex Arrays Object (VAO) Setup
	glGenVertexArrays(1, &m_vertexArrayObject);
	// We bind (i.e. select) to the Vertex Array Object (VAO) that we want to work withn.
	glBindVertexArray(m_vertexArrayObject);
	// Vertex Buffer Object (VBO) creation
	glGenBuffers(1, &m_vertexBufferObject);
    
    std::vector<GLfloat> vboData = modelLoader.GetVertexBufferObjectData();
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, 						        // Kind of buffer we are working with 
                                                                // (e.g. GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER)
                    vboData.size() * sizeof(GL_FLOAT), 	// Size of data in bytes
                    vboData.data(), 					// Raw array of data
                    GL_STATIC_DRAW);							// How we intend to use the data

    // Element Buffer Object (EBO) creation
    glGenBuffers(1, &m_elementBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBufferObject);
    std::vector<GLuint> eboData = modelLoader.GetElementBufferObjectData();
    std::cout << "Ebodata size: " << eboData.size() << std::endl;
    m_numVerticesToDraw = eboData.size();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, eboData.size() * sizeof(GLuint), eboData.data(), GL_STATIC_DRAW);

    // Texture Object Creation
    m_isTextureProvided = modelLoader.HasDiffuseTexture();
    if (m_isTextureProvided)    {
        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        glTexImage2D(GL_TEXTURE_2D,
                     0, // mipmap level
                     GL_RGB,
                     modelLoader.GetDiffuseTextureWidth(),
                     modelLoader.GetDiffuseTextureHeight(),
                     0, // border (must be 0 for some reason, this would be good to look up later)
                     GL_RGB,
                     GL_UNSIGNED_BYTE,
                     modelLoader.GetDiffuseTextureData().data());
        glGenerateMipmap(GL_TEXTURE_2D);
        // These parameters decide how to get the color value if the image is minimized/maximized, this is worth looking further into
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    // =============================
    // offsets every 3 floats
    // v     v     v
    // 
    // x,y,z,r,g,b,nx,ny,nz
    //
    // |------------------| strides is '9' floats
    //
    // ============================
    // Position information (x,y,z)
	glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,sizeof(GL_FLOAT)*11,(void*)0);
    // Color information (r,g,b)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,sizeof(GL_FLOAT)*11,(GLvoid*)(sizeof(GL_FLOAT)*3));
    // Normal information (nx,ny,nz)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,sizeof(GL_FLOAT)*11, (GLvoid*)(sizeof(GL_FLOAT)*6));
    // Texture information (tx,ty)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE,sizeof(GL_FLOAT)*11, (GLvoid*)(sizeof(GL_FLOAT)*9));

	// Unbind our currently bound Vertex Array Object
	glBindVertexArray(0);
	// Disable any attributes we opened in our Vertex Attribute Arrray,
	// as we do not want to leave them open. 
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
}

void GraphicsProgram::CreateLights() {
    glGenVertexArrays(1, &m_vertexArrayObjectLights);
    glBindVertexArray(m_vertexArrayObjectLights);
    glGenBuffers(1, &m_vertexBufferObjectLights);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObjectLights);
    PointLight l1(glm::vec3(1.0f, 3.0f, -3.f), glm::vec3(1.f, 1.f, 1.f), glm::vec3(1.f, 1.f, 1.f), glm::vec3(1.f, 1.f, 1.f), 0.f, 0.01f, 0.005f, 0.5f);
    m_lights.push_back(l1);
    std::vector<GLfloat> vboData = l1.GetVertexBufferObjectData();
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObjectLights);
    glBufferData(GL_ARRAY_BUFFER, 						        // Kind of buffer we are working with 
                                                                // (e.g. GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER)
                    vboData.size() * sizeof(GL_FLOAT), 	// Size of data in bytes
                    vboData.data(), 					// Raw array of data
                    GL_STATIC_DRAW);							// How we intend to use the data
    
    // Position information (x,y,z)
	glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,sizeof(GL_FLOAT)*6,(void*)0);
    // Color information (r,g,b)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,sizeof(GL_FLOAT)*6,(GLvoid*)(sizeof(GL_FLOAT)*3));

    // Element Buffer Object (EBO) creation
    glGenBuffers(1, &m_elementBufferObjectLights);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBufferObjectLights);
    std::vector<GLuint> eboData = l1.GetElementBufferObjectData();
    std::cout << "Ebodata size: " << eboData.size() << std::endl;
    m_numVerticesToDrawLights = eboData.size();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, eboData.size() * sizeof(GLuint), eboData.data(), GL_STATIC_DRAW);

	// Unbind our currently bound Vertex Array Object
	glBindVertexArray(0);
	// Disable any attributes we opened in our Vertex Attribute Arrray,
	// as we do not want to leave them open. 
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}


void GraphicsProgram::PreDraw(int graphicsPipeline, glm::mat4 viewMatrix, glm::mat4 projectionMatrix, 
                              glm::vec3 modelTranslation, glm::vec3 modelScale) {
	// Disable depth test and face culling.
    glEnable(GL_DEPTH_TEST);                    // NOTE: Need to enable DEPTH Test
    //glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // Set the polygon fill mode
    glPolygonMode(GL_FRONT_AND_BACK,gPolygonMode);

    // Initialize clear color
    // This is the background of the screen.
    glClearColor(0.0941176470588f, 0.0941176470588f, 0.215686f, 1.0f);

    // Use our shader
	glUseProgram(graphicsPipeline);

    // Model transformation by translating our object into world space
    glm::mat4 model = glm::scale(glm::mat4(1.0f), modelScale); 
    model = glm::translate(model, modelTranslation); 


    // Retrieve our location of our Model Matrix
    GLint u_ModelMatrixLocation = glGetUniformLocation(graphicsPipeline,"u_ModelMatrix");
    if(u_ModelMatrixLocation >=0){
        glUniformMatrix4fv(u_ModelMatrixLocation,1,GL_FALSE,&model[0][0]);
    }else{
        std::cout << "Could not find u_ModelMatrix, maybe a mispelling?\n";
        exit(EXIT_FAILURE);
    }


    // Update the View Matrix
    GLint u_ViewMatrixLocation = glGetUniformLocation(graphicsPipeline,"u_ViewMatrix");
    if(u_ViewMatrixLocation>=0){
        glUniformMatrix4fv(u_ViewMatrixLocation,1,GL_FALSE,&viewMatrix[0][0]);
    }else{
        std::cout << "Could not find u_ViewMatrix, maybe a mispelling?\n";
        exit(EXIT_FAILURE);
    }

    // Retrieve our location of our perspective matrix uniform 
    GLint u_ProjectionLocation= glGetUniformLocation(graphicsPipeline,"u_Projection");
    if(u_ProjectionLocation>=0){
        glUniformMatrix4fv(u_ProjectionLocation,1,GL_FALSE,&projectionMatrix[0][0]);
    }else{
        std::cout << "Could not find u_Projection, maybe a mispelling?\n";
        exit(EXIT_FAILURE);
    }
}

void GraphicsProgram::SetLightingUniforms(int graphicsPipeline, std::array<u_int8_t, 3> colorOfShadowDots, int numDotsHorizontally) {
    GLint positionLoc = glGetUniformLocation(graphicsPipeline, "u_Light.worldPosition");
    glUniform3fv(positionLoc, 1, m_lights[0].GetPositionForUniform());
    GLint ambientColorLoc = glGetUniformLocation(graphicsPipeline, "u_Light.ambientColor");
    glUniform3fv(ambientColorLoc, 1, m_lights[0].GetAmbientColorForUniform());
    GLint ambientStrengthLoc = glGetUniformLocation(graphicsPipeline, "u_Light.ambientStrength");
    glUniform1f(ambientStrengthLoc, m_lights[0].GetAmbientStrengthForUniform());
    GLint diffuseColorLoc = glGetUniformLocation(graphicsPipeline, "u_Light.diffuseColor");
    glUniform3fv(diffuseColorLoc, 1, m_lights[0].GetDiffuseColorForUniform());
    GLint specularColorLoc = glGetUniformLocation(graphicsPipeline, "u_Light.specularColor");
    glUniform3fv(specularColorLoc, 1, m_lights[0].GetSpecularColorForUniform());
    GLint constantFallOffLoc = glGetUniformLocation(graphicsPipeline, "u_Light.constantFallOff");
    glUniform1f(constantFallOffLoc, m_lights[0].GetConstantFallOffForUniform());
    GLint linearFallOffLoc = glGetUniformLocation(graphicsPipeline, "u_Light.linearFallOff");
    glUniform1f(linearFallOffLoc, m_lights[0].GetLinearFallOffForUniform());
    GLint quadtraticFallOffLoc = glGetUniformLocation(graphicsPipeline, "u_Light.quadtraticFallOff");
    glUniform1f(quadtraticFallOffLoc, m_lights[0].GetQuadtraticFallOffForUniform());

    GLint cameraWorldPositionLoc = glGetUniformLocation(graphicsPipeline, "u_CameraWorldPosition");
    glUniform3f(cameraWorldPositionLoc, gCamera.GetEyeXPosition(), gCamera.GetEyeYPosition(), gCamera.GetEyeZPosition());

    GLint dotColorLoc = glGetUniformLocation(graphicsPipeline, "u_dotColor");
    glUniform3f(dotColorLoc, colorOfShadowDots[0] / 255.0f, colorOfShadowDots[1] / 255.0f, colorOfShadowDots[2] / 255.0f);

    GLint numDotsHorizontallyLoc = glGetUniformLocation(graphicsPipeline, "u_numDotsHorizontally");
    glUniform1i(numDotsHorizontallyLoc, numDotsHorizontally);
}

// Note: Not used in halftoon shader
void GraphicsProgram::SetTextureUniforms(int graphicsPipeline)  {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    GLint sampler = glGetUniformLocation(graphicsPipeline, "u_TextureSampler");
    glUniform1i(sampler, 0);
}

void GraphicsProgram::SetShadowUniforms(int graphicsPipeline, ShadowDirectionalLight shadowCaster)  {
    glActiveTexture(shadowCaster.GetTexUnit());
    GLint u_ShadowLightSpaceMatrixLoc = glGetUniformLocation(graphicsPipeline,"u_ShadowLightSpaceMatrix");
    if(u_ShadowLightSpaceMatrixLoc >=0){
        glUniformMatrix4fv(u_ShadowLightSpaceMatrixLoc,1,GL_FALSE,&(shadowCaster.GetWorldToLightSpaceTransform()[0][0]));
    }else{
        std::cout << "Could not find u_ShadowLightSpaceMatrix, maybe a mispelling?\n";
        exit(EXIT_FAILURE);
    }

    // Something about this block is causing the objects to not render
    
    GLint depthMapLoc = glGetUniformLocation(graphicsPipeline, "u_DepthMap");
    if (depthMapLoc >=0)    {
        glUniform1i(depthMapLoc, 1); // TODO: Change this to shadowCaster.getTexUnit - GL_TEXTURE0
    } else {
        std::cout << "Could not find u_DepthMap uniform, maybe a mispelling\n";
        exit(EXIT_FAILURE);
    }
    
}

void GraphicsProgram::SetOutlineUniforms(int graphicsPipeline, float outlineExtrudeDistance) {
    GLint outlineExtrudeDistanceLoc = glGetUniformLocation(graphicsPipeline, "u_OutlineExtrudeDistance");
    glUniform1f(outlineExtrudeDistanceLoc, outlineExtrudeDistance);
}

void GraphicsProgram::DrawLit(){
    // Enable our attributes
	glBindVertexArray(m_vertexArrayObject);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBufferObject);
    //std::cout << "Number of vertices to draw: " << m_numVerticesToDraw << std::endl;
    //Render data
    glDrawElements(GL_TRIANGLES, m_numVerticesToDraw, GL_UNSIGNED_INT, nullptr);

	// Stop using our current graphics pipeline
	// Note: This is not necessary if we only have one graphics pipeline.
    glUseProgram(0);
}

void GraphicsProgram::DrawLights()  {
    // Enable our attributes
	glBindVertexArray(m_vertexArrayObjectLights);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBufferObjectLights);
    //std::cout << "Number of vertices to draw: " << m_numVerticesToDraw << std::endl;
    //Render data
    glDrawElements(GL_TRIANGLES, m_numVerticesToDrawLights, GL_UNSIGNED_INT, nullptr);

	// Stop using our current graphics pipeline
	// Note: This is not necessary if we only have one graphics pipeline.
    glUseProgram(0);
}

// TODO: epochTime needs milliseconds as well, right now only updating once a frame
void GraphicsProgram::RotateLights()    {
    const double PI = 3.141592653589793; 
    const double rotationRadius = 9.0;
    long long epochTime = deltaTime.GetSystemTimeMs();
    epochTime -= 1730246494422;
    double rad = epochTime * (PI / 180.0);
    rad /= 20.0; //Changing speed
    double x = cos(rad) * rotationRadius;
    double z = sin(rad) * rotationRadius;
    
    for (PointLight& l : m_lights)  {
        glm::vec3 curPos = l.GetPosition();
        glm::vec3 newPos = glm::vec3(x, curPos.y, z);
        l.SetPosition(newPos);
    }
}

void GraphicsProgram::getOpenGLVersionInfo(){
  std::cout << "Vendor: " << glGetString(GL_VENDOR) << "\n";
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";
  std::cout << "Version: " << glGetString(GL_VERSION) << "\n";
  std::cout << "Shading language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
}


void GraphicsProgram::Input(){
	// Event handler that handles various events in SDL
	// that are related to input and output
	SDL_Event e;
	//Handle events on queue
	while(SDL_PollEvent( &e ) != 0){
		// If users posts an event to quit
		// An example is hitting the "x" in the corner of the window.
		if(e.type == SDL_QUIT){
			std::cout << "Goodbye! (Leaving MainApplicationLoop())" << std::endl;
			gQuit = true;
		}
        if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE){
			std::cout << "ESC: Goodbye! (Leaving MainApplicationLoop())" << std::endl;
            gQuit = true;
        }
        if(e.type==SDL_MOUSEMOTION){
            gCamera.MouseLook(e.motion.xrel * m_lookSpeed, 
                              e.motion.yrel * m_lookSpeed);
        }
	}

    // Retrieve keyboard state
    const Uint8 *state = SDL_GetKeyboardState(NULL);

    // Camera
    // Update our position of the camera
    if (state[SDL_SCANCODE_W]) {
        gCamera.MoveForward(movementSpeed * deltaTime.GetDeltaTime());
    }
    if (state[SDL_SCANCODE_S]) {
        gCamera.MoveBackward(movementSpeed * deltaTime.GetDeltaTime());
    }
    if (state[SDL_SCANCODE_A]) {
        gCamera.MoveLeft(movementSpeed * deltaTime.GetDeltaTime());
    }
    if (state[SDL_SCANCODE_D]) {
        gCamera.MoveRight(movementSpeed * deltaTime.GetDeltaTime());
    }
    
    if (state[SDL_SCANCODE_TAB]) {
        SDL_Delay(250); // This is hacky in the name of simplicity,
                       // but we just delay the
                       // system by a few milli-seconds to process the 
                       // keyboard input once at a time.
        if(gPolygonMode== GL_FILL){
            gPolygonMode = GL_LINE;
        }else{
            gPolygonMode = GL_FILL;
        }
        std::cout << "Changed polygon mode" << std::endl;
    }
}


void GraphicsProgram::MainLoop(){

    // Little trick to map mouse to center of screen always.
    // Useful for handling 'mouselook'
    // This works because we effectively 're-center' our mouse at the start
    // of every frame prior to detecting any mouse motion.
    SDL_WarpMouseInWindow(gGraphicsApplicationWindow,gScreenWidth/2,gScreenHeight/2);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    Plane plane(20.0f, {100, 100, 100});
    // Position is behind origin for now, in future can make this position equal to the position of the light to emulate shadow from light
    ShadowDirectionalLight shadowCaster(GL_TEXTURE1, glm::vec3(0, 0.3, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), 0.1f, 30.0f);

    // Chalice
    /*glm::vec3 objFileTranslation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 objFileScale = glm::vec3(6.0f, 6.0f, 6.0f);
    float objFileOutlineExtrudeDistance = 0.02f;*/

    // Heaven cat
    glm::vec3 objFileTranslation = glm::vec3(0.0f, -0.09f, 0.0f);
    glm::vec3 objFileScale = glm::vec3(3.0f, 3.0f, 3.0f);
    float objFileOutlineExtrudeDistance = 0.02f;

    // Astronaut
    //glm::vec3 objFileTranslation = glm::vec3(0.0f, -0.026f, 0.0f);
    //glm::vec3 objFileScale = glm::vec3(30.0f, 30.0f, 30.0f);

    glm::vec3 planeTranslation = glm::vec3(0.0f, 0.3f, 10.0f);
    glm::vec3 planeScale = glm::vec3(1.0f, 1.0f, 1.0f);

    Plane plane1(2.0f, {255, 0, 0}); // Floating plane to test shadow
    glm::vec3 planeTranslation1 = glm::vec3(3.0f, 0.8f, 4.0f);
    glm::vec3 planeScale1 = glm::vec3(1.0f, 1.0f, 1.0f);

	// While application is running
	while(!gQuit){
        deltaTime.Update();

		// Handle Input
		Input();

        // SHADOW MAP PASS
        shadowCaster.ActivateTexUnitAndBindFBO();
        glClear(GL_DEPTH_BUFFER_BIT); // TODO: May not need this as already clear at end of loop, check this later
        glViewport(0, 0, shadowCaster.GetShadowMapWidth(), shadowCaster.GetShadowMapHeight());
        PreDraw(m_graphicsPipelineShadows, shadowCaster.GetViewMatrix(), shadowCaster.GetProjectionMatrix(), objFileTranslation, objFileScale);
        DrawLit();
        PreDraw(m_graphicsPipelineShadows, shadowCaster.GetViewMatrix(), shadowCaster.GetProjectionMatrix(), planeTranslation, planeScale);
        plane.Draw();
        //PreDraw(m_graphicsPipelineShadows, shadowCaster.GetViewMatrix(), shadowCaster.GetProjectionMatrix(), planeTranslation1, planeScale1);
        //plane1.Draw();


        // REAL RENDERING PASS
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear buffer from shadow pass
        glViewport(0, 0, gScreenWidth, gScreenHeight);
        // Projection matrix (in perspective) 
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f),
                                             (float)gScreenWidth/(float)gScreenHeight,
                                             0.1f,
                                             20.0f);
        // DRAW LIGHT
        RotateLights();
        PreDraw(m_graphicsPipelineLights, gCamera.GetViewMatrix(), projectionMatrix, m_lights[0].GetPosition());
        DrawLights();

        // DRAW MODEL
		// Setup anything (i.e. OpenGL State) that needs to take
		// place before draw calls
		PreDraw(m_graphicsPipelineLit, gCamera.GetViewMatrix(), projectionMatrix, objFileTranslation, objFileScale);
        shadowCaster.SetEyePosition(glm::vec3(m_lights[0].GetPosition()));
        SetShadowUniforms(m_graphicsPipelineLit, shadowCaster);

        // Gold Chalice
        SetLightingUniforms(m_graphicsPipelineLit, {20, 20, 20}, 75);
        // Astronaut
        // SetLightingUniforms(m_graphicsPipelineLit, {200, 200, 200}, 75);

        if (m_isTextureProvided)    {
            SetTextureUniforms(m_graphicsPipelineLit);
        }

        // Enable stencil test to outline object
        glEnable(GL_STENCIL_TEST);
        glStencilMask(0xFF); // enable writing to all parts of the stencil buffer
        glClear(GL_STENCIL_BUFFER_BIT); // glStencilMask must be 0xFF to write the stencil buffer back to 0
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // Place 1's for fragments that pass the stencil and depth test
        glStencilFunc(GL_ALWAYS, 1, 0xFF); // all fragments should pass the stencil test. The only thing we care about for this pass is the depth test.
		DrawLit();
        //PrintStencilBuffer(gScreenWidth, gScreenHeight);


        // Draw outline of object
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00); // disable writing to the stencil buffer
        PreDraw(m_graphicsPipelineOutline, gCamera.GetViewMatrix(), projectionMatrix, objFileTranslation, objFileScale);
        SetOutlineUniforms(m_graphicsPipelineOutline, objFileOutlineExtrudeDistance);
        //glDisable(GL_DEPTH_TEST); // Don't think I need this, I want it to be hidden behind things
        DrawLit(); // Not actually lit, just draws the model. Should be renamed to DrawModel()
        glDisable(GL_STENCIL_TEST);

         // DRAW PLANE
        PreDraw(m_graphicsPipelineLit, gCamera.GetViewMatrix(), projectionMatrix, planeTranslation, planeScale);
        SetLightingUniforms(m_graphicsPipelineLit, {40, 40, 40}, 120);
        plane.Draw();
        //PreDraw(m_graphicsPipelineLit, gCamera.GetViewMatrix(), projectionMatrix, planeTranslation1, planeScale1);
        //SetLightingUniforms(m_graphicsPipelineLit, {100, 0, 0}, 20);
        //plane1.Draw();

		//Update screen of our specified window
		SDL_GL_SwapWindow(gGraphicsApplicationWindow);

        //Clear color buffer and Depth Buffer
  	    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	}
}

void GraphicsProgram::PrintStencilBuffer(int width, int height) {
    std::cout << std::endl << std::endl << "Printing new stencil buffer " << std::endl;
    // Allocate memory to store stencil buffer values
    std::vector<GLubyte> stencilData(width * height);

    // Read the stencil buffer
    glReadPixels(0, 0, width, height, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, stencilData.data());

    // Print stencil buffer values
    std::cout << "Stencil Buffer Values:\n";
    for (int y = height - 1; y >= 0; --y) { // Start from top row (OpenGL's origin is bottom-left)
        for (int x = 0; x < width; ++x) {
            std::cout << static_cast<int>(stencilData[y * width + x]) << " ";
        }
        std::cout << "\n";
    }
}


void GraphicsProgram::CleanUp(){
	//Destroy our SDL2 Window
	SDL_DestroyWindow(gGraphicsApplicationWindow );
	gGraphicsApplicationWindow = nullptr;

    // Delete our OpenGL Objects
    glDeleteBuffers(1, &m_vertexBufferObject);
    glDeleteBuffers(1, &m_elementBufferObject);
    glDeleteVertexArrays(1, &m_vertexArrayObject);

	// Delete our Graphics pipeline
    glDeleteProgram(m_graphicsPipelineLit);
    glDeleteProgram(m_graphicsPipelineLights);

	//Quit SDL subsystems
	SDL_Quit();
}


void GraphicsProgram::Start(std::string modelPath){
    std::cout << "Use w s a and d keys to move forward and back\n";
    std::cout << "Use tab to toggle wireframe\n";
    std::cout << "Press ESC to quit\n";

	// 1. Setup the graphics program
	InitializeProgram();
	
	// 2. Setup our geometry
	VertexSpecification(modelPath);
    CreateLights();
	
	// 3. Create our graphics pipeline
	// 	- At a minimum, this means the vertex and fragment shader
	CreateGraphicsPipeline();
	
	// 4. Call the main application loop
	MainLoop();	

	// 5. Call the cleanup function when our program terminates
	CleanUp();
}
