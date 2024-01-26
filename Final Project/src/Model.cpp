#include <model.h>
#include <material.h>
#include <iostream>
#include <vector>
#include <maths_funcs.h>

// Assimp includes
#include <assimp/cimport.h> // scene importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations

// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>

// Texture include
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


// Load single mesh
void Model::loadMesh(const aiMesh* mesh) {
	printf("    %i vertices in mesh\n", mesh->mNumVertices);
	mPointCount += mesh->mNumVertices;
	for (unsigned int v_i = 0; v_i < mesh->mNumVertices; v_i++) {
		if (mesh->HasPositions()) {
			const aiVector3D* vp = &(mesh->mVertices[v_i]);
			mVertices.push_back(vec3(vp->x, vp->y, vp->z));
		}
		if (mesh->HasNormals()) {
			const aiVector3D* vn = &(mesh->mNormals[v_i]);
			mNormals.push_back(vec3(vn->x, vn->y, vn->z));
		}
		if (mesh->HasTextureCoords(0)) {
			const aiVector3D* vt = &(mesh->mTextureCoords[0][v_i]);
			mTextureCoords.push_back(vec2(vt->x, vt->y));
		}
		if (mesh->HasTangentsAndBitangents()) {
			/* You can extract tangents and bitangents here              */
			/* Note that you might need to make Assimp generate this     */
			/* data for you. Take a look at the flags that aiImportFile  */
			/* can take.                                                 */
		}
	}
}

Model::Model(const aiMesh* mesh, unsigned int shaderProgramID) {
	shaderProgram = shaderProgramID;
	std::cout << "Shader Program ID: " << shaderProgram << "\n";
	loadMesh(mesh);
	generateMesh(shaderProgramID);
	std::cout << std::endl;

}

Model::Model(const char* filepath, unsigned int shaderProgramID) {
	shaderProgram = shaderProgramID;
	std::cout << "Shader Program ID: " << shaderProgram << "\n";
	loadMeshes(filepath);
	generateMesh(shaderProgramID);
	std::cout << std::endl;
};


void Model::loadMeshes(const char* file_name) {
	/* Use assimp to read the model file, forcing it to be read as    */
	/* triangles. The second flag (aiProcess_PreTransformVertices) is */
	/* relevant if there are multiple meshes in the model file that   */
	/* are offset from the origin. This is pre-transform them so      */
	/* they're in the right position.                                 */
	const aiScene* scene = aiImportFile(
		file_name,
		aiProcess_Triangulate | aiProcess_PreTransformVertices
	);

	if (!scene) {
		fprintf(stderr, "ERROR: reading mesh %s\n", file_name);
		return;
	}

	printf("  %i materials\n", scene->mNumMaterials);
	printf("  %i meshes\n", scene->mNumMeshes);
	printf("  %i textures\n", scene->mNumTextures);

	for (unsigned int m_i = 0; m_i < scene->mNumMeshes; m_i++) {
		const aiMesh* mesh = scene->mMeshes[m_i];
		loadMesh(mesh);
	}

	aiReleaseImport(scene);
};

void Model::generateMesh(unsigned int shaderProgramID) {
		/* COPY INTO BUFFERS */
		glUseProgram(shaderProgram);
		
		int loc1 = glGetAttribLocation(shaderProgram, "vertex_position");
		int loc2 = glGetAttribLocation(shaderProgram, "vertex_normal");
		int loc3 = glGetAttribLocation(shaderProgram, "vertex_texture");

		// Error check to see if vertex data was found in shader
		if (loc1 == -1 || loc2 == -1 || loc3 == -1) {
			std::cout << "Could not find vertex data in shader" << "\n";
		}

		// Positions Buffer
		glGenBuffers(1, &vp_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
		glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(vec3), &mVertices[0], GL_STATIC_DRAW);


		// Normals Buffer
		glGenBuffers(1, &vn_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
		glBufferData(GL_ARRAY_BUFFER, mNormals.size() * sizeof(vec3), &mNormals[0], GL_STATIC_DRAW);


		// Texture Buffer
		glGenBuffers(1, &vt_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vt_vbo);
		glBufferData(GL_ARRAY_BUFFER, mTextureCoords.size() * sizeof(vec2), &mTextureCoords[0], GL_STATIC_DRAW);



		// Vao
		glGenVertexArrays(1, &rendererID);
		glBindVertexArray(rendererID);


		// Position and Normal Data passing to GPU
		glEnableVertexAttribArray(loc1);
		glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
		glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(loc2);
		glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
		glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glBindVertexArray(0);
		glUseProgram(0);
};


// Setter methods
void Model::updateModelPosition(vec3 pos)
{
	position = pos;
};

void Model::updateModelRotation(vec3 rot)
{
	rotation = rot;
};

void Model::moveModel(vec3 movement)
{
	position = position + movement;
};

void Model::rotate_x(float x_degs)
{
	rotation.v[0] = rotation.v[0] + x_degs;
};

void Model::rotate_y(float y_degs)
{
	rotation.v[1] = rotation.v[1] + y_degs;
};

void Model::rotate_z(float z_degs)
{
	rotation.v[2] = rotation.v[2] + z_degs;
};


void Model::updateMaterial(Material newMaterial)
{
	material = newMaterial;
};

void Model::addTexture(const char* texture_path)
{
	glUseProgram(shaderProgram);
	glBindVertexArray(rendererID);

	int loc3 = glGetAttribLocation(shaderProgram, "vertex_texture");

	// Texture
	glGenTextures(1, &textureID);
	std::cout << textureID << "\n";
	glBindTexture(GL_TEXTURE_2D, textureID);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load and generate the texture
	int width, height, nrChannels;
	unsigned char* textureData = stbi_load(texture_path, &width, &height, &nrChannels, 0);

	// Sampler2D unifrom ?
	if (textureData)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(textureData);

	// Texture data passed to GPU
	glEnableVertexAttribArray(loc3);
	glBindBuffer(GL_ARRAY_BUFFER, vt_vbo);
	glVertexAttribPointer(loc3, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindVertexArray(0);
	glUseProgram(0);

}


// Getter Methods
mat4 Model::getModelMatrix()
{
	mat4 model = identity_mat4();
	model = rotate_x_deg(model, rotation.v[0]);
	model = rotate_y_deg(model, rotation.v[1]);
	model = rotate_z_deg(model, rotation.v[2]);
	model = translate(model, position);

	return model;
};

size_t Model::getPointCount() {
	return mPointCount;
};

int Model::getRendererID() {
	return rendererID;
};


Material Model::getMaterial() {
	return material;
}

vec3 Model::getPosition()
{
	return position;
}

vec3 Model::getRotation()
{
	return rotation;
}

vec3 Model::getVelocity() {
	return velocity;
};


int Model::getShaderID()
{
	return shaderProgram;
}

int Model::getTextureID() {
	return textureID;
}
