#ifndef boid_h
#define boid_h

#include <model.h>
#include <vector>
#include <iostream>

class Boid : public Model
{
private:
	int boidID = 0;

	float avoidFactor = 0.05f;
	float matchingFactor = 0.01f;
	float centeringFactor = 0.01f;
	float turnFactor = 1.2f;

	float protectedRadius = 40.0f;
	float visualRadius = 80.0f;

	float minSpeed = 10.0f;
	float maxSpeed = 25.0f;

public:
	Model* droidTop;


	Boid(const char* droidBody, const char* droidTop, unsigned int shaderProgramID);

	// Getter functions
	int getBoidID();


	// Setter functions
	vec3 getNextVelocity(std::vector<Boid *> boids);

	void setBoidID(int bID) {
		boidID = bID;

		std::cout << "Boid ID set to: " << boidID << '\n';

	}

	void updateVelocity(vec3 newVelocity);

	void spinTop(float degrees);
};


#endif