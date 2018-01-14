// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list>

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
#include "camera.hpp"

#define N -0.0625   // normalized size of each block in texture file - negative for opengl
#define TEXCOORDY(Y) (1-(0.0625 * Y))    // corrected way to find y coordinates of texture file
#define TEXCOORDX(X) (0.0625 * X)    // corrected way to find x coordinates of texture file

struct BlockAsset {
    GLint shaders;
    Texture* texture;
    GLuint vbo;
    GLuint vao;
    GLenum drawType;
    GLint drawStart;
    GLint drawCount;
};

struct BlockInstance {
    BlockAsset* asset;
    glm::mat4 position;
};

// constants
const glm::vec2 SCREEN_SIZE(800, 600);

// globals - clean up in future
BlockAsset gGrassBlock;
std::list<BlockInstance> gInstances;
GLFWwindow* gWindow;
Camera gCamera;

// Textures are flipped vertically due to how opengl reads them
Texture* LoadTexture() {
    Bitmap bmp = Bitmap::bitmapFromFile("texture.png");
    return new Texture(bmp);
}

// Create the block buffer where the parameters are UV coordinates, allowing for different textures on each side of the block
void createBlockBuffer(GLfloat g_vertex_buffer_data[], int front_x, int front_y, int back_x, int back_y, int right_x, int right_y, 
    int left_x, int left_y, int top_x, int top_y, int bottom_x, int bottom_y) {

    // An array of for the block vertices 
    static const GLfloat vertex_buffer_data[] = {
        //  X     Y     Z       U     V
        // bottom
        -1.0f,-1.0f,-1.0f,   TEXCOORDX(bottom_x),  TEXCOORDY(bottom_y),
         1.0f,-1.0f,-1.0f,   TEXCOORDX(bottom_x)+N,TEXCOORDY(bottom_y),
        -1.0f,-1.0f, 1.0f,   TEXCOORDX(bottom_x),  TEXCOORDY(bottom_y)+N,
         1.0f,-1.0f,-1.0f,   TEXCOORDX(bottom_x)+N,TEXCOORDY(bottom_y),
         1.0f,-1.0f, 1.0f,   TEXCOORDX(bottom_x)+N,TEXCOORDY(bottom_y)+N,
        -1.0f,-1.0f, 1.0f,   TEXCOORDX(bottom_x),  TEXCOORDY(bottom_y)+N,

        // top
        -1.0f, 1.0f,-1.0f,   TEXCOORDX(top_x),  TEXCOORDY(top_y),
        -1.0f, 1.0f, 1.0f,   TEXCOORDX(top_x),  TEXCOORDY(top_y)+N,
         1.0f, 1.0f,-1.0f,   TEXCOORDX(top_x)+N,TEXCOORDY(top_y),
         1.0f, 1.0f,-1.0f,   TEXCOORDX(top_x)+N,TEXCOORDY(top_y),
        -1.0f, 1.0f, 1.0f,   TEXCOORDX(top_x),  TEXCOORDY(top_y)+N,
         1.0f, 1.0f, 1.0f,   TEXCOORDX(top_x)+N,TEXCOORDY(top_y)+N,

        // front
        -1.0f,-1.0f, 1.0f,   TEXCOORDX(front_x)+N,TEXCOORDY(front_y),
         1.0f,-1.0f, 1.0f,   TEXCOORDX(front_x),  TEXCOORDY(front_y),
        -1.0f, 1.0f, 1.0f,   TEXCOORDX(front_x)+N,TEXCOORDY(front_y)+N,
         1.0f,-1.0f, 1.0f,   TEXCOORDX(front_x),  TEXCOORDY(front_y),
         1.0f, 1.0f, 1.0f,   TEXCOORDX(front_x),  TEXCOORDY(front_y)+N,
        -1.0f, 1.0f, 1.0f,   TEXCOORDX(front_x)+N,TEXCOORDY(front_y)+N,

        // back
        -1.0f,-1.0f,-1.0f,   TEXCOORDX(back_x),  TEXCOORDY(back_y),
        -1.0f, 1.0f,-1.0f,   TEXCOORDX(back_x),  TEXCOORDY(back_y)+N,
         1.0f,-1.0f,-1.0f,   TEXCOORDX(back_x)+N,TEXCOORDY(back_y),
         1.0f,-1.0f,-1.0f,   TEXCOORDX(back_x)+N,TEXCOORDY(back_y),
        -1.0f, 1.0f,-1.0f,   TEXCOORDX(back_x),  TEXCOORDY(back_y)+N,
         1.0f, 1.0f,-1.0f,   TEXCOORDX(back_x)+N,TEXCOORDY(back_y)+N,

        // left
        -1.0f,-1.0f, 1.0f,   TEXCOORDX(left_x),  TEXCOORDY(left_y),
        -1.0f, 1.0f,-1.0f,   TEXCOORDX(left_x)+N,TEXCOORDY(left_y)+N,
        -1.0f,-1.0f,-1.0f,   TEXCOORDX(left_x)+N,TEXCOORDY(left_y),
        -1.0f,-1.0f, 1.0f,   TEXCOORDX(left_x),  TEXCOORDY(left_y),
        -1.0f, 1.0f, 1.0f,   TEXCOORDX(left_x),  TEXCOORDY(left_y)+N,
        -1.0f, 1.0f,-1.0f,   TEXCOORDX(left_x)+N,TEXCOORDY(left_y)+N,

        // right
        1.0f,-1.0f, 1.0f,   TEXCOORDX(right_x)+N,TEXCOORDY(right_y),
        1.0f,-1.0f,-1.0f,   TEXCOORDX(right_x),  TEXCOORDY(right_y),
        1.0f, 1.0f, 1.0f,   TEXCOORDX(right_x)+N,TEXCOORDY(right_y)+N,
        1.0f,-1.0f,-1.0f,   TEXCOORDX(right_x),  TEXCOORDY(right_y),
        1.0f, 1.0f,-1.0f,   TEXCOORDX(right_x),  TEXCOORDY(right_y)+N,
        1.0f, 1.0f, 1.0f,   TEXCOORDX(right_x)+N,TEXCOORDY(right_y)+N

    };

    std::copy(vertex_buffer_data, vertex_buffer_data+180, g_vertex_buffer_data);
}

