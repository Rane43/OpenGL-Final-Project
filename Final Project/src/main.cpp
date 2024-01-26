#pragma region includes
#define NOMINMAX
#include <limits>

// My includes
#include <shader.h>
#include <model.h>
#include <material.h>
#include <light.h>
#include <camera.h>
#include <boid.h>
#include <boids.h>
#include <Skybox.h>

// Windows includes (For Time, IO, etc.)
#include <time.h>
#include <windows.h>
#include <mmsystem.h>
#include <cctype>
#include <vector> // STL dynamic memory.

// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>

// Project includes
#include <maths_funcs.h>

// Assimp includes
#include <assimp/cimport.h> // scene importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations

// Irrklang includes
#include <Irrklang/irrKlang.h>
using namespace irrklang;

#pragma endregion includes

#pragma region macros
// Macro for rubbish bin asset
#define BIN "src\\assets\\rubbishBinCircular.dae"
#define STREET "src\\assets\\O'Connell's street - 6.dae"

#define DROIDBODY "src\\assets\\Droid.dae"
#define DROIDTOP "src\\assets\\Droid top.dae"

#define BIN_TEXTURE "src\\textures\\bin_texture.jpg"
#define BUILDING_TEXTURE "src\\textures\\brick texture.jpg"

#define WORLD_VERTEX_SHADER "src\\shaders\\worldVertexShader.vert"

#define MAX_NUM_LIGHTS 10

#define SIRENMP3 "src\\mp3s\\Siren.mp3"
#pragma endregion macros


// Global variables
#pragma region Global variables
// Skybox
Skybox* skybox;

// Shaders
std::vector<Shader*> shaders;

// Models
std::vector<Model*> models;

// Camera and Lights
Camera* camera;
std::vector<Light*> lights;

float lightPositionsArray[MAX_NUM_LIGHTS * 3];
float lightColorsArray[MAX_NUM_LIGHTS * 3];
float lightIntensitiesArray[MAX_NUM_LIGHTS];
int lightTypes[MAX_NUM_LIGHTS];

// Boids
Boids drones = Boids();


int width = 800;
int height = 600;

GLfloat rotate_y = 0.0f;

#pragma endregion Global variables



void createLightArrays() {
	for (int component = 0; component < 3; component++) {
		for (int i = 0; i < lights.size(); i++) {
			lightPositionsArray[3 * i + component] = lights[i]->getPosition().v[component];
			lightColorsArray[3 * i + component] = lights[i]->getColor().v[component];
		}
	}

	for (int i = 0; i < lights.size(); i++) {
		lightIntensitiesArray[i] = lights[i]->getIntensity();
		lightTypes[i] = lights[i]->getLightType();
		// std::cout << "Light type: " << lights[i]->getLightType() << "\n";
	}
}

