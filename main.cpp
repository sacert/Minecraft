// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include <unordered_map>
#include <vector>
#include <math.h>
#include <string>

// Include GLEW. Always include it before gl.h and glfw.h, since it's a bit magic.
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "texture.h"
#include "bitmap.h"
#include "camera.h"
#include "skybox.h"
#include "frustum.h"
#include "util.h"
#include "chunk_manager.h"
#include "libraries/stb_image.h"
#include "libraries/FastNoise.h"

#define N -0.0625   // normalized size of each block in texture file - negative for opengl
#define TEXCOORDY(Y) (1-(0.0625 * Y))    // corrected way to find y coordinates of texture file
#define TEXCOORDX(X) (0.0625+(0.0625 * X))    // corrected way to find x coordinates of texture file

struct Asset {
    GLint shaders;
    Texture* texture;
    GLuint vbo;
    GLuint vao;
    GLenum drawType;
    GLint drawStart;
    GLint drawCount;
    glm::vec2 scale;
};

// constants
const glm::vec2 SCREEN_SIZE(800, 600);

// globals - clean up in future
GLFWwindow* gWindow;
Camera gCamera;
SkyBox skybox;
Asset gui;

Frustum frustum;
FastNoise perlinNoise; // Create a FastNoise object

void LoadGui(Asset *block) {

    // set all the elements of block
    block->shaders = LoadShaders( "shaders/gui_vertex.glsl", "shaders/gui_fragment.glsl" );
    block->drawType = GL_TRIANGLES;
    block->drawStart = 0;
    block->drawCount = 2*3;
    block->texture = LoadTexture("crosshair.png");
    block->scale.x = 0.15 * (SCREEN_SIZE.y/SCREEN_SIZE.x); // by multiply by screen ratio, scale x and y will make a square
    block->scale.y = 0.15;
    glGenBuffers(1, &block->vbo);
    glGenVertexArrays(1, &block->vao);

	glBindVertexArray(block->vao);
    glBindBuffer(GL_ARRAY_BUFFER, block->vbo);

    // An array of for the crosshair vertices 
    GLfloat g_vertex_buffer_data[] = {
        -1.0f,-1.0f, 0.0f,   1.0f, 0.0f,
         1.0f,-1.0f, 0.0f,   0.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,   1.0f, 1.0f,
         1.0f,-1.0f, 0.0f,   0.0f, 0.0f,
         1.0f, 1.0f, 0.0f,   0.0f, 1.0f,
        -1.0f, 1.0f, 0.0f,   1.0f, 1.0f,
    };

    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    // crosshair vertices
    GLuint gVert = glGetAttribLocation(block->shaders, "vert");
    glEnableVertexAttribArray(gVert);
    glVertexAttribPointer(
        0,                  // attribute 0
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        5*sizeof(GLfloat),  // stride
        (void*)0            // array buffer offset
    );

    // UV values
    GLuint gVertTexCoord = glGetAttribLocation(block->shaders, "vertTexCoord");
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
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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


    // BELOW DEALS WITH DELETING AND SETTING A BLOCK AS SELECTED
    // CHANGED TO USING CHUNKS SO THIS STILL NEEDS TO BE SET UP

    // check to see if mouse was already pressed so it doesn't delete multiple blocks at once
    // required more testing
    // static int rightMousePressed = 0;
    // static int leftMousePressed = 0;
    // Coordinates cd = Coordinates(0,0,0);
    // // deleting blocks
    // glm::vec3 line = gCamera.position();
    // glm::vec3 prevLine;     // prevLine holds previous line position
    // glm::vec3 prevBlock;    // prevBlock holds the position of the block right before one is hit

    // for (int i = 0; i < 100 ;i++) {
    //     // create a line to determine which is the closest block
    //     // this determines how far the player will be able to break blocks - adjust accordingly
    //     line += 0.02f * moveSpeed * gCamera.forward(); 
        
    //     cd = Coordinates(floor(line.x), floor(line.y), floor(line.z));

    //     // keep a track of the previous position
    //     if (!map.count(cd)) {
    //         prevLine = line;
    //     }
    //     if (map.count(cd)) {
    //         prevBlock = prevLine;
    //         // clear the previous selected block and assign it to the new one
    //         if (map.at(cd).selected == 0 && currSelected != NULL) {
    //            currSelected->selected = 0;
    //         }
    //         currSelected = &map.at(cd);
    //         currSelected->selected = 1;
    //         break;
    //     } 

    //     // if the end of the line is reached without a block being looked at, there is no current selected block
    //     if (i == 99 && currSelected != NULL) {
    //         currSelected->selected = 0;
    //         currSelected = NULL;
    //     }
    // }
    // int stateLeft = glfwGetMouseButton(gWindow, GLFW_MOUSE_BUTTON_LEFT);
    // if (stateLeft == GLFW_PRESS && leftMousePressed == 0){
    //     if (map.count(cd)) {
    //         map.erase(cd);
    //         leftMousePressed = 1;
    //     } 
    // }

    // int stateRight = glfwGetMouseButton(gWindow, GLFW_MOUSE_BUTTON_RIGHT);
    // if (stateRight == GLFW_PRESS && rightMousePressed == 0){
    //     if (map.count(cd)) {
    //         // insert a dirt block for testing -- will later replace with player's inventory 
    //         BlockInstance block;
    //         block.asset = &gDirtBlock;
    //         block.selected = 0;
    //         block.position = glm::translate(glm::mat4(1.0f),glm::vec3((int)floor(prevBlock.x),(int)floor(prevBlock.y),(int)floor(prevBlock.z)));
    //         block.cartCoord = glm::vec3((int)floor(prevBlock.x),(int)floor(prevBlock.y),(int)floor(prevBlock.z));
    //         map[Coordinates((int)floor(prevBlock.x),(int)floor(prevBlock.y),(int)floor(prevBlock.z))] = block;
    //         gInstances.push_back(block);
    //         rightMousePressed = 1;
    //     } 
    // }

    // if (stateLeft == GLFW_RELEASE) {
    //     leftMousePressed = 0;
    // }

    // if (stateRight == GLFW_RELEASE) {
    //     rightMousePressed = 0;
    // }


    //rotate camera based on mouse movement
    const float mouseSensitivity = 0.1f;
    double mouseX, mouseY;
    glfwGetCursorPos(gWindow, &mouseX, &mouseY);
    gCamera.offsetOrientation(mouseSensitivity * (float)mouseY, mouseSensitivity * (float)mouseX);
    glfwSetCursorPos(gWindow, 0, 0); //reset the mouse, so it doesn't go out of the window
}