// Create the block where all sides are the same texture
void createBlockBuffer(GLfloat g_vertex_buffer_data[], int x, int y) {

    createBlockBuffer(g_vertex_buffer_data, x, y, x, y, x, y, x, y, x, y, x, y);
}

void LoadGrassBlock() {

    // set all the elements of gGrassBlock
    gGrassBlock.shaders = LoadShaders( "block_vertex.glsl", "block_fragment.glsl" );
    gGrassBlock.drawType = GL_TRIANGLES;
    gGrassBlock.drawStart = 0;
    gGrassBlock.drawCount = 6*2*3;
    gGrassBlock.texture = LoadTexture();
    glGenBuffers(1, &gGrassBlock.vbo);
    glGenVertexArrays(1, &gGrassBlock.vao);

    // Vertex Array Object - will cause everything underneath to be bound to the BAO
    // Makes it so you can just bind VAO and all corresponding will be loaded as well
	glGenVertexArrays(1, &gGrassBlock.vao);
	glBindVertexArray(gGrassBlock.vao);

    // Vertex Buffer Object
    glGenBuffers(1, &gGrassBlock.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, gGrassBlock.vbo);

    // An array of for the block vertices 
    GLfloat g_vertex_buffer_data[180];
    createBlockBuffer(g_vertex_buffer_data,3,15);

    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    // Set the variable 'vert' in vertex shader 
    // Equals to the vertices for the block
    GLuint gVert = glGetAttribLocation(gGrassBlock.shaders, "vert");
    glEnableVertexAttribArray(gVert);
    glVertexAttribPointer(
        0,                  // attribute 0
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        5*sizeof(GLfloat),  // stride
        (void*)0            // array buffer offset
    );

    // Set the variable 'vertTexCoord' in vertex shader 
    // Equals to the UV values for corresponding vertices to the block
    GLuint gVertTexCoord = glGetAttribLocation(gGrassBlock.shaders, "vertTexCoord");
    glEnableVertexAttribArray(gVertTexCoord);
    glVertexAttribPointer(
        1,                  // attribute 1
        2,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        5*sizeof(GLfloat),  // stride
        (const GLvoid*)(3 * sizeof(GLfloat))    // array buffer offset
    );

    // unbind VAO and VBO 
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void CreateInstance() {
    BlockInstance block;
    block.asset = &gGrassBlock;
    block.position = glm::mat4();
    gInstances.push_back(block);

    BlockInstance block1;
    block1.asset = &gGrassBlock;
    block1.position = glm::translate(glm::mat4(1.0f),glm::vec3(2,0,0));
    gInstances.push_back(block1);
}

void Update(float secondsElapsed) {
    
    //move position of camera based on WASD keys
    const float moveSpeed = 2.0; //units per second
    if(glfwGetKey(gWindow, 'S')){
        gCamera.offsetPosition(secondsElapsed * moveSpeed * -gCamera.forward());
    } else if(glfwGetKey(gWindow, 'W')){
        gCamera.offsetPosition(secondsElapsed * moveSpeed * gCamera.forward());
    }
    if(glfwGetKey(gWindow, 'A')){
        gCamera.offsetPosition(secondsElapsed * moveSpeed * -gCamera.right());
    } else if(glfwGetKey(gWindow, 'D')){
        gCamera.offsetPosition(secondsElapsed * moveSpeed * gCamera.right());
    }

    //rotate camera based on mouse movement
    const float mouseSensitivity = 0.1f;
    double mouseX, mouseY;
    glfwGetCursorPos(gWindow, &mouseX, &mouseY);
    gCamera.offsetOrientation(mouseSensitivity * (float)mouseY, mouseSensitivity * (float)mouseX);
    glfwSetCursorPos(gWindow, 0, 0); //reset the mouse, so it doesn't go out of the window
}

void RenderInstances (const BlockInstance& inst) {

    BlockAsset* asset = inst.asset;
    // bind the program (the shaders)
    glUseProgram(asset->shaders);

    // bind the texture and set the "tex" uniform in the fragment shader -- *** could I potentially just bind once since I'm only using 1 texture file? ***
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, asset->texture->object());
    // Get the id for the uniform variable "tex"
    GLint tex = glGetUniformLocation(asset->shaders, "tex");
    glUniform1i(tex, 0);

    // move block to correct position
    GLint model = glGetUniformLocation(asset->shaders, "model");
    glm::mat4 modelPosition = inst.position;
    glUniformMatrix4fv(
 		model,	// Id of this uniform variable
 		1,			    // Number of matrices
 		GL_FALSE,	    // Transpose
 		&modelPosition[0][0]	// The location of the data
 	);

    // set up the camera
    GLint cameraMatrix = glGetUniformLocation(asset->shaders, "camera");
    glm::mat4 cameraView = gCamera.matrix();
 	glUniformMatrix4fv(
 		cameraMatrix,	// Id of this uniform variable
 		1,			    // Number of matrices
 		GL_FALSE,	    // Transpose
 		&cameraView[0][0]	// The location of the data
 	);
        
    // bind the VAO (the triangle)
    glBindVertexArray(asset->vao);

    // draw the triangles 
    glDrawArrays(GL_TRIANGLES, 0, 3*2*6);
    
    // unbind the VAO
    glBindVertexArray(0);
    
    // unbind the program
    glUseProgram(0);

}