void passUniformsToWorldShader(Model model) {
	int shaderID = model.getShaderID();
	glUseProgram(shaderID);

	//Declare your uniform variables that will be used in your shader
	int matrix_location = glGetUniformLocation(shaderID, "model");
	int view_mat_location = glGetUniformLocation(shaderID, "view");
	int proj_mat_location = glGetUniformLocation(shaderID, "proj");

	int ambientLoc = glGetUniformLocation(shaderID, "material.ambientCI");
	int diffuseLoc = glGetUniformLocation(shaderID, "material.diffuseCI");
	int specLoc = glGetUniformLocation(shaderID, "material.specularCI");
	int phongLocation = glGetUniformLocation(shaderID, "material.phong");

	// Error Check
	if (matrix_location == -1 || view_mat_location == -1 || proj_mat_location == -1) {
		std::cout << "Matrix uniform not found" << "\n";
	}

	if (ambientLoc == -1 || diffuseLoc == -1 || specLoc == -1 || phongLocation == -1) {
		std::cout << "Material uniform not found \n";
	}

	// Root of the Hierarchy
	mat4 viewMatrix = camera->getViewMatrix();
	mat4 perspProjMatrix = camera->getProjectionMatrix();
	mat4 modelMatrix = model.getModelMatrix();


	// Matrix Uniforms
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, perspProjMatrix.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, viewMatrix.m);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, modelMatrix.m);

	// Private these properties?
	vec3 ambientCI = model.getMaterial().ambientCI;
	vec3 diffuseCI = model.getMaterial().diffuseCI;
	vec3 specularCI = model.getMaterial().specularCI;
	glUniform3f(ambientLoc, ambientCI.v[0], ambientCI.v[1], ambientCI.v[2]);
	glUniform3f(diffuseLoc, diffuseCI.v[0], diffuseCI.v[1], diffuseCI.v[2]);
	glUniform3f(specLoc, specularCI.v[0], specularCI.v[1], specularCI.v[2]);
	glUniform1i(phongLocation, model.getMaterial().phong);


	// Light uniforms
	int numLightsLoc = glGetUniformLocation(shaderID, "numLights");
	int numLights = lights.size();
	glUniform1i(numLightsLoc, numLights);

	int lightPositionsLoc = glGetUniformLocation(shaderID, "lightPositions");
	int lightIntensitiesLoc = glGetUniformLocation(shaderID, "lightIntensities");
	int lightColorsLoc = glGetUniformLocation(shaderID, "lightColors");
	int lightType = glGetUniformLocation(shaderID, "lightTypes");

	// Error Check
	if (lightPositionsLoc == -1 || lightIntensitiesLoc == -1 || lightColorsLoc == -1) {
		std::cout << "Light uniforms not found" << "\n";
	}

	glUniform3fv(lightPositionsLoc, MAX_NUM_LIGHTS, lightPositionsArray);
	glUniform1fv(lightIntensitiesLoc, MAX_NUM_LIGHTS, lightIntensitiesArray);
	glUniform3fv(lightColorsLoc, MAX_NUM_LIGHTS, lightColorsArray);
	glUniform1iv(lightType, MAX_NUM_LIGHTS, lightTypes);



	// Camera position
	int camPosLoc = glGetUniformLocation(shaderID, "camPos");
	vec3 camPos = camera->getPosition();
	glUniform3f(camPosLoc, camPos.v[0], camPos.v[1], camPos.v[2]);

	glUseProgram(0);
}

void passUniformsToLightShader(Light light) {
	int shaderID = light.getShaderID();
	glUseProgram(light.getShaderID());

	int matrix_location = glGetUniformLocation(shaderID, "model");
	int view_mat_location = glGetUniformLocation(shaderID, "view");
	int proj_mat_location = glGetUniformLocation(shaderID, "proj");

	// Root of the Hierarchy
	mat4 viewMatrix = camera->getViewMatrix();
	mat4 perspProjMatrix = camera->getProjectionMatrix();
	mat4 modelMatrix = light.getModelMatrix();

	// update uniforms & draw
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, perspProjMatrix.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, viewMatrix.m);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, modelMatrix.m);


	// Light uniforms
	int lightIntensity = glGetUniformLocation(shaderID, "lightIntensity");
	int lightColor = glGetUniformLocation(shaderID, "color");

	vec3 color = light.getColor();
	glUniform1f(lightIntensity, light.getIntensity());
	glUniform3f(lightColor, color.v[0], color.v[1], color.v[2]);

}


// Rendering
#pragma region Rendering
void renderSkybox() {
	glDepthFunc(GL_LEQUAL);

	int shaderID = skybox->getShaderID();
	glUseProgram(shaderID);

	int view_mat_location = glGetUniformLocation(shaderID, "view");
	int proj_mat_location = glGetUniformLocation(shaderID, "proj");

	// Error Check
	if (view_mat_location == -1 || proj_mat_location == -1) {
		std::cout << "Matrix uniform not found" << "\n";
	}

	mat4 viewMatrix = camera->getViewMatrix();
	// Remove translation of viewMatrix
	viewMatrix.m[3] = 0.0f; viewMatrix.m[7] = 0.0f;
	viewMatrix.m[11] = 0.0f; viewMatrix.m[12] = 0.0f;
	viewMatrix.m[13] = 0.0f; viewMatrix.m[14] = 0.0f;
	viewMatrix.m[15] = 0.0f;

	mat4 perspProjMatrix = camera->getProjectionMatrix();

	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, perspProjMatrix.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, viewMatrix.m);


	glBindVertexArray(skybox->getRendererID());
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->getTextureID());
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);

}

void render(Model model) {
	glBindTexture(GL_TEXTURE_2D, model.getTextureID());
	passUniformsToWorldShader(model);
	// Rebind shader
	glUseProgram(model.getShaderID());
	// bin->rotate_z(0.1f);
	glBindVertexArray(model.getRendererID());
	glDrawArrays(GL_TRIANGLES, 0, model.getPointCount());
}

