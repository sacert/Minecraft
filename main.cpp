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

#include "shader.hpp"
#include "texture.hpp"
#include "bitmap.hpp"
#include "camera.hpp"
#include "skybox.hpp"
#include "frustum.hpp"
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

struct BlockInstance {
    Asset* asset;
    glm::mat4 position;
    glm::vec3 cartCoord; // cartesian
    float selected;
};

struct Coordinate {
    int x;
    int y;
    int z;

    Coordinate (int xx, int yy, int zz) {
        x = xx;
        y = yy;
        z = zz;
    }

    bool operator==(const Coordinate &other) const
    { return (x == other.x
                && y == other.y
                && z == other.z);
    }
};

namespace std {

    // include this to be able to hash Coordinates
    template <>
    struct hash<Coordinate>
    { 
        std::size_t operator()(const Coordinate& k) const
        {
        using std::size_t;
        using std::hash;

        return ((hash<int>()(k.x)
                ^ (hash<int>()(k.y) << 1)) >> 1)
                ^ (hash<int>()(k.z) << 1);
        }
    };
}

// constants
const glm::vec2 SCREEN_SIZE(800, 600);

// globals - clean up in future
Asset gGrassBlock;
Asset gDirtBlock;
std::list<BlockInstance> gInstances;
GLFWwindow* gWindow;
Camera gCamera;
std::unordered_map<Coordinate, BlockInstance> map;
SkyBox skybox;
BlockInstance *currSelected;
Asset gui;

Frustum frustum;
FastNoise perlinNoise; // Create a FastNoise object

// Textures are flipped vertically due to how opengl reads them
Texture* LoadTexture(std::string fileLocation) {
    Bitmap bmp = Bitmap::bitmapFromFile(fileLocation);
    return new Texture(bmp);
}

// Create the block buffer where the parameters are UV coordinates, allowing for different textures on each side of the block
void createBlockBuffer(GLfloat g_vertex_buffer_data[], int front_x, int front_y, int back_x, int back_y, int right_x, int right_y, 
    int left_x, int left_y, int top_x, int top_y, int bottom_x, int bottom_y) {

    // An array of for the block vertices 
    GLfloat vertex_buffer_data[] = {
        //  X     Y     Z       U     V
        // bottom
        0.0f, 0.0f, 0.0f,   TEXCOORDX(bottom_x),  TEXCOORDY(bottom_y),
        1.0f, 0.0f, 0.0f,   TEXCOORDX(bottom_x)+N,TEXCOORDY(bottom_y),
        0.0f, 0.0f, 1.0f,   TEXCOORDX(bottom_x),  TEXCOORDY(bottom_y)+N,
        1.0f, 0.0f, 0.0f,   TEXCOORDX(bottom_x)+N,TEXCOORDY(bottom_y),
        1.0f, 0.0f, 1.0f,   TEXCOORDX(bottom_x)+N,TEXCOORDY(bottom_y)+N,
        0.0f, 0.0f, 1.0f,   TEXCOORDX(bottom_x),  TEXCOORDY(bottom_y)+N,

        // top
        0.0f, 1.0f, 0.0f,   TEXCOORDX(top_x),  TEXCOORDY(top_y),
        0.0f, 1.0f, 1.0f,   TEXCOORDX(top_x),  TEXCOORDY(top_y)+N,
        1.0f, 1.0f, 0.0f,   TEXCOORDX(top_x)+N,TEXCOORDY(top_y),
        1.0f, 1.0f, 0.0f,   TEXCOORDX(top_x)+N,TEXCOORDY(top_y),
        0.0f, 1.0f, 1.0f,   TEXCOORDX(top_x),  TEXCOORDY(top_y)+N,
        1.0f, 1.0f, 1.0f,   TEXCOORDX(top_x)+N,TEXCOORDY(top_y)+N,

        // front
        0.0f, 0.0f, 1.0f,   TEXCOORDX(front_x)+N,TEXCOORDY(front_y),
        1.0f, 0.0f, 1.0f,   TEXCOORDX(front_x),  TEXCOORDY(front_y),
        0.0f, 1.0f, 1.0f,   TEXCOORDX(front_x)+N,TEXCOORDY(front_y)+N,
        1.0f, 0.0f, 1.0f,   TEXCOORDX(front_x),  TEXCOORDY(front_y),
        1.0f, 1.0f, 1.0f,   TEXCOORDX(front_x),  TEXCOORDY(front_y)+N,
        0.0f, 1.0f, 1.0f,   TEXCOORDX(front_x)+N,TEXCOORDY(front_y)+N,

        // back
        0.0f, 0.0f, 0.0f,   TEXCOORDX(back_x),  TEXCOORDY(back_y),
        0.0f, 1.0f, 0.0f,   TEXCOORDX(back_x),  TEXCOORDY(back_y)+N,
        1.0f, 0.0f, 0.0f,   TEXCOORDX(back_x)+N,TEXCOORDY(back_y),
        1.0f, 0.0f, 0.0f,   TEXCOORDX(back_x)+N,TEXCOORDY(back_y),
        0.0f, 1.0f, 0.0f,   TEXCOORDX(back_x),  TEXCOORDY(back_y)+N,
        1.0f, 1.0f, 0.0f,   TEXCOORDX(back_x)+N,TEXCOORDY(back_y)+N,

        // left
        0.0f, 0.0f, 1.0f,   TEXCOORDX(left_x),  TEXCOORDY(left_y),
        0.0f, 1.0f, 0.0f,   TEXCOORDX(left_x)+N,TEXCOORDY(left_y)+N,
        0.0f, 0.0f, 0.0f,   TEXCOORDX(left_x)+N,TEXCOORDY(left_y),
        0.0f, 0.0f, 1.0f,   TEXCOORDX(left_x),  TEXCOORDY(left_y),
        0.0f, 1.0f, 1.0f,   TEXCOORDX(left_x),  TEXCOORDY(left_y)+N,
        0.0f, 1.0f, 0.0f,   TEXCOORDX(left_x)+N,TEXCOORDY(left_y)+N,

        // right
        1.0f, 0.0f, 1.0f,   TEXCOORDX(right_x)+N,TEXCOORDY(right_y),
        1.0f, 0.0f, 0.0f,   TEXCOORDX(right_x),  TEXCOORDY(right_y),
        1.0f, 1.0f, 1.0f,   TEXCOORDX(right_x)+N,TEXCOORDY(right_y)+N,
        1.0f, 0.0f, 0.0f,   TEXCOORDX(right_x),  TEXCOORDY(right_y),
        1.0f, 1.0f, 0.0f,   TEXCOORDX(right_x),  TEXCOORDY(right_y)+N,
        1.0f, 1.0f, 1.0f,   TEXCOORDX(right_x)+N,TEXCOORDY(right_y)+N

    };

    std::copy(vertex_buffer_data, vertex_buffer_data+180, g_vertex_buffer_data);
}