// draws a single frame
void Render() {

    // clear everything
    glClearColor(1, 1, 1, 1); // black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // render all instances
    std::list<BlockInstance>::const_iterator it;
    for (it = gInstances.begin(); it != gInstances.end(); ++it) {
        RenderInstances(*it);
    }
    
    // swap the display buffers (displays what was just drawn)
    glfwSwapBuffers(gWindow);
    
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
    gWindow = glfwCreateWindow( SCREEN_SIZE.x, SCREEN_SIZE.y, "Minecraft", NULL, NULL);
    if( gWindow == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(gWindow); // Initialize GLEW
    glewExperimental=1; // Needed in core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    // GLFW settings
    glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(gWindow, 0, 0);

    // Makes sure textures factor in depth when being loaded
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(gWindow, GLFW_STICKY_KEYS, GL_TRUE);

    LoadTexture();

    LoadGrassBlock();

    CreateInstance();

    // Setup gCamera
    gCamera.setPosition(glm::vec3(0,2,-2));
    gCamera.setViewportAspectRatio(SCREEN_SIZE.x / SCREEN_SIZE.y);

    double lastTime = glfwGetTime();
    while(!glfwWindowShouldClose(gWindow)){

        double thisTime = glfwGetTime();
        Update((float)(thisTime - lastTime));
        lastTime = thisTime;

        // draw one frame
        Render();
        glfwPollEvents();

        // check for errors
        GLenum error = glGetError();
        if(error != GL_NO_ERROR)
            std::cerr << "OpenGL Error " << error << std::endl;

        //exit program if escape key is pressed
        if(glfwGetKey(gWindow, GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(gWindow, GL_TRUE);

    } 
    
    return 0;
}