
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "chunk.h"
#include "util.h"

// set x and z value to appropriate positions
Chunk::Chunk(int xx, int zz, Camera *cam, BlockType ***block) {
    x = xx * 16;
    z = zz * 16;

    // each chunk has it's own perlin noise
    perlinNoise.SetNoiseType(FastNoise::Perlin); 
    perlinNoise.SetSeed(123);

    texture = LoadTexture("texture.png");
    shaders = LoadShaders( "shaders/block_vertex.glsl", "shaders/block_fragment.glsl" );
    camera = cam;
    blocksA = block;
}

Chunk::~Chunk() {

    // not properly working, have to look into it - seg fault

    // delete memory
    // for(int i = 0; i < CHUNK_HEIGHT; i++)
    // {
    //     for(int j(0); j < CHUNK_SIZE; j++)
    //         delete[] blocksA[i][j];
    //     delete[] blocksA[i];
    // }
    // delete[] blocksA;
}

void Chunk::updateBlock(Coordinates blockCoord) {
    // // height between -128 to 128
    // int maxHeight = CHUNK_HEIGHT/2;
    // int minHeight = -maxHeight;

    // glGenBuffers(1, &vbo);
    // glGenVertexArrays(1, &vao);

    // // Vertex Array Object - will cause everything underneath to be bound to the VAO
    // // Makes it so you can just bind VAO and all corresponding will be loaded as well
	// glBindVertexArray(vao);

    // // Vertex Buffer Object
    // glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // buffer_data.clear();

    // Coordinates coord(0,0,0);
    // addTopFace(buffer_data, coord, BlockType::GRASS);

    // // fill chunk vao
    // for (int xx = x; xx < (x+CHUNK_SIZE); xx++) {
    //     for (int zz = z; zz < (z+CHUNK_SIZE); zz++) {
    //         for (int yy = minHeight; yy < maxHeight; yy++) {

    //             Coordinates coord(xx,yy,zz);
    //             BlockType bt = blocksA[xx][yy][zz];

    //             if (bt != BlockType::AIR) {
    //                 // top
    //                 if (checkFace(xx,yy+1,zz)) {
    //                     addTopFace(buffer_data, coord, bt);
    //                     faces++;
    //                 }
    //                 // bottom
    //                 if (checkFace(xx,yy-1,zz)) {
    //                     addBottomFace(buffer_data, coord, bt);
    //                     faces++;
    //                 }
    //                 // right
    //                 if (checkFace(xx+1,yy,zz)) {
    //                     addRightFace(buffer_data, coord, bt);
    //                     faces++;
    //                 }
    //                 // left
    //                 if (checkFace(xx-1,yy,zz)) {
    //                     addLeftFace(buffer_data, coord, bt);
    //                     faces++;
    //                 }
    //                 // front
    //                 if (checkFace(xx,yy,zz+1)) {
    //                     addFrontFace(buffer_data, coord, bt);
    //                     faces++;
    //                 }
    //                 // back
    //                 if (checkFace(xx,yy,zz-1)) {
    //                     addBackFace(buffer_data, coord, bt);
    //                     faces++;
    //                 }
    //             }
    //         }
    //     }
    // }

    // // Give our vertices and UVs to OpenGL.
    // glBufferData(GL_ARRAY_BUFFER, buffer_data.size() * sizeof(GLfloat), buffer_data.data(), GL_STATIC_DRAW);

    // // Set the variable 'vert' in vertex shader 
    // // Equals to the vertices for the block
    // GLuint gVert = glGetAttribLocation(shaders, "vert");
    // glEnableVertexAttribArray(gVert);
    // glVertexAttribPointer(
    //     0,                  // attribute 0
    //     3,                  // size
    //     GL_FLOAT,           // type
    //     GL_FALSE,           // normalized?
    //     5*sizeof(GLfloat),  // stride
    //     (void*)0            // array buffer offset
    // );

    // // Set the variable 'vertTexCoord' in vertex shader 
    // // Equals to the UV values for corresponding vertices to the block
    // GLuint gVertTexCoord = glGetAttribLocation(shaders, "vertTexCoord");
    // glEnableVertexAttribArray(gVertTexCoord);
    // glVertexAttribPointer(
    //     1,                  // attribute 1
    //     2,                  // size
    //     GL_FLOAT,           // type
    //     GL_FALSE,           // normalized?
    //     5*sizeof(GLfloat),  // stride
    //     (const GLvoid*)(3 * sizeof(GLfloat))    // array buffer offset
    // );

    // // unbind VAO and VBO 
    // glBindVertexArray(0);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);

    // //glDisableVertexAttribArray(gVert);
    // //glDisableVertexAttribArray(gVertTexCoord);
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

    for (int xx = x; xx < (x+CHUNK_SIZE); xx++) {
        for (int zz = z; zz < (z+CHUNK_SIZE); zz++) {

            float height = round(perlinNoise.GetNoise(xx,zz) * 10);
            for (int yy = minHeight; yy < maxHeight; yy++) {

                BlockType bt;

                if (yy > height) {
                    bt = BlockType::AIR;
                } else if (yy == height) {
                    bt = BlockType::GRASS;
                } else if (yy <= -8 && yy > -127) {
                    bt = BlockType::COBBLESTONE;
                } else if (yy == -127) {
                    bt = BlockType::BEDROCK;
                } else {
                    bt = BlockType::DIRT;
                }

                //blocks.insert (std::make_pair<Coordinates, BlockType>(Coordinates(xx, yy, zz),bt));

                int xxx = 0;
                int yyy = 0;
                int zzz = 0;
                // convert to 0 .. 15 / 255
                yyy = yy + CHUNK_HEIGHT/2;

                //if (xx < 0) {
                    xxx = xx -x;
                // } else {
                //     xxx = xx;
                // }

                //if (zz < 0) {
                     zzz = zz -z;

                //} else {
                    //zzz = zz;
                //}
                blocksA[xxx][yyy][zzz] = bt;

            }
        }
    }
    
    // fill chunk vao
    for (int xx = x; xx < (x+CHUNK_SIZE); xx++) {
        for (int zz = z; zz < (z+CHUNK_SIZE); zz++) {
            for (int yy = minHeight; yy < maxHeight; yy++) {

                Coordinates coord(xx,yy,zz);

                int xxx = 0;
                int yyy = 0;
                int zzz = 0;
                // convert to 0 .. 15 / 255
                yyy = yy + CHUNK_HEIGHT/2;

                //if (xx < 0) {
                    xxx = xx - x;
                // } else {
                //     xxx = xx;
                // }

                //if (zz < 0) {
                        zzz = zz - z;
                // } else {
                //     zzz = zz;
                // }


                BlockType bt = blocksA[xxx][yyy][zzz];

        
                if (bt != BlockType::AIR) {
                    // top
                    
                    if (checkFace(xxx,yyy+1,zzz)) {
                        addTopFace(buffer_data, coord, bt);
                        faces++;
                    }
                    // bottom
                    if (checkFace(xxx,yyy-1,zzz)) {
                        addBottomFace(buffer_data, coord, bt);
                        faces++;
                    }
                    // right
                    if (checkFace(xxx+1,yyy,zzz)) {
                        addRightFace(buffer_data, coord, bt);
                        faces++;
                    }
                    // left
                    if (checkFace(xxx-1,yyy,zzz)) {
                        addLeftFace(buffer_data, coord, bt);
                        faces++;
                    }
                    // front
                    if (checkFace(xxx,yyy,zzz+1)) {
                        addFrontFace(buffer_data, coord, bt);
                        faces++;
                    }
                    // back
                    if (checkFace(xxx,yyy,zzz-1)) {
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

void Chunk::renderChunk() {
    
    glUseProgram(shaders);

    // // bind the texture and set the "tex" uniform in the fragment shader -- *** could I potentially just bind once since I'm only using 1 texture file? ***
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->object());
    // // Get the id for the uniform variable "tex"
    GLint tex = glGetUniformLocation(shaders, "tex");
    glUniform1i(tex, 0);

    // TODO: Implement selected block
    //GLint selected = glGetUniformLocation(shaders, "selected");
    //glUniform1fv(selected, 1, &inst.selected);

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

// only send block faces that aren't covered to the GPU
bool Chunk::checkFace(int xx, int yy, int zz) {

    // will have to use chunk manager to find block
    // since I don't have a way to look at blocks in different chunks, always render the edge of the chunk blocks
    // if (xx > (x+CHUNK_SIZE) || xx < x || zz < z || zz > (z+CHUNK_SIZE) || yy < -(CHUNK_HEIGHT/2) || yy > CHUNK_HEIGHT/2) {
    //     return true;
    // }
    // int xxx = 0;
    // int yyy = 0;
    // int zzz = 0;
    
    // // convert to 0 .. 15 / 255
    // yyy = yy + CHUNK_HEIGHT/2;

    // if (xx < 0) {
    //     xxx = xx + CHUNK_SIZE;
    // }

    // if (zz < 0) {
    //         zzz = zz + CHUNK_SIZE;

    // }

    // if (xx > (x+CHUNK_SIZE) || xx < x || zz < z || zz > (z+CHUNK_SIZE) || yy < -(CHUNK_HEIGHT/2) || yy > CHUNK_HEIGHT/2) {
    //     return true;
    // }

    if (xx < 0 || xx > 15 || yy < 0 || yy > 255 || zz < 0 || zz > 15) {
            //std::cout << xx << " " << yy << " " << zz << std::endl;

        return true;
    }

    return (blocksA[int(xx)][int(yy)][int(zz)] == BlockType::AIR);
    //return (blocks[Coordinates(xx, yy, zz)] == BlockType::AIR);
}

void Chunk::addBlock(Coordinates blockCoord, BlockType bt) {
    //blocks.at(Coordinates(blockCoord.x, blockCoord.y, blockCoord.z)) = bt;
    blocksA[blockCoord.x][blockCoord.y][blockCoord.z] = bt;
}

void Chunk::removeBlock(Coordinates blockCoord) {
    //blocks.erase(blockCoord);
    blocksA[blockCoord.x][blockCoord.y][blockCoord.z] = BlockType::AIR;
}

BlockType Chunk::getBlock(Coordinates blockCoord) {
    //return blocksA[blockCoord.x][blockCoord.y][blockCoord.z];
    return BlockType::GRASS;
    //return blocks[blockCoord];
}
