#pragma once

#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <string>

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> 

#include "Camera.hpp"
#include "DeltaTime.hpp"
#include "Geometry.hpp"
#include "PointLight.hpp"
#include "Plane.hpp"
#include "ShadowDirectionalLight.hpp"


class GraphicsProgram {
    private:
        const int gScreenWidth = 640 * 2;
        const int gScreenHeight = 480 * 2;
        const std::string m_vertexShaderSourceUnlit      =  "./shaders/unlit_vert.glsl"; // Displays unlit objects, such as light meshes
        const std::string m_fragmentShaderSourceUnlit   = "./shaders/unlit_frag.glsl";
        const std::string m_vertexShaderSourceTextureLit = "./shaders/texture_phong_vert.glsl";
        const std::string m_fragmentShaderSourceTextureLit = "./shaders/halftone_toon.glsl";
        const std::string m_vertexShaderSourceOutlineOfObject = "./shaders/outline_vert.glsl";
        const std::string m_fragmentShaderSourceOutlineOfObject = "./shaders/single_color_frag.glsl";

        SDL_Window* gGraphicsApplicationWindow 	= nullptr;
        SDL_GLContext gOpenGLContext			= nullptr;
        GLuint m_graphicsPipelineLit	= 0; // Displays lit objects
        GLuint m_graphicsPipelineLights = 0; // Displays lights themselves
        GLuint m_graphicsPipelineOutline = 0; // Displays outline of objects

        GLuint m_graphicsPipelineShadows = 0; // Graphics pipeline for shadow pass
        const std::string m_shadowVertShader = "./shaders/shadow_pass_vert.glsl";
        const std::string m_shadowFragShader = "./shaders/shadow_pass_frag.glsl";

        // Main loop flag
        bool gQuit = false; // If this is quit = 'true' then the program terminates.

        // shader
        // The following stores the a unique id for the graphics pipeline
        // program object that will be used for our OpenGL draw calls.

        // OpenGL Objects
        // Vertex Array Object (VAO)
        // Vertex array objects encapsulate all of the items needed to render an object.
        // For example, we may have multiple vertex buffer objects (VBO) related to rendering one
        // object. The VAO allows us to setup the OpenGL state to render that object using the
        // correct layout and correct buffers with one call after being setup.
        GLuint m_vertexArrayObject= 0;
        // Vertex Buffer Object (VBO)
        // Vertex Buffer Objects store information relating to vertices (e.g. positions, normals, textures)
        // VBOs are our mechanism for arranging geometry on the GPU.
        GLuint  m_vertexBufferObject            = 0;

        // Element Buffer Object (EBO)
        GLuint  m_elementBufferObject            = 0;

        GLuint m_vertexArrayObjectLights = 0;
        GLuint m_vertexBufferObjectLights = 0;
        GLuint m_elementBufferObjectLights = 0;
        std::vector<PointLight> m_lights;

        GLuint m_numVerticesToDraw = 0;
        GLuint m_numVerticesToDrawLights = 0;

        // Camera
        Camera gCamera;

        // Draw fill mode default
        GLenum gPolygonMode = GL_FILL;
        DeltaTime deltaTime;
        const float movementSpeed = 1.0f;
        const float m_lookSpeed = 0.7f;

        // Textures
        GLuint m_textureID = 0;
        bool m_isTextureProvided = false;
        
        void GLClearAllErrors();

        // Returns true if we have an error
        bool GLCheckErrorStatus(const char* function, int line);

        #define GLCheck(x) GLClearAllErrors(); x; GLCheckErrorStatus(#x,__LINE__);

        /**
        * LoadShaderAsString takes a filepath as an argument and will read line by line a file and return a string that is meant to be compiled at runtime for a vertex, fragment, geometry, tesselation, or compute shader.
        * e.g.
        *       LoadShaderAsString("./shaders/filepath");
        *
        * @param filename Path to the shader file
        * @return Entire file stored as a single string 
        */
        std::string LoadShaderAsString(const std::string& filename);

        /**
        * CompileShader will compile any valid vertex, fragment, geometry, tesselation, or compute shader.
        * e.g.
        *	    Compile a vertex shader: 	CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
        *       Compile a fragment shader: 	CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
        *
        * @param type We use the 'type' field to determine which shader we are going to compile.
        * @param source : The shader source code.
        * @return id of the shaderObject
        */
        GLuint CompileShader(GLuint type, const std::string& source);

        /**
        * Creates a graphics program object (i.e. graphics pipeline) with a Vertex Shader and a Fragment Shader
        *
        * @param vertexShaderSource Vertex source code as a string
        * @param fragmentShaderSource Fragment shader source code as a string
        * @return id of the program Object
        */
        GLuint CreateShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);

        /**
        * Create the graphics pipeline
        *
        * @return void
        */
        void CreateGraphicsPipeline();

        /**
        * Initialization of the graphics application. Typically this will involve setting up a window
        * and the OpenGL Context (with the appropriate version)
        *
        * @return void
        */
        void InitializeProgram();

        /**
        * Setup your geometry during the vertex specification step
        *
        * @return void
        */
        void VertexSpecification(std::string modelPath);

        void CreateLights();

        /**
        * PreDraw
        * Typically we will use this for setting some sort of 'state'
        * Note: some of the calls may take place at different stages (post-processing) of the
        * 		 pipeline.
        * @return void
        */
        void PreDraw(int graphicsPipeline,
                     glm::mat4 viewMatrix, 
                     glm::mat4 projectionMatrix,
                     glm::vec3 modelTranslation = glm::vec3(0.0f, 0.0f, 0.0f), 
                     glm::vec3 modelScale = glm::vec3(1.0f, 1.0f, 1.0f));

        /**
         * Sets the lighting uniforms for Phong shading. Need to use glUseProgram before calling, handled in PreDraw.
         */
        void SetLightingUniforms(int graphicsPipeline, std::array<u_int8_t, 3> colorOfShadowDots, int numDotsHorizontally);

        void SetShadowUniforms(int graphicsPipeline, ShadowDirectionalLight shadowCaster);

        void SetOutlineUniforms(int graphicsPipeline, float outlineExtrudeDistance);

        /**
         * Sets the uniforms in shader that uses textures. Need to use glUseProgram before calling, handled in PreDraw.
         */
        void SetTextureUniforms(int graphicsPipeline);

        /**
        * Draw
        * The render function gets called once per loop.
        * Typically this includes 'glDraw' related calls, and the relevant setup of buffers
        * for those calls.
        *
        * @return void
        */
        void DrawLit();

        void DrawLights();

        void RotateLights();

        /**
        * Helper Function to get OpenGL Version Information
        *
        * @return void
        */
        void getOpenGLVersionInfo();

        /**
        * Function called in the Main application loop to handle user input
        *
        * @return void
        */
        void Input();

        /**
        * Main Application Loop
        * This is an infinite loop in our graphics application
        *
        * @return void
        */
        void MainLoop();

        /**
        * The last function called in the program
        * This functions responsibility is to destroy any global
        * objects in which we have create dmemory.
        *
        * @return void
        */
        void CleanUp();

        // Handy function from GPT
        void PrintStencilBuffer(int width, int height);

    public:
        /**
         * This starts the graphics program and runs the loop
         * 
         * @return void
         */
        void Start(std::string modelToRender);
};