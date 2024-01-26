#include <camera.h>
#include <maths_funcs.h>
#include <vector>

/* Edit this to take in focal length and stuff*/
Camera::Camera(int width, int height)
{
	this->width = width;
	this->height = height;

	projMatrix = perspective(45.0f, (float)width / (float)height, 0.1f, 3000.0f);
}


// Getter Functions
mat4 Camera::getViewMatrix() {
	mat4 view = identity_mat4();
	view = translate(view, vec3(0.0f, 0.0f, 0.0f)-position); 
	view = rotate_y_deg(view, -rotation.v[1]); 
	view = rotate_x_deg(view, -rotation.v[0]);
	view = rotate_z_deg(view, -rotation.v[2]);

	return view;
}

mat4 Camera::getProjectionMatrix() {
	return projMatrix;
}

vec3 Camera::getPosition()
{
	return position;
}

vec3 Camera::getRotation()
{
	return rotation;
}


void Camera::setyPosition(float yPos)
{
	position.v[1] = yPos;
}

// Setter Functions - // Need to update
void Camera::setPerspectiveMatrix(float focalLength) {
	projMatrix = perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
}

void Camera::setOrthographicMatrix() {
	projMatrix = identity_mat4();
}

void Camera::updatePosition(vec3 pos)
{
	position = pos;
}

void Camera::updateRotation(vec3 rot)
{
	rotation = rot;
}

void Camera::movePosition(vec3 translationVectorCamFrame)
{
	vec3 translationVectorGlobalFrame = vec3(inverse(getViewMatrix())*vec4(translationVectorCamFrame, 0.0f));
	position = position + translationVectorGlobalFrame;
}

void Camera::rotate_y(float y_degs)
{
	float yDegNew = fmodf(y_degs+rotation.v[1], 360);
	rotation.v[1] = yDegNew;
}


void Camera::rotate_x(float x_degs)
{
	float xDegNew = fmodf(x_degs + rotation.v[0], 360);
	rotation.v[0] = xDegNew;
}

void Camera::rotate_z(float z_degs)
{
	float zDegNew = fmodf(z_degs + rotation.v[2], 360);
	rotation.v[2] = zDegNew;
}

