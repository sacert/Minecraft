// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW. Always include it before gl.h and glfw.h, since it's a bit magic.
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

#include "shader.hpp"

GLuint gVAO = 0;
GLuint gVBO = 0;
GLuint gElements = 0;
GLFWwindow* window;
GLint mvpMatrix;

glm::mat4 buildMatrix()
 {
 	glm::mat4 projection = glm::perspective(
 		45.0f,		// Field of view
 		4.0f / 3.0f, // Aspect ratio
 		0.1f,		// Near clipping plane
 		100.0f		// Far clipping plane
 	);
 
 	glm::mat4 view = glm::lookAt(
 		glm::vec3(4, 3, 3),	// Camera location in world coordinates
 		glm::vec3(0, 0, 0),	// Looking at the origin
 		glm::vec3(0, 1, 0)	// Camera up vector
 	);
 
 	glm::mat4 model = glm::mat4(1.0f);
 
 	return projection * view * model;
 }

void LoadCube(GLuint program) {
    // Vertex Array Object
	glGenVertexArrays(1, &gVAO);
	glBindVertexArray(gVAO);

    // Vertex Buffer Object
    glGenBuffers(1, &gVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gVBO);

    // An array of 3 vectors which represents 3 vertices
    static const GLfloat g_vertex_buffer_data[] = {
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
    };

    static const GLubyte elements[][3] =
    {
        {7, 6, 4},
        {1, 7, 5},
        {2, 7, 3},
        {1, 3, 7},
        {7, 4, 5},
        {2, 6, 7},
        {4, 6, 2},
        {0, 3, 1},
        {3, 0, 2},
        {0, 1, 5},
        {0, 5, 4},
        {0, 4, 2},
    };

    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    // Create an element buffer and send the element data
 	glGenBuffers(1, &gElements);
 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gElements);
 	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    GLuint index = glGetAttribLocation(program, "vert");
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    // unbind VAO and VBO 
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// draws a single frame
void Render(GLuint program) {
    // clear everything
    glClearColor(0, 0, 0, 1); // black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // bind the program (the shaders)
    glUseProgram(program);

    // projection * view * model
    glm::mat4 mvp = buildMatrix();
 	glUniformMatrix4fv(
 		mvpMatrix,	// Id of this uniform variable
 		1,			// Number of matrices
 		GL_FALSE,	// Transpose
 		&mvp[0][0]	// The location of the data
 	);
        
    // bind the VAO (the triangle)
    glBindVertexArray(gVAO);

    // draw the elements 
    glDrawElements(
 	    GL_TRIANGLES,  		// mode
 	    3 * 12,             // count
  	    GL_UNSIGNED_BYTE,   // type
  	    nullptr             // element array buffer offset
  	);
    
    // unbind the VAO
    glBindVertexArray(0);
    
    // unbind the program
    glUseProgram(0);
    
    // swap the display buffers (displays what was just drawn)
    glfwSwapBuffers(window);
}

int main() {

    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 800, 600, "Minecraft", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // Initialize GLEW
    glewExperimental=1; // Needed in core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    GLuint program = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );

    // Get the id for the uniform variable "mvpMatrix"
 	mvpMatrix = glGetUniformLocation(program, "mvpMatrix");

    LoadCube(program);

    do {

        Render(program);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while ( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
    glfwWindowShouldClose(window) == 0 );
    
    return 0;
}