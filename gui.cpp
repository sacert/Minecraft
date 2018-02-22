#include "gui.h"
#include "shader.h"
#include "util.h"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>

GUI::GUI() {

     // set all the elements of block
    crosshair_shaders = LoadShaders( "shaders/gui_vertex.glsl", "shaders/gui_fragment.glsl" );
    crosshair_texture = LoadTexture("crosshair.png");

    // set all the elements of block
    inventory_texture = LoadTexture("texture.png");
    inventory_shaders = LoadShaders( "shaders/block_vertex.glsl", "shaders/block_fragment.glsl" );
}


void GUI::LoadCrosshair() {

    scaleX = 0.15 * (SCREEN_SIZE.y/SCREEN_SIZE.x); // by multiply by screen ratio, scale x and y will make a square
    scaleY = 0.15;
    glGenBuffers(1, &guiVBO);
    glGenVertexArrays(1, &guiVAO);

	glBindVertexArray(guiVAO);
    glBindBuffer(GL_ARRAY_BUFFER, guiVBO);

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
    GLuint gVert = glGetAttribLocation(crosshair_shaders, "vert");
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
    GLuint gVertTexCoord = glGetAttribLocation(crosshair_shaders, "vertTexCoord");
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

void GUI::RenderCrosshair() {

    // use program - shaders
    glUseProgram(crosshair_shaders);

    // active and bind textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, crosshair_texture->object());

    // scaling crosshair
    GLint scale = glGetUniformLocation(crosshair_shaders, "scale");
    glUniform2f(scale, scaleX, scaleY);

    // texture values
    GLint tex = glGetUniformLocation(crosshair_shaders, "tex");
    glUniform1i(tex, 0);
        
    // bind the VAO and draw everything inside
    glBindVertexArray(guiVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // unbind VAO and program (shaders)
    glBindVertexArray(0);
    glUseProgram(0);
}

void GUI::LoadInventory(BlockType selected) {
    
    glGenBuffers(1, &inventoryVBO);
    glGenVertexArrays(1, &inventoryVAO);

    // Vertex Array Object - will cause everything underneath to be bound to the VAO
    // Makes it so you can just bind VAO and all corresponding will be loaded as well
	glBindVertexArray(inventoryVAO);

    // Vertex Buffer Object
    glBindBuffer(GL_ARRAY_BUFFER, inventoryVBO);

    BlockType bt = selected;

    Coordinates coord(-1,-1,0);
    std::vector<GLfloat> buffer_data;

    addTopFace(buffer_data, coord, bt);
    addBottomFace(buffer_data, coord, bt);
    addRightFace(buffer_data, coord, bt);
    addLeftFace(buffer_data, coord, bt);
    addFrontFace(buffer_data, coord, bt);
    addBackFace(buffer_data, coord, bt);
    
    // Give our vertices and UVs to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, buffer_data.size() * sizeof(GLfloat), buffer_data.data(), GL_STATIC_DRAW);

    // Set the variable 'vert' in vertex shader 
    // Equals to the vertices for the block
    GLuint gVert = glGetAttribLocation(inventory_shaders, "vert");
    glEnableVertexAttribArray(gVert);
    glVertexAttribPointer(
        0,                  // attribute 0
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        8*sizeof(GLfloat),  // stride
        (void*)0            // array buffer offset
    );

    // Set the variable 'vertTexCoord' in vertex shader 
    // Equals to the UV values for corresponding vertices to the block
    GLuint gVertTexCoord = glGetAttribLocation(inventory_shaders, "vertTexCoord");
    glEnableVertexAttribArray(gVertTexCoord);
    glVertexAttribPointer(
        1,                  // attribute 1
        2,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        8*sizeof(GLfloat),  // stride
        (const GLvoid*)(3 * sizeof(GLfloat))    // array buffer offset
    );

    // Equals to the normal values for corresponding vertices to the block
    GLuint gVertNormal = glGetAttribLocation(inventory_shaders, "vertNormal");
    glEnableVertexAttribArray(gVertNormal);
    glVertexAttribPointer(
        2,                  // attribute 1
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        8*sizeof(GLfloat),  // stride
        (const GLvoid*)(5 * sizeof(GLfloat))    // array buffer offset
    );

    // unbind VAO and VBO 
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GUI::RenderInventory() {

    // use program - shaders
    glUseProgram(inventory_shaders);

    // active and bind textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inventory_texture->object());

    // scaling crosshair
    //GLint scale = glGetUniformLocation(shaders, "scale");
    //glUniform2f(scale, scaleX, scaleY);

    // texture values
    GLint tex = glGetUniformLocation(inventory_shaders, "tex");
    glUniform1i(tex, 0);

    glm::mat4 angle = glm::rotate(glm::mat4(1.0f), glm::radians(15.0f), glm::vec3(1,0,0)); // rotate camera down
    angle = glm::rotate(angle, glm::radians(40.0f), glm::vec3(0,1,0));  // rotate camera to the side
    angle = glm::scale(angle, glm::vec3(0.11f, 0.15f, 0.11f));  // scale it so it isn't so big
    angle = glm::translate(angle, glm::vec3(10, -6, 0));    // put it in the appropriate spot

    GLint viewMatrix = glGetUniformLocation(inventory_shaders, "camera");
 	glUniformMatrix4fv(
 		viewMatrix,	// Id of this uniform variable
 		1,			    // Number of matrices
 		GL_FALSE,	    // Transpose
 		&angle[0][0]	// The location of the data
 	);
        
    // bind the VAO and draw everything inside
    glBindVertexArray(inventoryVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6*6);
    
    // unbind VAO and program (shaders)
    glBindVertexArray(0);
    glUseProgram(0);

}