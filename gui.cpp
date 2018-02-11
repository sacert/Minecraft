#include "gui.h"
#include "shader.h"
#include "util.h"

void GUI::LoadGUI() {

    // set all the elements of block
    shaders = LoadShaders( "shaders/gui_vertex.glsl", "shaders/gui_fragment.glsl" );
    texture = LoadTexture("crosshair.png");
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

    // UV values
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
}

void GUI::RenderCrosshair() {

    // use program - shaders
    glUseProgram(shaders);

    // active and bind textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->object());

    // scaling crosshair
    GLint scale = glGetUniformLocation(shaders, "scale");
    glUniform2f(scale, scaleX, scaleY);

    // texture values
    GLint tex = glGetUniformLocation(shaders, "tex");
    glUniform1i(tex, 0);
        
    // bind the VAO and draw everything inside
    glBindVertexArray(guiVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // unbind VAO and program (shaders)
    glBindVertexArray(0);
    glUseProgram(0);
}