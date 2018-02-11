
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "chunk.h"
#include "util.h"

Chunk::Chunk() {
    empty = true;
}

// set x and z value to appropriate positions
Chunk::Chunk(int x, int z, Camera *cam, BlockType ***block) {
    chunk_x = x * 16;
    chunk_z = z * 16;

    // each chunk has it's own perlin noise
    perlinNoise.SetNoiseType(FastNoise::Perlin); 
    perlinNoise.SetSeed(123);

    texture = LoadTexture("texture.png");
    shaders = LoadShaders( "shaders/block_vertex.glsl", "shaders/block_fragment.glsl" );
    camera = cam;
    blocks = block;

    empty = false;
}

Chunk::~Chunk() {

    // not properly working, have to look into it - seg fault

    // delete memory
    // for(int i = 0; i < CHUNK_HEIGHT; i++)
    // {
    //     for(int j(0); j < CHUNK_SIZE; j++)
    //         delete[] blocks[i][j];
    //     delete[] blocks[i];
    // }
    // delete[] blocks;
}

void Chunk::updateChunk(Chunk* nb) {

    // assign it's appropriate neighbours
    neighbours = nb;

    // height between -128 to 128
    int maxHeight = CHUNK_HEIGHT/2;
    int minHeight = -maxHeight;

    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);

    // Vertex Array Object - will cause everything underneath to be bound to the VAO
    // Makes it so you can just bind VAO and all corresponding will be loaded as well
	glBindVertexArray(vao);

    // Vertex Buffer Object
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    buffer_data.clear();
    
    // fill chunk vao
    for (int x = chunk_x; x < (chunk_x+CHUNK_SIZE); x++) {
        for (int z = chunk_z; z < (chunk_z+CHUNK_SIZE); z++) {
            for (int y = minHeight; y < maxHeight; y++) {

                Coordinates coord(x,y,z);

                // convert to 0 .. 15 / 255
                int block_x = x - chunk_x;
                int block_y = y + CHUNK_HEIGHT/2;
                int block_z = z - chunk_z;

                BlockType bt = blocks[block_x][block_y][block_z];

        
                if (bt != BlockType::AIR) {
                    // top
                    
                    if (checkNeighbour(block_x,block_y+1,block_z)) {
                        addTopFace(buffer_data, coord, bt);
                        faces++;
                    }
                    // bottom
                    if (checkNeighbour(block_x,block_y-1,block_z)) {
                        addBottomFace(buffer_data, coord, bt);
                        faces++;
                    }
                    // right
                    if (checkNeighbour(block_x+1,block_y,block_z)) {
                        addRightFace(buffer_data, coord, bt);
                        faces++;
                    }
                    // left
                    if (checkNeighbour(block_x-1,block_y,block_z)) {
                        addLeftFace(buffer_data, coord, bt);
                        faces++;
                    }
                    // front
                    if (checkNeighbour(block_x,block_y,block_z+1)) {
                        addFrontFace(buffer_data, coord, bt);
                        faces++;
                    }
                    // back
                    if (checkNeighbour(block_x,block_y,block_z-1)) {
                        addBackFace(buffer_data, coord, bt);
                        faces++;
                    }
                }
            }
        }
    }

    // std::chrono::steady_clock::time_point end= std::chrono::steady_clock::now();
    // std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() <<std::endl;

    // Give our vertices and UVs to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, buffer_data.size() * sizeof(GLfloat), buffer_data.data(), GL_STATIC_DRAW);

    // Set the variable 'vert' in vertex shader 
    // Equals to the vertices for the block
    GLuint gVert = glGetAttribLocation(shaders, "vert");
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
    GLuint gVertTexCoord = glGetAttribLocation(shaders, "vertTexCoord");
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

    //glDisableVertexAttribArray(gVert);
    //glDisableVertexAttribArray(gVertTexCoord);
}

Chunk * Chunk::getNeighbours() {
    return neighbours;
}

