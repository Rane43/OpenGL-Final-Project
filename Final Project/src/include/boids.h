#ifndef boids_h
#define boids_h

#include <boid.h>
#include <vector>

class Boids {
public:
	std::vector<Boid *> boids;
	int count = 0;

public:
	Boids();

	void add(Boid &boid);

	void update(float timestep);
};

#endif