float timestep() {
	static DWORD last_time = 0;
	DWORD curr_time = timeGetTime();
	if (last_time == 0)
		last_time = curr_time;
	float delta = (curr_time - last_time) * 0.001f;
	last_time = curr_time;

	return delta;
}

void display() {
	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// Display only point Lights
	for (int i = 0; i < lights.size(); i++) {
		if (lights[i]->getLightType() == POINT_LIGHT) {
			glUseProgram(lights[i]->getShaderID());
			passUniformsToLightShader(*lights[i]);
			glBindVertexArray(lights[i]->getRendererID());
			glDrawArrays(GL_TRIANGLES, 0, lights[i]->getPointCount());
		}
	}

	

	/* --------------- */
	// Update drone positions
	drones.update(timestep());

	/* --------------- */
	// Render Meshes
	for (int i = 0; i < models.size(); i++) {
		render(*models[i]);
	}


	// Display skybox last
	renderSkybox();


	glutSwapBuffers();
}

void updateScene() {
	// Draw the next frame
	glutPostRedisplay();
}
#pragma endregion Rendering



// User Interaction and Camera Control
#pragma region User Interaction and Camera Control
bool visible = true;
void camMovement(unsigned char key, int x, int y) {
	float prevYCoord = camera->getPosition().v[1];
	unsigned char keyL = std::tolower(key);
	float camSpeed = 10.0f;
	switch (keyL) {
	case 'w':
		camera->movePosition(vec3(0.0, 0.0, -1.0)*camSpeed);
		break;
	case 's':
		camera->movePosition(vec3(0.0, 0.0, 1.0)*camSpeed);
		break;
	case 'a':
		camera->movePosition(vec3(-1.0, 0.0, 0.0)*camSpeed);
		break;
	case 'd':
		camera->movePosition(vec3(1.0, 0.0, 0.0)*camSpeed);
		break;
	case 27:
		visible = true;
		glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
		break;
	}

	// Fix Camera along y-height
	// camera->setyPosition(prevYCoord);

}

int xPrev = width / 2;
int yPrev = height / 2;
void mousemove(int x, int y) {
	if (!visible) {
		float yCameraAngle = -(x - xPrev) / (float)width * 360.0f;
		float xCameraAngle = -(y - yPrev) / (float)height * 180.0f;
		camera->rotate_y(yCameraAngle);
		camera->rotate_x(xCameraAngle);
		glutWarpPointer(xPrev, yPrev);


	}
}

void mouseClicks(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		glutSetCursor(GLUT_CURSOR_NONE);
		glutWarpPointer(xPrev, yPrev);
		visible = false;

	}
}

#pragma endregion User Interaction and Camera Control




float randomNum(float a, float b) {
	float num = (float)rand() / RAND_MAX;
	return (b - a) * num + a;
}

void loadStreet(std::vector<Model*> &street, const char* file_name, unsigned int shaderProgramID) {
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

	std::cout << "The number of street models is: " << scene->mNumMeshes << "\n";

	for (unsigned int m_i = 0; m_i < scene->mNumMeshes; m_i++) {
		const aiMesh* mesh = scene->mMeshes[m_i];
		Model* newMesh = new Model(mesh, shaderProgramID);
		street.push_back(newMesh);
	}

	aiReleaseImport(scene);
};