void Chunk::createChunk() {

    // height between -128 to 128
    int maxHeight = CHUNK_HEIGHT/2;
    int minHeight = -maxHeight;

    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);

    // Vertex Array Object - will cause everything underneath to be bound to the VAO
    // Makes it so you can just bind VAO and all corresponding will be loaded as well
	glBindVertexArray(vao);

    // Vertex Buffer Object
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // 16 * 256 * 16
    // build chunk map

    for (int x = chunk_x; x < (chunk_x+CHUNK_SIZE); x++) {
        for (int z = chunk_z; z < (chunk_z+CHUNK_SIZE); z++) {

            float height = round(perlinNoise.GetNoise(x,z) * 10);
            for (int y = minHeight; y < maxHeight; y++) {

                BlockType bt;

                if (y > height) {
                    bt = BlockType::AIR;
                } else if (y == height) {
                    bt = BlockType::GRASS;
                } else if (y <= -8 && y > -127) {
                    bt = BlockType::COBBLESTONE;
                } else if (y == -127) {
                    bt = BlockType::BEDROCK;
                } else {
                    bt = BlockType::DIRT;
                }

                // convert to 0 .. 15 / 255
                int block_x = x - chunk_x;
                int block_y = y + CHUNK_HEIGHT/2;
                int block_z = z - chunk_z;
                
                blocks[block_x][block_y][block_z] = bt;

            }
        }
    }
    
    // fill chunk vao
    for (int x = chunk_x; x < (chunk_x+CHUNK_SIZE); x++) {
        for (int z = chunk_z; z < (chunk_z+CHUNK_SIZE); z++) {
            for (int y = minHeight; y < maxHeight; y++) {

                Coordinates coord(x,y,z);

                // convert to 0 .. 15 / 255
                int block_x = x - chunk_x;
                int block_y = y + CHUNK_HEIGHT/2;
                int block_z = z - chunk_z;

                BlockType bt = blocks[block_x][block_y][block_z];

        
                if (bt != BlockType::AIR) {
                    // top
                    
                    if (checkFace(block_x,block_y+1,block_z)) {
                        addTopFace(buffer_data, coord, bt);
                        faces++;
                    }
                    // bottom
                    if (checkFace(block_x,block_y-1,block_z)) {
                        addBottomFace(buffer_data, coord, bt);
                        faces++;
                    }
                    // right
                    if (checkFace(block_x+1,block_y,block_z)) {
                        addRightFace(buffer_data, coord, bt);
                        faces++;
                    }
                    // left
                    if (checkFace(block_x-1,block_y,block_z)) {
                        addLeftFace(buffer_data, coord, bt);
                        faces++;
                    }
                    // front
                    if (checkFace(block_x,block_y,block_z+1)) {
                        addFrontFace(buffer_data, coord, bt);
                        faces++;
                    }
                    // back
                    if (checkFace(block_x,block_y,block_z-1)) {
                        addBackFace(buffer_data, coord, bt);
                        faces++;
                    }
                }
            }
        }
    }

    // Give our vertices and UVs to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, buffer_data.size() * sizeof(GLfloat), buffer_data.data(), GL_STATIC_DRAW);

    // Set the variable 'vert' in vertex shader 
    // Equals to the vertices for the block
    GLuint gVert = glGetAttribLocation(shaders, "vert");
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
    GLuint gVertTexCoord = glGetAttribLocation(shaders, "vertTexCoord");
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

    //glDisableVertexAttribArray(gVert);
    //glDisableVertexAttribArray(gVertTexCoord);
}

