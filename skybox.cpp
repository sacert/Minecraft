#include "skybox.hpp"
#include "shader.hpp"
#include <glm/glm.hpp>

void SkyBox::LoadSkyBox() {
    shaders = LoadShaders( "shaders/skybox_vertex.glsl", "shaders/skybox_fragment.glsl" );

    GLfloat skyboxVertices[] = {
        // Positions
        // Back face
            -1.0f, -1.0f, -1.0f,    // Bottom-left
            1.0f, 1.0f, -1.0f,      // top-right
            1.0f, -1.0f, -1.0f,     // bottom-right
            1.0f, 1.0f, -1.0f,      // top-right
            -1.0f, -1.0f, -1.0f,    // bottom-left
            -1.0f, 1.0f, -1.0f,     // top-left
            // Front face
            -1.0f, -1.0f, 1.0f,     // bottom-left
            1.0f, -1.0f, 1.0f,      // bottom-right
            1.0f, 1.0f, 1.0f,       // top-right
            1.0f, 1.0f, 1.0f,       // top-right
            -1.0f, 1.0f, 1.0f,      // top-left
            -1.0f, -1.0f, 1.0f,     // bottom-left
            // Left face
            -1.0f, 1.0f, 1.0f,      // top-right
            -1.0f, 1.0f, -1.0f,     // top-left
            -1.0f, -1.0f, -1.0f,    // bottom-left
            -1.0f, -1.0f, -1.0f,    // bottom-left
            -1.0f, -1.0f, 1.0f,     // bottom-right
            -1.0f, 1.0f, 1.0f,      // top-right
            // Right face
            1.0f, 1.0f, 1.0f,       // top-left
            1.0f, -1.0f, -1.0f,     // bottom-right
            1.0f, 1.0f, -1.0f,      // top-right
            1.0f, -1.0f, -1.0f,     // bottom-right
            1.0f, 1.0f, 1.0f,       // top-left
            1.0f, -1.0f, 1.0f,      // bottom-left
            // Bottom face
            -1.0f, -1.0f, -1.0f,    // top-right
            1.0f, -1.0f, -1.0f,     // top-left
            1.0f, -1.0f, 1.0f,      // bottom-left
            1.0f, -1.0f, 1.0f,      // bottom-left
            -1.0f, -1.0f, 1.0f,     // bottom-right
            -1.0f, -1.0f, -1.0f,    // top-right
            // Top face
            -1.0f, 1.0f, -1.0f,     // top-left
            1.0f, 1.0f, 1.0f,       // bottom-right
            1.0f, 1.0f, -1.0f,      // top-right
            1.0f, 1.0f, 1.0f,       // bottom-right
            -1.0f, 1.0f, -1.0f,     // top-left
            -1.0f, 1.0f, 1.0f,      // bottom-left
    };

    // Setup skybox VAO
    glGenVertexArrays( 1, &skyboxVAO );
    glGenBuffers( 1, &skyboxVBO );
    glBindVertexArray( skyboxVAO );
    glBindBuffer( GL_ARRAY_BUFFER, skyboxVBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( skyboxVertices ), &skyboxVertices, GL_STATIC_DRAW );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( GLfloat ), ( GLvoid * ) 0 );
    glBindVertexArray(0);
}

void SkyBox::Render(Camera camera) {
    glm::mat4 view = camera.view();
    glm::mat4 model;
        
    glDepthFunc( GL_LEQUAL );  // Change depth function so depth test passes when values are equal to depth buffer's content
    glUseProgram(shaders);

    glUniformMatrix4fv( glGetUniformLocation( shaders, "projection" ), 1, GL_FALSE, &camera.projection()[0][0] );      
    glUniformMatrix4fv( glGetUniformLocation( shaders, "view" ), 1, GL_FALSE,  &view[0][0] ) ;
    glUniformMatrix4fv( glGetUniformLocation( shaders, "model" ), 1, GL_FALSE,  &model[0][0] ) ;

    // skybox cube
    glBindVertexArray( skyboxVAO );
    glDrawArrays( GL_TRIANGLES, 0, 36 );
    glBindVertexArray( 0 );
    glDepthFunc( GL_LESS ); // Set depth function back to default
}