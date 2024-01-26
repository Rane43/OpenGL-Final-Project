#ifndef model_h
#define model_h

#include <vector>
#include <maths_funcs.h>
#include <material.h>

// Assimp includes
#include <assimp/cimport.h> // scene importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations

class Model
{
protected:
	unsigned int rendererID = 0;
	unsigned int textureID = 0;
	int shaderProgram = -1;

	unsigned int vp_vbo = 0;
	unsigned int vn_vbo = 0;
	unsigned int vt_vbo = 0;

	vec3 position = vec3(0.0f, 0.0f, 0.0f);

	// vec3 forwardVector = vec3(1.0f, 0.0f, 0.0f);
	// vec3 upVector = vec3(0.0f, 1.0f, 0.0f);

	vec3 rotation = vec3(0.0f, 0.0f, 0.0f);
	vec3 velocity = vec3(0.0f, 0.0f, 0.0f);

	size_t mPointCount = 0;
	std::vector<vec3> mVertices;
	std::vector<vec3> mNormals;
	std::vector<vec2> mTextureCoords;

	Material material = Material();

	void generateMesh(unsigned int shaderProgramID);

	void loadMesh(const aiMesh*);

	void loadMeshes(const char* filepath);

public:
	Model(const aiMesh* mesh, unsigned int shaderProgramID);

	Model(const char* filepath, unsigned int shaderProgramID);


	// Setter methods

	void updateMaterial(Material newMaterial);
	void addTexture(const char* texture_path);

	void updateModelPosition(vec3 pos);
	void updateModelRotation(vec3 rot);

	void moveModel(vec3 movement);

	void rotate_x(float x_degs);
	void rotate_y(float y_degs);
	void rotate_z(float z_degs);
	

	// Getter methods
	mat4 getModelMatrix();

	vec3 getPosition();
	vec3 getRotation();
	vec3 getVelocity();

	size_t getPointCount();

	int getRendererID();
	int getShaderID();
	int getTextureID();

	Material getMaterial();

	


};

#endif