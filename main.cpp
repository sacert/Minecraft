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
#include "gui.h"
#include "util.h"
#include "chunk_manager.h"
#include "libraries/stb_image.h"
#include "libraries/FastNoise.h"

// globals - clean up in future
GLFWwindow* gWindow;
Camera gCamera;
SkyBox skybox;
GUI gui;

Coordinates Update(float secondsElapsed, ChunkManager &cm) {

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

    // check to see if mouse was already pressed so it doesn't delete multiple blocks at once
    static int rightMousePressed = 0;
    static int leftMousePressed = 0;

    // used to determine if block exists in corresponding coordinates
    Coordinates cd = Coordinates(0,0,0);

    Coordinates currSelected = Coordinates(999,999,999);

    // deleting blocks
    glm::vec3 line = gCamera.position();
    glm::vec3 prevLine;     // prevLine holds previous line position
    glm::vec3 prevBlock;    // prevBlock holds the position of the block right before one is hit

    for (int i = 0; i < 100 ;i++) {
        // create a line to determine which is the closest block
        // this determines how far the player will be able to break blocks - adjust accordingly
        line += 0.02f * moveSpeed * gCamera.forward(); 
        
        cd = Coordinates(floor(line.x), floor(line.y), floor(line.z));

        // keep a track of the previous position
        if (!cm.getBlock(cd)) {
            prevLine = line;
        }

        // if block found
        if (cm.getBlock(cd)) {
            currSelected = cd;
            break;
        } 

        // if the end of the line is reached without a block being looked at, there is no current selected block
        if (i == 99) {
            currSelected = Coordinates(999,999,999);
        }
    }
    int stateLeft = glfwGetMouseButton(gWindow, GLFW_MOUSE_BUTTON_LEFT);
    if (stateLeft == GLFW_PRESS && leftMousePressed == 0){
        if (cm.getBlock(cd)) {
            cm.removeBlock(cd);
            leftMousePressed = 1;
        }
    }

    int stateRight = glfwGetMouseButton(gWindow, GLFW_MOUSE_BUTTON_RIGHT);
    if (stateRight == GLFW_PRESS && rightMousePressed == 0){
        if (cm.getBlock(cd)) {
            Coordinates cs = Coordinates(floor(prevLine.x), floor(prevLine.y), floor(prevLine.z));
            cm.addBlock(Coordinates(cs.x, cs.y, cs.z), BlockType::DIRT);
            rightMousePressed = 1;
        }
    }

    if (stateLeft == GLFW_RELEASE) {
        leftMousePressed = 0;
    }

    if (stateRight == GLFW_RELEASE) {
        rightMousePressed = 0;
    }


    //rotate camera based on mouse movement
    const float mouseSensitivity = 0.1f;
    double mouseX, mouseY;
    glfwGetCursorPos(gWindow, &mouseX, &mouseY);
    gCamera.offsetOrientation(mouseSensitivity * (float)mouseY, mouseSensitivity * (float)mouseX);
    glfwSetCursorPos(gWindow, 0, 0); //reset the mouse, so it doesn't go out of the window

    return currSelected;
}

// draws a single frame
void Render(ChunkManager &cm, Coordinates selected) {

    // clear everything
    glClear(GL_DEPTH_BUFFER_BIT);

    cm.renderChunks(selected);

    gui.RenderCrosshair();

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

    skybox.LoadSkyBox();
    gui.LoadGUI();

    // Setup gCamera
    gCamera.setPosition(glm::vec3(0,2,-2));
    gCamera.setViewportAspectRatio(SCREEN_SIZE.x / SCREEN_SIZE.y);

    double update_prevTime = glfwGetTime();
    double fps_prevTime = glfwGetTime();
    int frames = 0;

    // chunk manager holds all chunks and deals with them
    ChunkManager cm(&gCamera);

    // itialize basic blocks - mainly just for testing here
    for (int i = -2; i < 2; i++) {
        for (int j = -2; j < 2; j++) {
            cm.addChunk(Coordinates(i,0,j));
        }
    }

    // itialize basic blocks - mainly just for testing here
    for (int i = -2; i < 2; i++) {
        for (int j = -2; j < 2; j++) {
            cm.updateChunk(Coordinates(i,0,j));
        }
    }
    //cm.updateChunk(Coordinates(0,0,0));

    Coordinates selected = Coordinates(999,999,999);

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
        selected = Update((float)(update_currTime - update_prevTime), cm);
        update_prevTime = update_currTime;

        // draw one frame
        Render(cm, selected);
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