// Create the block where all sides are the same texture
void createBlockBuffer(GLfloat g_vertex_buffer_data[], int x, int y) {

    createBlockBuffer(g_vertex_buffer_data, x, y, x, y, x, y, x, y, x, y, x, y);
}

void LoadBlock(Asset *block, int front_x, int front_y, int back_x, int back_y, int right_x, int right_y, 
    int left_x, int left_y, int top_x, int top_y, int bottom_x, int bottom_y) {

    // set all the elements of block
    block->shaders = LoadShaders( "shaders/block_vertex.glsl", "shaders/block_fragment.glsl" );
    block->drawType = GL_TRIANGLES;
    block->drawStart = 0;
    block->drawCount = 6*2*3;
    block->texture = LoadTexture("texture.png");
    glGenBuffers(1, &block->vbo);
    glGenVertexArrays(1, &block->vao);

    // Vertex Array Object - will cause everything underneath to be bound to the VAO
    // Makes it so you can just bind VAO and all corresponding will be loaded as well
	glBindVertexArray(block->vao);

    // Vertex Buffer Object
    glBindBuffer(GL_ARRAY_BUFFER, block->vbo);

    // An array of for the block vertices 
    GLfloat g_vertex_buffer_data[180];
    createBlockBuffer(g_vertex_buffer_data,front_x,front_y,back_x,back_y,right_x,right_y,left_x,left_y,top_x,top_y,bottom_x,bottom_y);

    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    // Set the variable 'vert' in vertex shader 
    // Equals to the vertices for the block
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

    // Set the variable 'vertTexCoord' in vertex shader 
    // Equals to the UV values for corresponding vertices to the block
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

// overload function so that it is easier to load blocks where all the sides are the same texture
void LoadBlock(Asset *block, int texture_x, int texture_y) {
    LoadBlock(block, texture_x, texture_y, texture_x, texture_y, texture_x, texture_y, texture_x, texture_y, texture_x, texture_y, texture_x, texture_y);
}

// world creation should be in here
void CreateWorld() {

    // testing 

    // 10x10x3 block of dirt
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            for (int k = 0; k < 1; k++) {
                BlockInstance block;
                if (k == 1) {
                    block.asset = &gGrassBlock;
                } else {
                    block.asset = &gDirtBlock;
                }
                float height = round(perlinNoise.GetNoise(j,k,i) * 10);
                block.selected = 0;
                block.position = glm::translate(glm::mat4(1.0f),glm::vec3(j,height,i));
                block.cartCoord = glm::vec3(j,height,i);
                map[Coordinate(j, height, i)] = block;
                gInstances.push_back(block);
            }
        }
    }
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


    // check to see if mouse was already pressed so it doesn't delete multiple blocks at once
    // required more testing
    static int rightMousePressed = 0;
    static int leftMousePressed = 0;
    Coordinate cd = Coordinate(0,0,0);
    // deleting blocks
    glm::vec3 line = gCamera.position();
    glm::vec3 prevLine;     // prevLine holds previous line position
    glm::vec3 prevBlock;    // prevBlock holds the position of the block right before one is hit

    for (int i = 0; i < 100 ;i++) {
        // create a line to determine which is the closest block
        // this determines how far the player will be able to break blocks - adjust accordingly
        line += 0.02f * moveSpeed * gCamera.forward(); 
        
        cd = Coordinate(floor(line.x), floor(line.y), floor(line.z));

        // keep a track of the previous position
        if (!map.count(cd)) {
            prevLine = line;
        }
        if (map.count(cd)) {
            prevBlock = prevLine;
            // clear the previous selected block and assign it to the new one
            if (map.at(cd).selected == 0 && currSelected != NULL) {
               currSelected->selected = 0;
            }
            currSelected = &map.at(cd);
            currSelected->selected = 1;
            break;
        } 

        // if the end of the line is reached without a block being looked at, there is no current selected block
        if (i == 99 && currSelected != NULL) {
            currSelected->selected = 0;
            currSelected = NULL;
        }
    }
    int stateLeft = glfwGetMouseButton(gWindow, GLFW_MOUSE_BUTTON_LEFT);
    if (stateLeft == GLFW_PRESS && leftMousePressed == 0){
        if (map.count(cd)) {
            map.erase(cd);
            leftMousePressed = 1;
        } 
    }

    int stateRight = glfwGetMouseButton(gWindow, GLFW_MOUSE_BUTTON_RIGHT);
    if (stateRight == GLFW_PRESS && rightMousePressed == 0){
        if (map.count(cd)) {
            // insert a dirt block for testing -- will later replace with player's inventory 
            BlockInstance block;
            block.asset = &gDirtBlock;
            block.selected = 0;
            block.position = glm::translate(glm::mat4(1.0f),glm::vec3((int)floor(prevBlock.x),(int)floor(prevBlock.y),(int)floor(prevBlock.z)));
            block.cartCoord = glm::vec3((int)floor(prevBlock.x),(int)floor(prevBlock.y),(int)floor(prevBlock.z));
            map[Coordinate((int)floor(prevBlock.x),(int)floor(prevBlock.y),(int)floor(prevBlock.z))] = block;
            gInstances.push_back(block);
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
}

void RenderInstances (const BlockInstance& inst) {

    // check if the current block is in view - if not, no need to render it
    if (!frustum.cubeInFrustum(inst.cartCoord.x, inst.cartCoord.y, inst.cartCoord.z, 1)) {
        return;
    }

    Asset* asset = inst.asset;
    // bind the program (the shaders)
    glUseProgram(asset->shaders);

    // bind the texture and set the "tex" uniform in the fragment shader -- *** could I potentially just bind once since I'm only using 1 texture file? ***
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, asset->texture->object());
    // Get the id for the uniform variable "tex"
    GLint tex = glGetUniformLocation(asset->shaders, "tex");
    glUniform1i(tex, 0);

    GLint selected = glGetUniformLocation(asset->shaders, "selected");
    glUniform1fv(selected, 1, &inst.selected);

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
    glDrawArrays(GL_TRIANGLES, 0, asset->drawCount);
    
    // unbind the VAO
    glBindVertexArray(0);
    
    // unbind the program
    glUseProgram(0);

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
void Render() {

    // clear everything
    glClearColor(1, 1, 1, 1); // white
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // update frustum to current view
    frustum.getFrustum(gCamera.view(), gCamera.projection());

    // go through the unordered list and render all blocks -- there may be a faster approach to this
    for (std::unordered_map<Coordinate, BlockInstance>::iterator it = map.begin(); it != map.end(); ++it) {
        RenderInstances(it->second);
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

    // Block Loading
    LoadBlock(&gDirtBlock, 2, 15);
    LoadBlock(&gGrassBlock, 3, 15, 3, 15, 3, 15, 3, 15, 0, 15, 2, 15);
    skybox.LoadSkyBox();
    LoadGui(&gui);

    CreateWorld();

    // Setup gCamera
    gCamera.setPosition(glm::vec3(0,2,-2));
    gCamera.setViewportAspectRatio(SCREEN_SIZE.x / SCREEN_SIZE.y);

    double update_prevTime = glfwGetTime();
    double fps_prevTime = glfwGetTime();
    int frames = 0;

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