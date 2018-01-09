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
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "texture.hpp"
#include "bitmap.hpp"

GLuint gVAO = 0;
GLuint gVBO = 0;
GLuint gElements = 0;
GLFWwindow* window;
GLint mvpMatrix;
GLint tex;
Texture* gTexture = NULL;

void LoadTexture() {
    Bitmap bmp = Bitmap::bitmapFromFile("texture.png");
    bmp.flipVertically();
    gTexture = new Texture(bmp);
}

glm::mat4 buildMatrix()
 {
 	glm::mat4 projection = glm::perspective(
 		45.0f,		// Field of view
 		4.0f / 3.0f, // Aspect ratio
 		0.1f,		// Near clipping plane
 		100.0f		// Far clipping plane
 	);
 
 	glm::mat4 view = glm::lookAt(
 		glm::vec3(-4, 3, 3),	// Camera location in world coordinates
 		glm::vec3(0, 0, 0),	// Looking at the origin
 		glm::vec3(0, 1, 0)	// Camera up vector
 	);
 
 	glm::mat4 model = glm::mat4(1.0f);
 
 	return projection * view * model;
 }

#define N 0.0625
#define width (0.0625 * 15)
#define height (0.0625 * 0)

void LoadCube(GLuint program) {
    // Vertex Array Object
	glGenVertexArrays(1, &gVAO);
	glBindVertexArray(gVAO);

    // Vertex Buffer Object
    glGenBuffers(1, &gVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gVBO);

    // An array of 3 vectors which represents 3 vertices
    static const GLfloat g_vertex_buffer_data[] = {
        //  X     Y     Z       U     V
        // bottom
        -1.0f,-1.0f,-1.0f,   width, height,
        1.0f,-1.0f,-1.0f,   width+N, height,
        -1.0f,-1.0f, 1.0f,   width, height+N,
        1.0f,-1.0f,-1.0f,   width+N, height,
        1.0f,-1.0f, 1.0f,   width+N, height+N,
        -1.0f,-1.0f, 1.0f,   width, height+N,

        // top
        -1.0f, 1.0f,-1.0f,   width, height,
        -1.0f, 1.0f, 1.0f,   width, height+N,
        1.0f, 1.0f,-1.0f,   width+N, height,
        1.0f, 1.0f,-1.0f,   width+N, height,
        -1.0f, 1.0f, 1.0f,   width, height+N,
        1.0f, 1.0f, 1.0f,   width+N, height+N,

        // front
        -1.0f,-1.0f, 1.0f,   width+N, height,
        1.0f,-1.0f, 1.0f,   width, height,
        -1.0f, 1.0f, 1.0f,   width+N, height+N,
        1.0f,-1.0f, 1.0f,   width, height,
        1.0f, 1.0f, 1.0f,   width, height+N,
        -1.0f, 1.0f, 1.0f,   width+N, height+N,

        // back
        -1.0f,-1.0f,-1.0f,   width, height,
        -1.0f, 1.0f,-1.0f,   width, height+N,
        1.0f,-1.0f,-1.0f,   width+N, height,
        1.0f,-1.0f,-1.0f,   width+N, height,
        -1.0f, 1.0f,-1.0f,   width, height+N,
        1.0f, 1.0f,-1.0f,   width+N, height+N,

        // left
        -1.0f,-1.0f, 1.0f,   width, height+N,
        -1.0f, 1.0f,-1.0f,   width+N, height,
        -1.0f,-1.0f,-1.0f,   width, height,
        -1.0f,-1.0f, 1.0f,   width, height+N,
        -1.0f, 1.0f, 1.0f,   width+N, height+N,
        -1.0f, 1.0f,-1.0f,   width+N, height,

        // right
        1.0f,-1.0f, 1.0f,   width+N, height+N,
        1.0f,-1.0f,-1.0f,   width+N, height,
        1.0f, 1.0f,-1.0f,   width, height,
        1.0f,-1.0f, 1.0f,   width+N, height+N,
        1.0f, 1.0f,-1.0f,   width, height,
        1.0f, 1.0f, 1.0f,   width, height+N

    };

    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    GLuint iVert = glGetAttribLocation(program, "vert");
    glEnableVertexAttribArray(iVert);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        5*sizeof(GLfloat),                  // stride
        (void*)0            // array buffer offset
    );

    GLuint ivertTexCoord = glGetAttribLocation(program, "vertTexCoord");
    glEnableVertexAttribArray(ivertTexCoord);
    glVertexAttribPointer(
        1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        2,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        5*sizeof(GLfloat),                  // stride
        (const GLvoid*)(3 * sizeof(GLfloat))         // array buffer offset
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

    // bind the texture and set the "tex" uniform in the fragment shader
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTexture->object());
    // Get the id for the uniform variable "tex"
 	tex = glGetUniformLocation(program, "tex");
    glUniform1i(tex, 0);

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

    // draw the triangles 
    glDrawArrays(GL_TRIANGLES, 0, 3*2*6);
    
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

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    GLuint program = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );

    // Get the id for the uniform variable "mvpMatrix"
 	mvpMatrix = glGetUniformLocation(program, "mvpMatrix");

    LoadTexture();

    LoadCube(program);

    do {

        Render(program);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while ( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
    glfwWindowShouldClose(window) == 0 );
    
    return 0;
}