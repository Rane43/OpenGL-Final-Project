#include <boid.h>
#include <vector>
#include <iostream>
#include <maths_funcs.h>

Boid::Boid(const char* droidBody, const char* droidTop, unsigned int shaderProgramID) : Model(droidBody, shaderProgramID)
{
	this->droidTop = new Model(droidTop, shaderProgramID);
}

int Boid::getBoidID()
{
	return boidID;
}


vec3 Boid::getNextVelocity(std::vector<Boid *> boids)
{
	vec3 newVelocity = velocity;

	int neighbouringBoids = 0;

	vec3 avgAwayVector = vec3(0.0f, 0.0f, 0.0f);
	vec3 avgVelocity = vec3(0.0f, 0.0f, 0.0f);
	vec3 avgPosition = vec3(0.0f, 0.0f, 0.0f);

	for (Boid * otherBoid : boids) {
		if (otherBoid->boidID != boidID) {
			vec3 awayVector = getPosition() - otherBoid->getPosition();
			if (length(awayVector) < protectedRadius) {
				// Separation - Steer to avoid collision
				avgAwayVector = avgAwayVector + awayVector;
			}
			else if (length(awayVector) < visualRadius) {
				// Alignment - Steer to match average velocity of the mass
				avgVelocity = avgVelocity + otherBoid->getVelocity();

				// Cohesion - Steer towards center of mass
				avgPosition = avgPosition + otherBoid->getPosition();

				neighbouringBoids++;
			}

		}
	}


	// avgAwayVector = normalise(avgAwayVector); ?
	newVelocity = newVelocity + avgAwayVector * avoidFactor;

	if (neighbouringBoids > 0) {
		avgVelocity = avgVelocity / neighbouringBoids;
		avgPosition = avgPosition / neighbouringBoids;

		newVelocity = newVelocity + (avgVelocity - velocity) * matchingFactor + (avgPosition - position) * centeringFactor;

	}

	if (position.v[0] < -100.0f) {
		newVelocity.v[0] = newVelocity.v[0] + turnFactor;
	}
	else if (position.v[0] > 100.0f) {
		newVelocity.v[0] = newVelocity.v[0] - turnFactor;
	}
	else if (position.v[1] < 50.0f) {
		newVelocity.v[1] = newVelocity.v[1] + turnFactor;
	}
	else if (position.v[1] > 70.0f) {
		newVelocity.v[1] = newVelocity.v[1] - turnFactor;
	}
	else if (position.v[2] < -500.0f) {
		newVelocity.v[2] = newVelocity.v[2] + turnFactor;

	}
	else if (position.v[2] > 500.0f) {
		newVelocity.v[2] = newVelocity.v[2] - turnFactor;

	}

	// Cap speed
	float speed = length(newVelocity);
	if (speed > maxSpeed) {
		newVelocity = normalise(newVelocity) * maxSpeed;
	}
	else if (speed < minSpeed) {
		newVelocity = normalise(newVelocity) * minSpeed;
	}
	
	return newVelocity;

}

void Boid::updateVelocity(vec3 newVelocity)
{
	float speed = length(newVelocity);
	if (speed > maxSpeed) {
		velocity = normalise(newVelocity) * maxSpeed;
	}
	else if (speed < minSpeed) {
		velocity = normalise(newVelocity) * minSpeed;
	}
	else {
		velocity = newVelocity;
	}

}

void Boid::spinTop(float degrees)
{
	droidTop->rotate_y(degrees);
}
