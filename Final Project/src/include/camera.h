#ifndef camera_h
#define camera_h

#include <maths_funcs.h>

class Camera {
private:
	mat4 projMatrix = identity_mat4();

	int width = 0;
	int height = 0;

	vec3 position = vec3(0.0f, 0.0f, 0.0f);
	vec3 rotation = vec3(0.0f, 0.0f, 0.0f);

public:
	Camera(int width, int height);

	// Getter functions
	mat4 getViewMatrix();

	mat4 getProjectionMatrix();

	vec3 getPosition();
	vec3 getRotation();


	// Setter functions
	void setyPosition(float yPos);

	void setPerspectiveMatrix(float focalLength);
	
	void setOrthographicMatrix();

	void updatePosition(vec3 pos);

	void updateRotation(vec3 rot);

	void movePosition(vec3 translationVectorCamFrame);

	void rotate_y(float y_degs);

	void rotate_x(float x_degs);

	void rotate_z(float z_degs);
};

#endif