void Chunk::renderChunk(Coordinates selected) {

     int maxHeight = CHUNK_HEIGHT/2;
    int minHeight = -maxHeight;

    // update frustum to current view
    frustum.getFrustum(camera->view(), camera->projection());

    int found = 0;
    for (int i = 0; i < 16; i++) {
        if ((frustum.cubeInFrustum(chunk_x,minHeight + (i*16),chunk_z,chunk_x+CHUNK_SIZE,minHeight + ((i+1)*16) -1,chunk_z + CHUNK_SIZE, CHUNK_SIZE/2))) {
            found = 1;
            break;
        } 
    }

    if (!found)
        return;

    //chunksNum++;
    glUseProgram(shaders);

    // // bind the texture and set the "tex" uniform in the fragment shader -- *** could I potentially just bind once since I'm only using 1 texture file? ***
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->object());
    // // Get the id for the uniform variable "tex"
    GLint tex = glGetUniformLocation(shaders, "tex");
    glUniform1i(tex, 0);

    // TODO: Implement selected block
    GLint s = glGetUniformLocation(shaders, "selected");
    glm::vec3 select = glm::vec3(selected.x, selected.y, selected.z);
    glUniform3fv(s, 1, &select[0]);

    // set up the camera
    GLint cameraMatrix = glGetUniformLocation(shaders, "camera");
    glm::mat4 cameraView = camera->matrix();
 	glUniformMatrix4fv(
 		cameraMatrix,	// Id of this uniform variable
 		1,			    // Number of matrices
 		GL_FALSE,	    // Transpose
 		&cameraView[0][0]	// The location of the data
 	);
        
    // bind the VAO (the triangle)
    glBindVertexArray(vao);

    // draw the triangles 
    glDrawArrays(GL_TRIANGLES, 0, faces*6);
    
    // unbind the VAO
    glBindVertexArray(0);
    
    // unbind the program
    glUseProgram(0);
}

int Chunk::chunksNum;

// only send block faces that aren't covered to the GPU
bool Chunk::checkFace(int x, int y, int z) {

    if (x < 0 || x > 15 || y < 0 || y > 255 || z < 0 || z > 15) {
        return false;
    }

    return (blocks[x][y][z] == BlockType::AIR);
}

// check if the block's neighbour is air
bool Chunk::checkNeighbour(int x, int y, int z) {

    // right
    if (x > 15 && !neighbours[0].isEmpty()) 
        return neighbours[0].getBlockNormalized(Coordinates(0, y, z)) == BlockType::AIR;

    // left
    if (x < 0 && !neighbours[1].isEmpty()) 
        return neighbours[1].getBlockNormalized(Coordinates(15, y, z)) == BlockType::AIR;

    // front
    if (z > 15 && !neighbours[2].isEmpty()) 
        return neighbours[2].getBlockNormalized(Coordinates(x, y, 0)) == BlockType::AIR;

    // back
    if (z < 0 && !neighbours[3].isEmpty()) 
        return neighbours[3].getBlockNormalized(Coordinates(x, y, 15)) == BlockType::AIR;

    if (x < 0 || x > 15 || y < 0 || y > 255 || z < 0 || z > 15) {
        return false;
    }
    return (blocks[x][y][z] == BlockType::AIR);
}

void Chunk::addBlock(Coordinates blockCoord, BlockType bt) {

     // convert to 0 .. 15 / 255
    int block_x = blockCoord.x - chunk_x;
    int block_y = blockCoord.y + CHUNK_HEIGHT/2;
    int block_z = blockCoord.z - chunk_z;

    blocks[block_x][block_y][block_z] = bt;
}

void Chunk::removeBlock(Coordinates blockCoord) {

    // convert to 0 .. 15 / 255
    int block_x = blockCoord.x - chunk_x;
    int block_y = blockCoord.y + CHUNK_HEIGHT/2;
    int block_z = blockCoord.z - chunk_z;

    blocks[block_x][block_y][block_z] = BlockType::AIR;
}

BlockType Chunk::getBlock(Coordinates blockCoord) {

    // convert to 0 .. 15 / 255
    int block_x = blockCoord.x - chunk_x;
    int block_y = blockCoord.y + CHUNK_HEIGHT/2;
    int block_z = blockCoord.z - chunk_z;

    return blocks[block_x][block_y][block_z];
}

BlockType Chunk::getBlockNormalized(Coordinates blockCoord) {

    return blocks[blockCoord.x][blockCoord.y][blockCoord.z];
}