void RenderCrosshair() {

    // use program - shaders
    glUseProgram(gui.shaders);

    // active and bind textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gui.texture->object());

    // scaling crosshair
    GLint scale = glGetUniformLocation(gui.shaders, "scale");
    glUniform2f(scale, gui.scale.x, gui.scale.y);

    // texture values
    GLint tex = glGetUniformLocation(gui.shaders, "tex");
    glUniform1i(tex, 0);
        
    // bind the VAO and draw everything inside
    glBindVertexArray(gui.vao);
    glDrawArrays(GL_TRIANGLES, 0, gui.drawCount);
    
    // unbind VAO and program (shaders)
    glBindVertexArray(0);
    glUseProgram(0);
}

// draws a single frame
void Render(std::vector<Chunk> &chunks) {

    // clear everything
    glClear(GL_DEPTH_BUFFER_BIT);

    // update frustum to current view
    frustum.getFrustum(gCamera.view(), gCamera.projection());

    // render the chunks
    for (int i = 0; i < chunks.size(); i++) {
        chunks.at(i).renderChunk();
    }

    RenderCrosshair();

    // render skybox last
    glDisable(GL_CULL_FACE);
    skybox.Render(gCamera);
    glEnable(GL_CULL_FACE);
    
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
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(gWindow, GLFW_STICKY_KEYS, GL_TRUE);

    // Turn off vsync ***** LEAVE ON FOR OPTIMISATION TESTING BUT DELETE AFTER ******
    glfwSwapInterval(0);

    // Setting up perlin noise - should be within it's own class (change later)
    perlinNoise.SetNoiseType(FastNoise::Perlin); // Set the desired noise type
    perlinNoise.SetSeed(123);

    skybox.LoadSkyBox();
    LoadGui(&gui);

    // Setup gCamera
    gCamera.setPosition(glm::vec3(0,2,-2));
    gCamera.setViewportAspectRatio(SCREEN_SIZE.x / SCREEN_SIZE.y);

    double update_prevTime = glfwGetTime();
    double fps_prevTime = glfwGetTime();
    int frames = 0;

    std::vector<Chunk> chunks;

    for (int i = -1; i < 1; i++) {
        for (int j = -1; j < 1; j++) {
            Chunk chunk(i, j, &gCamera);
            chunk.createChunk();
            chunks.push_back(chunk); 
        }
    }

    while(!glfwWindowShouldClose(gWindow)){

        // basic FPS display
        double fps_currTime = glfwGetTime();
        frames++;
        if (fps_currTime - fps_prevTime >= 1.0) {
            printf("%f FPS\n", frames/(fps_currTime - fps_prevTime));
            frames = 0;
            fps_prevTime = fps_currTime;
        }

        double update_currTime = glfwGetTime();
        Update((float)(update_currTime - update_prevTime));
        update_prevTime = update_currTime;

        // draw one frame
        Render(chunks);
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