#include <boids.h>
#include <boid.h>

#include <stdio.h>

#include <vector>
#include <maths_funcs.h>






Boids::Boids(){

}

void Boids::add(Boid &boid) {
	boid.setBoidID(count);
	boids.push_back(&boid);
	count++;
}

void Boids::update(float timestep)
{
	std::vector<vec3> newVelocities;
	for (Boid* boid : boids) {
		vec3 nextVelocity = boid->getNextVelocity(boids);
		newVelocities.push_back(nextVelocity);
	}

	int i = 0;
	for (Boid* boid : boids) {
		vec3 velocity = newVelocities[i];
		boid->updateVelocity(velocity);
		boid->updateModelPosition(boid->getPosition() + velocity*timestep);
		boid->droidTop->updateModelPosition(boid->getPosition() + vec3(0.0f, 1.0f, 0.0f));

		// Update boid to face velocity - assuming y axis aligned - which I have as it looks better.
		boid->updateModelRotation(vec3(0.0f, ydegs(velocity), 0.0f));

		boid->spinTop(180.0);

		i++;
	}
}

