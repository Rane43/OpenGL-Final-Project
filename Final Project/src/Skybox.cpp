#include <Skybox.h>
#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <maths_funcs.h>

void Skybox::genTextureSkyBox()
{
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < 6; i++)
    {
        unsigned char* data = stbi_load(textures[i], &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            std::cout << "Skybox textures not found" << "\n";
            stbi_image_free(data);
        }
    
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

}

void Skybox::genMeshData()
{
    glUseProgram(shaderID);

    // Vao
    glGenVertexArrays(1, &rendererID);
    glGenBuffers(1, &vp_vbo); 
    
    glBindVertexArray(rendererID);

    glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    // Position passed to GPU
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*) 0);

    glBindVertexArray(0);
    glUseProgram(0);

}

unsigned int Skybox::getShaderID() {
    return shaderID;
}

unsigned int Skybox::getRendererID()
{
    return rendererID;
}

unsigned int Skybox::getTextureID()
{
    return textureID;
}

Skybox::Skybox(unsigned int shaderID, const char* texture1, const char* texture2, const char* texture3, const char* texture4, const char* texture5, const char* texture6)
{
    this->shaderID = shaderID;

    textures[0] = texture1;
    textures[1] = texture2;
    textures[2] = texture3;
    textures[3] = texture4;
    textures[4] = texture5;
    textures[5] = texture6;

    genTextureSkyBox();

    genMeshData();


}