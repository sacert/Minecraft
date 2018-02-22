// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include <unordered_map>
#include <vector>
#include <math.h>
#include <string>
#include <thread>

// Include GLEW. Always include it before gl.h and glfw.h, since it's a bit magic.
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

#include "gui.h"
#include "shader.h"
#include "texture.h"
#include "bitmap.h"
#include "camera.h"
#include "skybox.h"
#include "util.h"
#include "libraries/stb_image.h"
#include "libraries/FastNoise.h"

// determine if the user has changed currently selected block
int userBlock = 1;
int prevBlock = 1;

int getChunkPos(int val) {
    while(1) {
        if (val  % 16 == 0) 
            break;
        val--;
    }
    return val/16;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {

    // scrolling down - skip 0 cause that is air
    if (yoffset < 0) {
        userBlock -= 1;
        if (userBlock == 0) 
            userBlock = BlockType::WOOD;
    } else {    // scrolling up, again skip 0 since it is air
        userBlock += 1;
        if ((userBlock % (BlockType::WOOD+1) == 0)) {
            userBlock++;
        }
    }

    // make sure to select blocks only within available blocks - in the current case everything below wood block should be placeable
    userBlock %= (BlockType::WOOD+1);
}

Coordinates Update(float secondsElapsed, ChunkManager *cm, Camera &camera, GLFWwindow* window, GUI &gui) {

    // only change the block if there has been an update
    if (prevBlock != userBlock) {
        gui.LoadInventory(BlockType(userBlock));
        prevBlock = userBlock;
    }
    //std::thread t1(&ChunkManager::proceduralMapUpdate, cm, camera.position());
    //Chunk chunk;
    //std::thread t1(test, camera);
    //t1.join();
    cm->proceduralMapUpdate(camera.position());

    //move position of camera based on WASD keys
    const float moveSpeed = 10.0; //units per second
    if(glfwGetKey(window, 'S')){
        camera.offsetPosition(secondsElapsed * moveSpeed * -camera.forward());
    } else if(glfwGetKey(window, 'W')){
        camera.offsetPosition(secondsElapsed * moveSpeed * camera.forward());
    }
    if(glfwGetKey(window, 'A')){
        camera.offsetPosition(secondsElapsed * moveSpeed * -camera.right());
    } else if(glfwGetKey(window, 'D')){
        camera.offsetPosition(secondsElapsed * moveSpeed * camera.right());
    }

    // check to see if mouse was already pressed so it doesn't delete multiple blocks at once
    static int rightMousePressed = 0;
    static int leftMousePressed = 0;

    // used to determine if block exists in corresponding coordinates
    Coordinates cd = Coordinates(0,0,0);

    // hold the block that is currently selected
    Coordinates currSelected = Coordinates(999,999,999);

    // deleting blocks
    glm::vec3 line = camera.position();
    glm::vec3 prevLine;     // prevLine holds previous line position
    glm::vec3 prevBlock;    // prevBlock holds the position of the block right before one is hit

    for (int i = 0; i < 100 ;i++) {
        // create a line to determine which is the closest block
        // this determines how far the player will be able to break blocks - adjust accordingly
        line += 0.02f * moveSpeed * camera.forward(); 
        
        cd = Coordinates(floor(line.x), floor(line.y), floor(line.z));

        // keep a track of the previous position
        if (!cm->getBlock(cd)) {
            prevLine = line;
        }

        // if block found
        if (cm->getBlock(cd)) {
            currSelected = cd;
            break;
        } 

        // if the end of the line is reached without a block being looked at, there is no current selected block
        if (i == 99) {
            currSelected = Coordinates(999,999,999);
        }
    }
    int stateLeft = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (stateLeft == GLFW_PRESS && leftMousePressed == 0){
        if (cm->getBlock(cd)) {
            cm->removeBlock(cd);
            leftMousePressed = 1;
        }
    }

    int stateRight = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    if (stateRight == GLFW_PRESS && rightMousePressed == 0){
        if (cm->getBlock(cd)) {
            Coordinates cs = Coordinates(floor(prevLine.x), floor(prevLine.y), floor(prevLine.z));
            cm->addBlock(Coordinates(cs.x, cs.y, cs.z), BlockType(userBlock));
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
    glfwGetCursorPos(window, &mouseX, &mouseY);
    camera.offsetOrientation(mouseSensitivity * (float)mouseY, mouseSensitivity * (float)mouseX);
    glfwSetCursorPos(window, 0, 0); //reset the mouse, so it doesn't go out of the window

    return currSelected;
}

// draws a single frame
void Render(ChunkManager *cm, Coordinates selected, Camera &camera, GLFWwindow *window, GUI gui, SkyBox skybox) {

    // clear everything
    glClear(GL_DEPTH_BUFFER_BIT);

    cm->renderChunks(selected);

    // render skybox last
    glDisable(GL_CULL_FACE);
    skybox.Render(camera);
    glEnable(GL_CULL_FACE);

    // render crosshair and inventory
    // disable depth test so it always appears in front - like a gui
    glDisable(GL_DEPTH_TEST);
    gui.RenderCrosshair();
    gui.RenderInventory();
    glEnable(GL_DEPTH_TEST);
    
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
    GLFWwindow* window = glfwCreateWindow( SCREEN_SIZE.x, SCREEN_SIZE.y, "Minecraft", NULL, NULL);
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

    // GLFW settings
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(window, 0, 0);
    glfwSetScrollCallback(window, scroll_callback);

    // Makes sure textures factor in depth when being loaded
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Turn off vsync ***** LEAVE ON FOR OPTIMISATION TESTING BUT DELETE AFTER ******
    glfwSwapInterval(0);
    
    SkyBox skybox;
    skybox.LoadSkyBox();

    GUI gui;
    gui.LoadCrosshair();
    gui.LoadInventory(BlockType(userBlock));


    // Setup camera
    Camera camera;
    camera.setPosition(glm::vec3(0,2,0));
    camera.setViewportAspectRatio(SCREEN_SIZE.x / SCREEN_SIZE.y);

    double update_prevTime = glfwGetTime();
    double fps_prevTime = glfwGetTime();
    int frames = 0;

    // chunk manager holds all chunks and deals with them
    ChunkManager* cm = new ChunkManager(&camera);

    // itialize basic blocks - mainly just for testing here
    for (int i = -CHUNK_RENDER_DISTANCE; i < CHUNK_RENDER_DISTANCE; i++) {
        for (int j = -CHUNK_RENDER_DISTANCE; j < CHUNK_RENDER_DISTANCE; j++) {
            cm->addChunk(Coordinates(i,0,j));
        }
    }

    // itialize basic blocks - mainly just for testing here
    for (int i = -CHUNK_RENDER_DISTANCE; i < CHUNK_RENDER_DISTANCE; i++) {
        for (int j = -CHUNK_RENDER_DISTANCE; j < CHUNK_RENDER_DISTANCE; j++) {
            cm->updateChunk(Coordinates(i,0,j));
        }
    }

    while(!glfwWindowShouldClose(window)){

        // basic FPS display
        double fps_currTime = glfwGetTime();
        frames++;
        if (fps_currTime - fps_prevTime >= 1.0) {
            printf("%f FPS\n", frames/(fps_currTime - fps_prevTime));
            frames = 0;
            fps_prevTime = fps_currTime;
        }

        double update_currTime = glfwGetTime();
        Coordinates selected = Update((float)(update_currTime - update_prevTime), cm, camera, window, gui);
        update_prevTime = update_currTime;

        // draw one frame
        Render(cm, selected, camera, window, gui, skybox);
        glfwPollEvents();

        // check for errors
        GLenum error = glGetError();
        if(error != GL_NO_ERROR)
            std::cerr << "OpenGL Error " << error << std::endl;

        //exit program if escape key is pressed
        if(glfwGetKey(window, GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window, GL_TRUE);

    } 
    
    return 0;
}