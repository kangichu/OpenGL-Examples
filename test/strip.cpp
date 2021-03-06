//========================================================================
// This is a small test application which builds off of quad.cpp
// To define a strip of triangles instead of a list of disconnected
// triangles.
//========================================================================

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include "GL/glfw.h"
#include "glm/glm.hpp"

#include "GLSLProgram.hpp"
#include "glUtil.hpp"

int main( void )
{
    int width, height, x;
//    double t;

    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        exit( EXIT_FAILURE );
    }
    
    glewExperimental = GL_TRUE;

    // We need this to get the code to compile+run on MacOSX. I have yet
    // to confirm if this works on Linux...

#ifdef __APPLE__
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif


    // Open a window and create its OpenGL context
    if( !glfwOpenWindow( 640, 480, 8,8,8,8,24,8, GLFW_WINDOW ) )
    {
        fprintf( stderr, "Failed to open GLFW window\n" );

        glfwTerminate();
        exit( EXIT_FAILURE );
    }

    // Initialize GLEW
    GLenum err = glewInit();
    if( err != GLEW_OK )
    {
        fprintf( stderr, "Failed to initialize GLEW: %s\n",
        				 glewGetErrorString(err));
        exit( EXIT_FAILURE );
    }

    printGLVersion();

    fprintf( stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	if (!GLEW_ARB_vertex_buffer_object)
	{
        fprintf( stderr, "VBO not supported\n");
		exit( EXIT_FAILURE );
	}

    glfwSetWindowTitle( "Spinning Triangle" );

    // Ensure we can capture the escape key being pressed below
    glfwEnable( GLFW_STICKY_KEYS );

    // Enable vertical sync (on cards that support it)
    glfwSwapInterval( 1 );

    shader::GLSLProgram prog;

    // Compile vertex shader
    if( ! prog.compileShaderFromFile("shaders/basic.vert", shader::VERTEX))
    {
		printf("Vertex shader failed to compile!\n%s", prog.log().c_str());
		exit(1);
	}

    // Compile fragment shader
    if( ! prog.compileShaderFromFile("shaders/basic.frag", shader::FRAGMENT))
    {
		printf("Fragment shader failed to compile!\n%s", prog.log().c_str());
		exit(1);
	}

    // Link shaders
    if( ! prog.link() )
    {
    	printf("Shader program failed to link!\n%s", prog.log().c_str());
    	exit(1);
    }

    GLint pLoc = prog.getAttribLocation("VertexPosition");
    GLint cLoc = prog.getAttribLocation("VertexColor");

    GLuint vaoHandle;
    GLfloat positionData[] = {
    		-0.2f, -0.2f, 0.0f,
    		-0.6f, -0.2f, 0.0f,
    		-0.2f,  0.2f, 0.0f,
    		-0.6f,  0.2f, 0.0f,

    		// These are two throw-away triangles that will generate the zero
    		// area triangles between the two quads
    		-0.6f,  0.2f, 0.0f,
    		 0.6f, -0.2f, 0.0f,

    		 0.6f, -0.2f, 0.0f,
    		 0.2f, -0.2f, 0.0f,
    		 0.6f,  0.2f, 0.0f,
    		 0.2f,  0.2f, 0.0f
    };

    GLfloat colorData[] = {
    		0.0f, 1.0f, 0.0f,
    		1.0f, 0.0f, 0.0f,
    		0.0f, 0.0f, 1.0f,
    		1.0f, 1.0f, 0.0f,

    		1.0f, 1.0f, 0.0f,
    		0.0f, 1.0f, 0.0f,

    		0.0f, 1.0f, 0.0f,
    		1.0f, 0.0f, 0.0f,
    		0.0f, 0.0f, 1.0f,
    		1.0f, 1.0f, 0.0f
    };

    GLuint vboHandles[2];
    glGenBuffers(2, vboHandles);
    GLuint positionBufferHandle = vboHandles[0];
    GLuint colorBufferHandle = vboHandles[1];

    // Populate position buffer
    glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, 10 * 3 * sizeof(GLfloat), positionData,
    			 GL_STATIC_DRAW);

    // Populate color buffer
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, 10 * 3 * sizeof(GLfloat), colorData,
    			 GL_STATIC_DRAW);

    // Create and set-up array object
    glGenVertexArrays( 1, &vaoHandle );
    glBindVertexArray(vaoHandle);

    // Enable vertex attribute arrays
    glEnableVertexAttribArray(0); // Vertex position
    glEnableVertexAttribArray(1); // Vertex color

    // Map index 0 to the position buffer
    glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
    glVertexAttribPointer( pLoc, 3, GL_FLOAT, GL_FALSE, 0,
    					  (GLubyte *)NULL );

    // Map index 1 to the position buffer
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
    glVertexAttribPointer( cLoc, 3, GL_FLOAT, GL_FALSE, 0,
    					  (GLubyte *)NULL );

    do
    {
//        t = glfwGetTime();
//		  float r = 0.3f*(GLfloat)x + (GLfloat)t*100.0f
        glfwGetMousePos( &x, NULL );

        // Get window size (may be different than the requested size)
        glfwGetWindowSize( &width, &height );

        // Special case: avoid division by zero below
        height = height > 0 ? height : 1;

        prog.use();
        glBindVertexArray(vaoHandle);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 10);

        // Swap buffers
        glfwSwapBuffers();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
           glfwGetWindowParam( GLFW_OPENED ) );

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    exit( EXIT_SUCCESS );
}