void init()
{
	#pragma region skybox
	Shader* skyboxShader = new Shader("src\\shaders\\skyboxVertexShader.vert", "src\\shaders\\skyboxFragmentShader.frag");
	shaders.push_back(skyboxShader);

	skybox = new Skybox(skyboxShader->getShaderID(), 
		"src\\textures\\skybox textures\\right.jpg",
		"src\\textures\\skybox textures\\left.jpg",
		"src\\textures\\skybox textures\\top.jpg",
		"src\\textures\\skybox textures\\bottom.jpg",
		"src\\textures\\skybox textures\\front.jpg",
		"src\\textures\\skybox textures\\back.jpg");


	#pragma endregion skybox
	
	#pragma region lights
	// - Lights -
	Shader* lightShader = new Shader("src\\shaders\\lightVertexShader.vert", "src\\shaders\\lightFragShader.frag");
	shaders.push_back(lightShader);

	// Point Lights
	Light* light1 = new Light(10.0f, vec3(-250.0f, 40.0f, 0.0f), lightShader->getShaderID(), "src\\assets\\Sphere.dae", POINT_LIGHT);
	Light* light2 = new Light(10.0f, vec3(-250.0f, 40.0f, 200.0f), lightShader->getShaderID(), "src\\assets\\Sphere.dae", POINT_LIGHT);
	Light* light3 = new Light(10.0f, vec3(-250.0f, 40.0f, -200.0f), lightShader->getShaderID(), "src\\assets\\Sphere.dae", POINT_LIGHT);

	lights.push_back(light1);
	lights.push_back(light2);
	lights.push_back(light3);


	// Directional Light
	Light* light4 = new Light(0.2f, vec3(10.0f, 50.0f, 4.0f), lightShader->getShaderID(), "src\\assets\\Sphere.dae", DIRECTIONAL_LIGHT);

	lights.push_back(light4);

	// IMPORTANT: CALL AFTER CREATING ALL LIGHTS IN THE SCENE
	createLightArrays();
	#pragma endregion lights

	#pragma region camera
	// - Camera -
	camera = new Camera(width, height);
	camera->updatePosition(vec3(8.71, 212.48, 810.28));
	camera->updateRotation(vec3(-10.20, -4.05, 0.00));
	#pragma endregion camera

	#pragma region meshes
	// Meshes
	Shader* worldShader = new Shader(WORLD_VERTEX_SHADER, "src\\shaders\\worldFragShader.frag");
	shaders.push_back(worldShader);
	
	

	// O'Connell's Street
	std::vector<Model*> streetModels;
	loadStreet(streetModels, STREET, worldShader->getShaderID());

	std::cout << "The number of street models is: " << streetModels.size() << "\n";
	
	std::vector<const char*> textures;
	textures.push_back("src\\textures\\cobblestone road.jpg");
	textures.push_back("src\\textures\\windows.jpg");
	textures.push_back("src\\textures\\brick texture.jpg");
	textures.push_back("src\\textures\\spire.jpg");
	textures.push_back("src\\textures\\brick texture.jpg");
	textures.push_back("src\\textures\\brick texture.jpg");
	textures.push_back("src\\textures\\brick texture.jpg");
	textures.push_back("src\\textures\\road.jpg");
	textures.push_back("src\\textures\\cobblestone road.jpg");
	textures.push_back("src\\textures\\windows.jpg");

	Material streetMaterial = Material(vec3(1.0, 1.0, 1.0), vec3(1.0, 1.0, 1.0), vec3(0.0f, 0.0f, 0.0f), 1);
	int tex = 0;
	for (Model* model : streetModels) {
		model->updateMaterial(streetMaterial);
		// DONT FORGET TO CHANGE TEXTURE ID HERE
		model->addTexture(textures[tex]);
		models.push_back(model);
		tex++;
	}
	
	#pragma endregion meshes

	#pragma region drones
	// ------------ Drones --------------
	Shader normalShader = Shader(WORLD_VERTEX_SHADER, "src\\shaders\\dronesFragShader.frag");
	Material droneMaterial = Material(vec3(0.2, 0.2, 0.2) * 5, vec3(1.0, 1.0, 1.0), vec3(10.0, 10.0, 10.0), 100);
	srand((unsigned)time(NULL));
	for (int i = 0; i < 20; i++) {
		Boid* drone = new Boid(DROIDBODY, DROIDTOP, normalShader.getShaderID());

		drone->updateMaterial(droneMaterial);
		drone->droidTop->updateMaterial(droneMaterial);
		
		float x = randomNum(-200, 200), y = randomNum(70, 70), z = randomNum(-800, 800);
		vec3 prevPosition = drone->getPosition();
		drone->updateModelPosition(vec3(x,y,z));
		drone->droidTop->updateModelPosition(drone->getPosition() + vec3(0.0, 1.0, 0.0));

		drone->updateVelocity(vec3(randomNum(-1, 1), 0.0f, randomNum(-1, 1)));


		drones.add(*drone);
		models.push_back(drone);
		models.push_back(drone->droidTop);
	}
	#pragma endregion drones

	#pragma region sound
	// Sound 
	ISoundEngine* SoundEngine = createIrrKlangDevice();
	SoundEngine->play2D(SIRENMP3, true);
	
	#pragma endregion sound
	

	glutSetCursor(GLUT_CURSOR_LEFT_ARROW);

}


int main(int argc, char** argv) {
	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("Final Project");

	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	glutKeyboardFunc(camMovement);
	glutPassiveMotionFunc(mousemove);
	glutMouseFunc(mouseClicks);

	// A call to glewInit() must be done after glut is initialized!
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	// Set up your objects and shaders
	init();
	// Begin infinite event loop
	glutMainLoop();
	return 0;
}