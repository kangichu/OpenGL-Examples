//========================================================================
// This is a small test application for GLFW.
// The program opens a window (640x480), and renders a spinning colored
// triangle (it is controlled with both the GLFW timer and the mouse).
//========================================================================

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include "GL/glfw.h"
#include "glm/glm.hpp"

#include "glslHelper.hpp"
#include "imageUtil.hpp"
#include "glUtil.hpp"

#define BUFFER_OFFSET(i) ((GLfloat*)NULL + (i))

typedef struct CVertex
{
	GLfloat pos[3];
	GLfloat uv[2];
} CVertex;

int main( void )
{
    int width, height, x;

    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        exit( EXIT_FAILURE );
    }


    // Open a window and create its OpenGL context
    if( !glfwOpenWindow( 640, 480, 0,0,0,0, 0,0, GLFW_WINDOW ) )
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

    GLuint basicProgram = createGLSLProgram("shaders/basictexture");

    GLint pLoc =  glGetAttribLocation(basicProgram, "VertexPosition");
    GLint uvLoc = glGetAttribLocation(basicProgram, "VertexTexCoord");

    // Set the sampler to use texture 0
    GLint texLoc = glGetUniformLocation(basicProgram, "Tex1");
    if( texLoc >= 0 ) {
    	glUniform1i(texLoc, 0);
    }else{
    	fprintf(stderr, "Uniform variable Tex1 not found!\n");
    }

    GLuint vaoHandle;
    CVertex packedData[] = {
    		{{ 0.8f, -0.8f, 0.0f},
    		 { 1.0f,  0.0f}},

    		{{-0.8f, -0.8f, 0.0f},
    		 { 0.0f,  0.0f}},

    		{{ 0.8f,  0.8f, 0.0f},
    		 { 1.0f,  1.0f}},

    		{{-0.8f,  0.8f, 0.0f},
    		 { 0.0f,  1.0f}}
    };

    GLuint vboHandle;
    glGenBuffers(1, &vboHandle);
    GLuint dataBufferHandle = vboHandle;;

    // Populate position buffer
    glBindBuffer(GL_ARRAY_BUFFER, dataBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(CVertex), packedData,
    			 GL_STATIC_DRAW);


    // Create and set-up array object
    glGenVertexArrays( 1, &vaoHandle );
    glBindVertexArray(vaoHandle);

    // Enable vertex attribute arrays
    glEnableVertexAttribArray(pLoc); // Vertex position
    glEnableVertexAttribArray(uvLoc); // Vertex color

    // Map index 0 to the position buffer
    glBindBuffer(GL_ARRAY_BUFFER, dataBufferHandle);

    glVertexAttribPointer( pLoc, 3, GL_FLOAT, GL_FALSE, sizeof(CVertex), BUFFER_OFFSET(0) );
    glVertexAttribPointer( uvLoc, 2, GL_FLOAT, GL_FALSE, sizeof(CVertex), BUFFER_OFFSET(3) );

    // REMEMBER THESE!!!
    ilInit();
    iluInit();
    ilutRenderer(ILUT_OPENGL);

    GLuint texID = util::image::loadImage("/home/cgibson/Projects/OpenGL-Examples/img/random.png");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);

    do
    {
        glfwGetMousePos( &x, NULL );

        // Get window size (may be different than the requested size)
        glfwGetWindowSize( &width, &height );

        // Special case: avoid division by zero below
        height = height > 0 ? height : 1;

        glUseProgram( basicProgram );
        glBindVertexArray(vaoHandle);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Swap buffers
        glfwSwapBuffers();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
           glfwGetWindowParam( GLFW_OPENED ) );

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    exit( EXIT_SUCCESS );
}
